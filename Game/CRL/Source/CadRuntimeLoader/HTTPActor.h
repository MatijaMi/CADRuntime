// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "HTTPActor.generated.h"

UCLASS()
class CADRUNTIMELOADER_API AHTTPActor : public AActor
{
	GENERATED_BODY()
	
public:	
	FHttpModule* Http;
	UPROPERTY(BlueprintReadOnly, Category ="HTTP")
	FString ResponseContent;
	UPROPERTY(BlueprintReadWrite, Category = "HTTP")
		bool FileUploadCompleted;
	UPROPERTY(BlueprintReadWrite, Category = "HTTP")
		bool FileDownloadCompleted;



	/* The actual HTTP call */
	UFUNCTION(BlueprintCallable, Category = "HTTP")
		void HttpDownload(FString URL);

	UFUNCTION(BlueprintCallable, Category = "HTTP")
		void HttpUpload(FString URL, FString Content, FString type, FString FileName);
	/*Assign this function to call when the GET request processes sucessfully*/
	void GETResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void POSTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Sets default values for this actor's properties
	AHTTPActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
