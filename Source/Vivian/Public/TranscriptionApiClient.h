// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "TranscriptionApiClient.generated.h"

// Declare a delegate type for processing the response
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTranscriptionResponseProcessed, const FString&, TranscriptionResponseText);
// Use the Game config file to read the API key and base conversation context
UCLASS(Config=Game)
class VIVIAN_API UTranscriptionApiClient: public UObject
{
	GENERATED_BODY()
public:
	UTranscriptionApiClient(const FObjectInitializer& ObjectInitializer);
	virtual ~UTranscriptionApiClient() override = default;
	// Send the audio file to the Transcription API
	UFUNCTION(BlueprintCallable)
	void SendAudioToOpenAI();
	// Custom event to be called when the response text is ready
	UPROPERTY(BlueprintAssignable)
	FTranscriptionResponseProcessed OnTranscriptionResponseProcessed;

protected:
	void OnTranscriptionComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

private:
	FHttpModule* Http;
	// Read the API key from the config file
	UPROPERTY(VisibleAnywhere, Config)
	FString ApiKey;
	// Read the Transcription API URL from the config file
	UPROPERTY(VisibleAnywhere, Config)
	FString TranscriptionApiUrl;
	// Boundary FStrings for setting up the multipart form data
	FString BoundaryLabel = FString();
	FString BoundaryBegin = FString();
	FString BoundaryEnd = FString();
	// Method to add data to the request
	FString AddStringValueToMultipartFormData(FString Name, FString Value);
};
