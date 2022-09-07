// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Runtime\Core\Public\Misc\FileHelper.h>
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
		static TArray<FString>  OpenFolder(FString Directory);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static TArray<FString> ParseFile(FString Input, TArray<FVector>& vertices, TArray<FVector2D>& textureCoords, TArray<FVector>& Normals);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static TArray<FString> ParseOBJ(FString Input, TArray<FVector>& vertices, TArray<FVector2D>& textureCoords, TArray<FVector>& Normals);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static TMap<FString, FString> ParseOBJMaterial(FString input);

	UFUNCTION(BlueprintCallable, Category = " File I/O")
		static TArray<FString> ParseSTL(FString Input, TArray<FVector>& vertices, TArray<FVector2D>& textureCoords, TArray<FVector>& Normals);

};
