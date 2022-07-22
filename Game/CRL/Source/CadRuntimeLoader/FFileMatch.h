// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class CADRUNTIMELOADER_API FFileMatch : public IPlatformFile::FDirectoryVisitor
{
public:
	TArray<FString>& Result;
	FString WildCard;
	bool bFiles;
	bool bDirectories;
	FFileMatch(TArray<FString>& InResult, const FString& InWildCard, bool bInFiles, bool bInDirectories)
		: Result(InResult)
		, WildCard(InWildCard)
		, bFiles(bInFiles)
		, bDirectories(bInDirectories)
	{
	}

	virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory);
};
