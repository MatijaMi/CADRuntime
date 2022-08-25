// Fill out your copyright notice in the Description page of Project Settings.


#include "CRLGenerator.h"


void UCRLGenerator::GenerateMesh(AActor* Actor, FString File, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, bool collision)
{
	TArray<FVector> Vertices;
	TArray<FVector2D> TextureCoords;
	TArray<FVector> Normals;
	TArray<FString> Materials;
	TArray<float> MaterialValues;
	TArray<FString> MaterialNames;
	FString materialFileName = "Models/inosoft.mtl";
	FString materialFile;
	MaterialNames ;
	TArray<FString> triangleStrings;

	URuntimeMeshComponentStatic* rmc = NewObject<URuntimeMeshComponentStatic>(Actor, TEXT("RMC"));
	rmc->AttachToComponent(Actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	rmc->RegisterComponent();
	URuntimeMeshProviderStatic* StaticProvider = NewObject<URuntimeMeshProviderStatic>(rmc, TEXT("StaticProvider"));
	rmc->Initialize(StaticProvider);
	FRuntimeMeshCollisionSettings CS;
	CS.bUseAsyncCooking = true;
	CS.bUseComplexAsSimple = true;
	CS.CookingMode = ERuntimeMeshCollisionCookingMode::CookingPerformance;
	StaticProvider->SetCollisionSettings(CS);

	for (int i = 0; i < MaterialNames.Num(); i++) {
		UMaterialInstanceDynamic* CustomMaterial;
		if (MaterialValues[5 * i + 3] == 1) {
			CustomMaterial = UMaterialInstanceDynamic::Create(OpaqueMaterial, rmc);
			CustomMaterial->SetScalarParameterValue("Specular", MaterialValues[5 * i + 4] / 1000);
		}
		else {
			CustomMaterial = UMaterialInstanceDynamic::Create(TransMaterial, rmc);
			CustomMaterial->SetScalarParameterValue("Opacity", MaterialValues[5 * i + 3]);
		}

		float r = MaterialValues[5 * i];
		float g = MaterialValues[5 * i + 1];
		float b = MaterialValues[5 * i + 2];
		CustomMaterial->SetVectorParameterValue("Colour", FLinearColor(r, g, b));
		StaticProvider->SetupMaterialSlot(i, FName(MaterialNames[i]), CustomMaterial);
	}

	TArray<FColor> Colors{ FColor::Blue, FColor::Red, FColor::Green };
	TArray<FVector> EmptyNormals;
	TArray<FVector2D> EmptyTexCoords;
	TArray<FRuntimeMeshTangent> EmptyTangents;

	for (int i = 0; i < triangleStrings.Num(); i++) {
		TArray<int> triangles;
		StaticProvider->CreateSectionFromComponents(0, i, MaterialNames.IndexOfByKey(Materials[i]), Vertices, triangles, EmptyNormals, TextureCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, collision);
	}
}

void UCRLGenerator::GenerateMeshSection(AActor* actor, int sectionId, TArray<FVector> Vertices, TArray<FString> Sections, TArray<FVector2D> TextureCoords, TArray<FVector> Normals, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, TMap<FString, FString> Materials, bool collision)
{
	UMyRuntimeMeshComponentStatic* RMC = NewObject<UMyRuntimeMeshComponentStatic>(actor);
	RMC->SetIsReplicated(true);
	RMC->AttachToComponent(actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	RMC->RegisterComponent();
	
		URuntimeMeshProviderStatic* StaticProvider = NewObject<URuntimeMeshProviderStatic>(RMC, FName("StaticProvider"));
	FRuntimeMeshCollisionSettings CS;
	CS.bUseAsyncCooking = true;
	CS.bUseComplexAsSimple = true;
	CS.CookingMode = ERuntimeMeshCollisionCookingMode::CookingPerformance;
	StaticProvider->SetCollisionSettings(CS);
	
	TArray<FString> SectionMaterials;
	TArray<FString> Parts;
	FString sep = " ";
	TArray<FString> PartMaterialValues;
	UMaterialInstanceDynamic* CustomMaterial;
	TArray<FColor> Colors{ FColor::Blue, FColor::Red, FColor::Green };
	TArray<FVector> EmptyNormals;
	TArray<FRuntimeMeshTangent> EmptyTangents;
	RMC->SectionId = sectionId;
	RMC->EnableNormalTangentGeneration();
	GetSectionParts(Sections[sectionId], SectionMaterials, Parts);
	float maxX = -FLT_MAX;
	float minX = FLT_MAX;
	float maxY = -FLT_MAX;
	float minY = FLT_MAX;
	float maxZ = -FLT_MAX;
	float minZ = FLT_MAX;

	for (int i = 0; i < Parts.Num(); i++) {
		TArray<int> Faces = GetFacesFromPart(Parts[i]);
		for (auto& num : Faces) {
			float x = Vertices[num].X;
			float y = Vertices[num].Y;
			float z = Vertices[num].Z;
			if (x > maxX) { maxX = x; }
			if (x < minX) { minX = x; }
			if (y > maxY) { maxY = y; }
			if (y < minY) { minY = y; }
			if (z > maxZ) { maxZ = z; }
			if (z < minZ) { minZ = z; }
		}
	}
	FVector Center = FVector((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2);
	RMC->MidVector = Center;
	RMC->SetRelativeLocation(Center);
	int length = Vertices.Num();
	
	TArray<FVector> NormalizedVertices;
	for (int i = 0; i < Vertices.Num(); i++) {
		NormalizedVertices.Add(Vertices[i]-Center);
	}
	
	for (int i = 0; i < Parts.Num(); i++) {
		TArray<int> Faces = GetFacesFromPart(Parts[i]);
		if (SectionMaterials.Num() > 0) {
			FString* PartMaterial = Materials.Find(SectionMaterials[i]);
			if (PartMaterial != NULL) {
				PartMaterial->ParseIntoArray(PartMaterialValues, *sep);
				if (FCString::Atof(*PartMaterialValues[3]) > 0.99) {
					CustomMaterial = UMaterialInstanceDynamic::Create(OpaqueMaterial, RMC);
					CustomMaterial->SetScalarParameterValue("Specular", FCString::Atof(*PartMaterialValues[4]) / 1000);
				}
				else {
					CustomMaterial = UMaterialInstanceDynamic::Create(TransMaterial, RMC);
					CustomMaterial->SetScalarParameterValue("Opacity", FCString::Atof(*PartMaterialValues[3]));
				}
				float r = FCString::Atof(*PartMaterialValues[0]);
				float g = FCString::Atof(*PartMaterialValues[1]);
				float b = FCString::Atof(*PartMaterialValues[2]);
				CustomMaterial->SetVectorParameterValue("Colour", FLinearColor(r, g, b));
				StaticProvider->SetupMaterialSlot(i, FName(SectionMaterials[i]), CustomMaterial);
			}
			else {
				StaticProvider->SetupMaterialSlot(i, FName("NoMaterial"), NULL);
			}
		}
			//RMC->SetVisibility(false, false);
			StaticProvider->CreateSectionFromComponents(0, i, i, NormalizedVertices, Faces, EmptyNormals, TextureCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, collision);
		}
}

void UCRLGenerator::GenerateMeshComponent(AActor* actor, TArray<FVector> vertices, TArray<int> triangles, UMaterialInstanceDynamic* Material, bool collision)
{
	UMyRuntimeMeshComponentStatic* RMC = NewObject<UMyRuntimeMeshComponentStatic>(actor);
	RMC->SetIsReplicated(true);
	RMC->AttachToComponent(actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	RMC->RegisterComponent();
	URuntimeMeshProviderStatic* StaticProvider = NewObject<URuntimeMeshProviderStatic>(RMC, FName("StaticProvider"));
	FRuntimeMeshCollisionSettings CS;
	CS.bUseAsyncCooking = true;
	CS.bUseComplexAsSimple = true;
	CS.CookingMode = ERuntimeMeshCollisionCookingMode::CookingPerformance;
	StaticProvider->SetCollisionSettings(CS);
	StaticProvider->SetupMaterialSlot(0, FName("Material"), NULL);
	TArray<FColor> Colors{ FColor::Blue, FColor::Red, FColor::Green };
	TArray<FVector> EmptyNormals;
	TArray<FVector2D> EmptyTexCoords;
	TArray<FRuntimeMeshTangent> EmptyTangents;
	StaticProvider->CreateSectionFromComponents(0, 0, 0, vertices, triangles, EmptyNormals, EmptyTexCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, collision);
}

void UCRLGenerator::GetDataForSection(FString trianglesIn, TArray<FVector> verticesIn, TArray<FVector>& vertices, TArray<int>& trianglesOut)
{
	TArray<int> trianglesTemp;
	TArray<int> checkedVerts;

	for (int i = 0; i < trianglesTemp.Num(); i++) {
		if (checkedVerts.Contains(trianglesTemp[i])) {
			for (int j = 0; j < checkedVerts.Num(); j++) {
				if (checkedVerts[j] == trianglesTemp[i]) {
					trianglesOut.Add(j);
				}
			}
		}
		else {
			trianglesOut.Add(checkedVerts.Num());
			checkedVerts.Add(trianglesTemp[i]);
			vertices.Add(verticesIn[trianglesTemp[i]]);
		}
	}
}

void UCRLGenerator::GetSectionParts(FString Section, TArray<FString>& Materials, TArray<FString>& Parts)
{
	TArray<FString> InputArray;
	FString sep = "\n";
	Section.ParseIntoArray(InputArray, *sep);
	FString Part = "";
	
	for (auto line : InputArray) {
		if (line.StartsWith("usemtl ")) {
			line.RemoveFromStart("usemtl ");
			Materials.Add(line);
			if (Part != "") {
				Parts.Add(Part);
				Part = "";
			}
		}else{
			Part.Append(line);
			Part.Append("\n");
		}
	}
	Parts.Add(Part);
}

TArray<int> UCRLGenerator::GetFacesFromPart(FString Part)
{
	TArray<int> Faces;
	FString sep = "/";
	TArray<FString> inputArray;
	FString sep1 = "\n";
	Part.ParseIntoArray(inputArray, *sep1);
	for (auto& sting : inputArray) {

		sting.RemoveFromStart("f ");
		FString left;
		FString temp;
		FString mid;
		FString right;

		TArray<FString> one;
		TArray<FString> two;
		TArray<FString> three;

		sting.Split(" ", &left, &temp);
		temp.Split(" ", &mid, &right);

		left.ParseIntoArray(one, *sep);
		mid.ParseIntoArray(two, *sep);
		right.ParseIntoArray(three, *sep);

		int j = FCString::Atoi(*one[0]);
		int k = FCString::Atoi(*two[0]);
		int l = FCString::Atoi(*three[0]);

		j--;
		k--;
		l--;

		Faces.Add(j);
		Faces.Add(k);
		Faces.Add(l);
	}

	return Faces;
}
