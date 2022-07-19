// Fill out your copyright notice in the Description page of Project Settings.


#include "CADParser.h"

FString UCADParser::LoadFileToString(FString Filename) {
	FString directory = FPaths::ProjectContentDir();
	FString result;
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();

	if (file.CreateDirectory(*directory)) {
		FString myFile = directory + "/" + Filename;
		FFileHelper::LoadFileToString(result, *myFile);
	}
	return result;

}


TArray<int> UCADParser::GetTriangleFromString(FString input)
{
	TArray<int> triangle;
	FString sep = "/";
	TArray<FString> inputArray;
	FString sep1 = "\n";
	input.ParseIntoArray(inputArray, *sep1);

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

		triangle.Add(j);
		triangle.Add(k);
		triangle.Add(l);
	}
	return triangle;
}

TArray<FString> UCADParser::GetArraysFromString(FString Input, TArray<FVector>& vertices, TArray<FVector2D>& textureCoords, TArray<FVector>& normals, TArray<FString>& materials)
{
	TArray<FString> inputArray;
	FString sep = "\n";
	Input.ParseIntoArray(inputArray, *sep);
	FString output = "";
	TArray<FString> triangles;
	TArray<FString> workingArray;
	FString currentMaterial;
	FString loopSeparator = " ";
	float maxX = -FLT_MAX;
	float minX = FLT_MAX;
	float maxY = -FLT_MAX;
	float minY = FLT_MAX;
	float maxZ = -FLT_MAX;
	float minZ = FLT_MAX;


	for (auto line : inputArray) {
		workingArray.Empty();
		if (line.StartsWith("v ")) {
			line.RemoveFromStart("v ");
			line.ParseIntoArray(workingArray, *loopSeparator);
			float x = FCString::Atof(*workingArray[0]);
			float y = FCString::Atof(*workingArray[1]);
			float z = FCString::Atof(*workingArray[2]);
			if (x > maxX) { maxX = x; }
			if (x < minX) { minX = x; }
			if (y > maxY) { maxY = y; }
			if (y < minY) { minY = y; }
			if (z > maxZ) { maxZ = z; }
			if (z < minZ) { minZ = z; }

			vertices.Add(FVector(x, y, z));
		}
		else {
			if (line.StartsWith("vt ")) {
				line.RemoveFromStart("vt ");
				line.ParseIntoArray(workingArray, *loopSeparator);
				float x = FCString::Atof(*workingArray[0]);
				float y = FCString::Atof(*workingArray[1]);
				textureCoords.Add(FVector2D(x, y));
			}
			else {
				if (line.StartsWith("vn ")) {
					line.RemoveFromStart("vn ");
					line.ParseIntoArray(workingArray, *loopSeparator);
					float x = FCString::Atof(*workingArray[0]);
					float y = FCString::Atof(*workingArray[1]);
					float z = FCString::Atof(*workingArray[2]);
					normals.Add(FVector(x, y, z));
				}
				else {
					if ((line.StartsWith("usemtl ") || line.StartsWith("g "))) {
						if (output != "") {
							materials.Add(currentMaterial);
							triangles.Add(output);
							output = "";
						}
						if (line.StartsWith("usemtl ")) {
							line.RemoveFromStart("usemtl ");
							currentMaterial = line;
						}
					}
					else {
						if (line.StartsWith("f ")) {
							output.Append(line);
							output.Append("\n");
						}
					}
				}
			}
		}
	}
	float maxDiff = abs(maxX - minX);
	if (abs(maxY - minY) > maxDiff) {
		maxDiff = abs(maxY - minY);
	}
	else {
		if (abs(maxZ - minZ) > maxDiff) {
			maxDiff = abs(maxZ - minZ);
		}
	}

	for (auto& vertex : vertices) {
		vertex.X = -(2 * ((vertex.X - minX) / (maxX - minX)) - 1) * (500 * ((abs(maxX - minX)) / maxDiff));
		vertex.Y = (2 * ((vertex.Y - minY) / (maxY - minY)) - 1) * (500 * ((abs(maxY - minY)) / maxDiff));
		vertex.Z = (2 * ((vertex.Z - minZ) / (maxZ - minZ)) - 1) * (500 * ((abs(maxZ - minZ)) / maxDiff));
	}
	triangles.Add(output);
	materials.Add(currentMaterial);
	return triangles;
}

void UCADParser::GenerateMesh(AActor* Actor, FString File, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, bool collision)
{
	TArray<FVector> Vertices;
	TArray<FVector2D> TextureCoords;
	TArray<FVector> Normals;
	TArray<FString> Materials;
	TArray<float> MaterialValues;
	TArray<FString> MaterialNames;
	FString materialFileName = "Models/inosoft.mtl";
	FString materialFile = LoadFileToString(materialFileName);
	MaterialNames = getMaterialsFromString(materialFile, MaterialValues);
	TArray<FString> triangleStrings = GetArraysFromString(File, Vertices, TextureCoords, Normals, Materials);

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
		TArray<int> triangles = GetTriangleFromString(triangleStrings[i]);
		StaticProvider->CreateSectionFromComponents(0, i, MaterialNames.IndexOfByKey(Materials[i]), Vertices, triangles, EmptyNormals, TextureCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, collision);
	}
}

void UCADParser::GenerateMeshSection(AActor* actor, int sectionId, TArray<FVector> vertices, TArray<int> triangles, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, TArray<FString> Materials, TArray<float> MaterialValues, TArray<FString> MaterialNames, bool collision)
{
	URuntimeMeshComponentStatic* rmc = NewObject<URuntimeMeshComponentStatic>(actor);
	rmc->AttachToComponent(actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	rmc->RegisterComponent();
	URuntimeMeshProviderStatic* StaticProvider = NewObject<URuntimeMeshProviderStatic>(rmc, FName("StaticProvider"));
	UMaterialInstanceDynamic* CustomMaterial;
	int i;
	if (MaterialNames.IndexOfByKey(Materials[sectionId]) != -1) {
		i = MaterialNames.IndexOfByKey(Materials[sectionId]);
	}
	else {
		i = 0;
	}

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
	StaticProvider->SetupMaterialSlot(0, FName(MaterialNames[i]), CustomMaterial);


	TArray<FColor> Colors{ FColor::Blue, FColor::Red, FColor::Green };
	TArray<FVector> EmptyNormals;
	TArray<FVector2D> EmptyTexCoords;
	TArray<FRuntimeMeshTangent> EmptyTangents;
	StaticProvider->CreateSectionFromComponents(0, 0, 0, vertices, triangles, EmptyNormals, EmptyTexCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, collision);
}

void UCADParser::GetDataForSection(FString trianglesIn, TArray<FVector> verticesIn, TArray<FVector>& vertices, TArray<int>& trianglesOut)
{
	TArray<int> trianglesTemp = GetTriangleFromString(trianglesIn);
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

TArray<FString> UCADParser::getMaterialsFromString(FString input, TArray<float>& materialValues)
{
	TArray<FString> inputArray;
	FString sep = "\n";
	input.ParseIntoArray(inputArray, *sep);
	FString output = "";
	TArray<FString> materialNames;
	TArray<FString> workingArray;
	FString loopSeparator = " ";


	for (auto line : inputArray) {
		workingArray.Empty();
		if (line.StartsWith("newmtl ")) {
			line.RemoveFromStart("newmtl ");
			materialNames.Add(line);
		}
		else {
			if (line.StartsWith("Kd ")) {
				line.RemoveFromStart("Kd ");
				line.ParseIntoArray(workingArray, *loopSeparator);
				for (auto num : workingArray) {
					materialValues.Add(FCString::Atof(*num));
				}

			}
			else {
				if (line.StartsWith("d ")) {
					line.RemoveFromStart("d ");
					materialValues.Add(FCString::Atof(*line));
				}
				else {
					if (line.StartsWith("Ns ")) {
						line.RemoveFromStart("Ns ");
						materialValues.Add(FCString::Atof(*line));
					}
				}

			}
		}
	}
	return materialNames;
}
