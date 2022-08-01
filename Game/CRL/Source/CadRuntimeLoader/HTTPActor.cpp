// Fill out your copyright notice in the Description page of Project Settings.


#include "HTTPActor.h"

// Sets default values
AHTTPActor::AHTTPActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Http = &FHttpModule::Get();
}

void AHTTPActor::BeginPlay()
{
}

void AHTTPActor::Tick(float DeltaTime)
{
}

void AHTTPActor::HttpDownload(FString URL)
{
	FHttpRequestRef Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHTTPActor::GETResponseReceived);
	//This is the url on which to process the request
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void AHTTPActor::HttpUpload(FString URL, FString Content,FString type,FString FileName)
{
	FHttpRequestRef Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHTTPActor::POSTResponseReceived);
	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", type);
	Request->SetHeader("FileName", FileName);
	Request->SetContentAsStreamedFile(Content);
	Request->ProcessRequest();

}

/*Assigned function on successfull http call*/
void AHTTPActor::POSTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString content = Response->GetContentAsString();
	NotifyUploadDone();
}

void AHTTPActor::GETResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	ResponseContent = Response->GetContentAsString();
	ResponseURL = Response->GetURL();
	NotifyDownloadDone();
}