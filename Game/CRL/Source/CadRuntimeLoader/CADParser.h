// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Runtime\Core\Public\Misc\FileHelper.h>
#include "Providers/RuntimeMeshProviderStatic.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/RuntimeMeshComponentStatic.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "CADParser.generated.h"

/**
 * 
 */
UCLASS()
class CADRUNTIMELOADER_API UCADParser : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static FString LoadFileToString(FString Filename);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static TArray<int> GetTriangleFromString(FString input);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static TArray<FString> GetArraysFromString(FString Input, TArray<FVector>& vertices, TArray<FVector2D>& textureCoords, TArray<FVector>& Normals, TArray<FString>& materials);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static void GenerateMesh(AActor* Actor, FString File, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, bool collision);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static void GenerateMeshSection(AActor* actor, int sectionId, TArray<FVector> vertices, TArray<int> triangles, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, TArray<FString> Materials, TArray<float> MaterialValues, TArray<FString> MaterialNames, bool collision);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static void GetDataForSection(FString trianglesIn, TArray < FVector> verticesIn, TArray<FVector>& vertices, TArray<int>& trianglesOut);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static TArray<FString> getMaterialsFromString(FString input, TArray<float>& materialValues);
};
