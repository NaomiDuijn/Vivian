// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "HttpModule.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Sound/SoundWave.h"
#include <Components/MultiLineEditableTextBox.h>
#include "ApiClient.generated.h"

// Declare a delegate type for processing the response
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResponseProcessed);
// Use the Game config file to read the API key and base conversation context
UCLASS(Config=Game)
class VIVIAN_API UApiClient : public UUserWidget
{
    GENERATED_BODY()

public:
    UApiClient(const FObjectInitializer& ObjectInitializer);

    // Return the response text
    UFUNCTION(BlueprintCallable)
        FString GetResponseText() const;
    UFUNCTION(BlueprintCallable)
        void SendAudioToOpenAI();
    // Custom event to be called when the response text is ready
    UPROPERTY(BlueprintAssignable)
    FResponseProcessed OnResponseProcessed;
    // The response text box in the UI
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
        UMultiLineEditableTextBox* ApiTextOutput;
    // Loading spinner in the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
        UWidget* LoadingSpinner;
    
protected:
    void SendOpenAIChatRequest(const FString& InputText);
    void HandleAPIResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void OnTranscriptionComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

private:
    FHttpModule* Http;
    // Read the API key from the config file
    UPROPERTY(VisibleAnywhere, Config)
        FString ApiKey;
    // Read the base conversation context from the config file
    UPROPERTY(VisibleAnywhere, Config)
        FString BaseConversationContext;
    // Read the Chat API URL from the config file
    UPROPERTY(VisibleAnywhere, Config)
    FString ChatApiUrl;
    // Read the Transcription API URL from the config file
    UPROPERTY(VisibleAnywhere, Config)
    FString TranscriptionApiUrl;
    // Boundary FStrings for setting up the multipart form data
    FString BoundaryLabel = FString();
    FString BoundaryBegin = FString();
    FString BoundaryEnd = FString();
    // Method to add data to the request
    FString AddStringValueToMultipartFormData(FString Name, FString Value);
    // Response text variable
    FString ResponseText;
    // Method to set the response text
    void SetResponseText(const FString& Response);
};
