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
		static void GenerateMeshComponent(AActor* actor, int ComponentID, TArray<FVector> Vertices, FString Component, TArray<FVector2D> TextureCoords, TArray<FVector> Normals, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial, TMap<FString, FString> Materials, bool collision);

	static void GetComponentSections(FString Component, TArray<FString>& Materials, TArray<FString>& Sections);

	static TArray<int> GetFacesFromSection(FString Section);

	static UMaterialInstanceDynamic* GenerateDynamicMaterial(UMyRuntimeMeshComponentStatic* RMC, FString* PartMaterial, TArray<FString> SectionMaterialValues, UMaterialInterface* OpaqueMaterial, UMaterialInterface* TransMaterial);

};
