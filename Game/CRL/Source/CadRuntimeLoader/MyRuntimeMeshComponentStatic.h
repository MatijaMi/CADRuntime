// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RuntimeMeshComponentStatic.h"
#include "MyRuntimeMeshComponentStatic.generated.h"

/**
 * 
 */
UCLASS()
class CADRUNTIMELOADER_API UMyRuntimeMeshComponentStatic : public URuntimeMeshComponentStatic
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "RMC")
		FString ComponentName;

	UPROPERTY(BlueprintReadOnly, Category = "RMC")
		FVector MidVector;
	UPROPERTY(BlueprintReadOnly, Category = "RMC")
		int SectionId;

	UPROPERTY(BlueprintReadOnly, Category = "RMC")
		TArray<int> Faces;

	UPROPERTY(BlueprintReadOnly, Category = "RMC")
		TArray<UMaterialInstanceDynamic*> Material;


};
