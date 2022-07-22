// Fill out your copyright notice in the Description page of Project Settings.


#include "FFileMatch.h"

bool FFileMatch::Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
{

	// Pass back the file name
	if (bIsDirectory) {
		new(Result)FString("\\" + FPaths::GetCleanFilename(FilenameOrDirectory));
	}
	else {
		new(Result)FString(FPaths::GetCleanFilename(FilenameOrDirectory));
	}
	

	return true;
}
