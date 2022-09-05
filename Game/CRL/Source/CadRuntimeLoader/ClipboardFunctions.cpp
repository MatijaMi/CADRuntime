// Fill out your copyright notice in the Description page of Project Settings.


#include "ClipboardFunctions.h"

FString UClipboardFunctions::PasteMessageFromClipboard()
{
	FString ClipboardContent;
	FPlatformMisc::ClipboardPaste(ClipboardContent);
	return ClipboardContent;
}
