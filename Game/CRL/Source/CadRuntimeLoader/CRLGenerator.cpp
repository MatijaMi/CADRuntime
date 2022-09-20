// Fill out your copyright notice in the Description page of Project Settings.


#include "CRLGenerator.h"



void UCRLGenerator::GenerateMeshComponent(AActor* actor, int ComponentID, TArray<FVector> Vertices, FString Component, TArray<FVector2D> TextureCoords, TArray<FVector> Normals, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, TMap<FString, FString> Materials, bool collision)
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
	
	TArray<FString> ComponentMaterials;
	TArray<FString> Sections;
	FString sep = " ";
	TArray<FString> SectionMaterialValues;
	UMaterialInstanceDynamic* CustomMaterial;

	TArray<FColor> Colors{ FColor::Blue, FColor::Red, FColor::Green };
	TArray<FVector> EmptyNormals;
	TArray<FRuntimeMeshTangent> EmptyTangents;

	RMC->SectionId = ComponentID;
	RMC->EnableNormalTangentGeneration();
	GetComponentSections(Component, ComponentMaterials, Sections);
	float maxX = -FLT_MAX;
	float minX = FLT_MAX;
	float maxY = -FLT_MAX;
	float minY = FLT_MAX;
	float maxZ = -FLT_MAX;
	float minZ = FLT_MAX;
	
	for (int i = 0; i < Sections.Num(); i++) {
		TArray<int> Faces = GetFacesFromSection(Sections[i]);
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
	
	for (int i = 0; i < Sections.Num(); i++) {
		TArray<int> Faces = GetFacesFromSection(Sections[i]);
		if (ComponentMaterials.Num() > 0) {
			FString* PartMaterial = Materials.Find(ComponentMaterials[i]);
			if (PartMaterial != NULL) {
				PartMaterial->ParseIntoArray(SectionMaterialValues, *sep);
				if (FCString::Atof(*SectionMaterialValues[3]) > 0.99) {
					CustomMaterial = UMaterialInstanceDynamic::Create(OpaqueMaterial, RMC);
					CustomMaterial->SetScalarParameterValue("Specular", FCString::Atof(*SectionMaterialValues[4]) / 1000);
				}
				else {
					CustomMaterial = UMaterialInstanceDynamic::Create(TransMaterial, RMC);
					CustomMaterial->SetScalarParameterValue("Opacity", FCString::Atof(*SectionMaterialValues[3]));
				}
				float r = FCString::Atof(*SectionMaterialValues[0]);
				float g = FCString::Atof(*SectionMaterialValues[1]);
				float b = FCString::Atof(*SectionMaterialValues[2]);
				CustomMaterial->SetVectorParameterValue("Colour", FLinearColor(r, g, b));
				StaticProvider->SetupMaterialSlot(i, FName(ComponentMaterials[i]), CustomMaterial);
			}
			else {
				StaticProvider->SetupMaterialSlot(i, FName("NoMaterial"), NULL);
			}
		}
			StaticProvider->CreateSectionFromComponents(0, i, i, NormalizedVertices, Faces, EmptyNormals, TextureCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, collision);
		}
		
}

void UCRLGenerator::GetComponentSections(FString Component, TArray<FString>& Materials, TArray<FString>& Sections)
{
	TArray<FString> InputArray;
	FString sep = "\n";
	Component.ParseIntoArray(InputArray, *sep);
	FString Section = "";
	
	for (auto line : InputArray) {
		if (line.StartsWith("usemtl ")) {
			line.RemoveFromStart("usemtl ");
			Materials.Add(line);
			if (Section != "") {
				Sections.Add(Section);
				Section = "";
			}
		}else{
			Section.Append(line);
			Section.Append("\n");
		}
	}
	Sections.Add(Section);
}

TArray<int> UCRLGenerator::GetFacesFromSection(FString Section)
{
	TArray<int> Faces;
	FString sep = "/";
	TArray<FString> inputArray;
	FString sep1 = "\n";
	Section.ParseIntoArray(inputArray, *sep1);
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
