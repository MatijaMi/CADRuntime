// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Providers/RuntimeMeshProviderStatic.h"
#include "MyRuntimeMeshComponentStatic.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/RuntimeMeshComponentStatic.h"
#include "CADParser.h"
#include "CRLGenerator.generated.h"

/**
 * 
 */
UCLASS()
class CADRUNTIMELOADER_API UCRLGenerator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static void GenerateMesh(AActor* Actor, FString File, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, bool collision);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static void GenerateMeshSection(AActor* actor, int sectionId, TArray<FVector> vertices, TArray<FString> Section, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, TMap<FString, FString> Materials, bool collision);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static void GenerateMeshComponent(AActor* actor, TArray<FVector> vertices, TArray<int> triangles, UMaterialInstanceDynamic* Material, bool collision);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static void GetDataForSection(FString trianglesIn, TArray < FVector> verticesIn, TArray<FVector>& vertices, TArray<int>& trianglesOut);

	static void GetSectionParts(FString Section, TArray<FString>& Materials, TArray<FString>& Parts);
	static TArray<int> GetFacesFromPart(FString Part);
};
