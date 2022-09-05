// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ClipboardFunctions.generated.h"

/**
 * 
 */
UCLASS()
class CADRUNTIMELOADER_API UClipboardFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Clipboard")
		static FString PasteMessageFromClipboard();
};
