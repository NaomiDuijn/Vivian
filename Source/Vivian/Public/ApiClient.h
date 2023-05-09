// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Http.h"
#include "HttpModule.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Sound/SoundWave.h"
#include <Components/MultiLineEditableTextBox.h>
#include <Components/EditableTextBox.h>
#include "ApiClient.generated.h"

// Use the Game config file to read the API key
UCLASS(Config=Game)
class VIVIAN_API UApiClient : public UUserWidget
{
    GENERATED_BODY()

public:
    UApiClient(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
        void MakeAPIRequest();
    UFUNCTION(BlueprintCallable)
        void SetResponseText(FString Response);
    FString AddData(FString Name, FString Value);
    UFUNCTION(BlueprintCallable)
        void SendAudioToOpenAI();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
        UEditableTextBox* ApiTextInput;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
        UMultiLineEditableTextBox* ApiTextOutput;

protected:
    // Call the Api 
    void HandleAPIResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void OnTranscriptionComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

private:
    FHttpModule* Http;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Private", meta= (AllowPrivateAccess = "true"))
    FString InputText;
    // Read the API key from the config file
    UPROPERTY(VisibleAnywhere, Config)
        FString ApiKey;
    UPROPERTY(VisibleAnywhere, Config)
        FString BaseConversationContext;

    FString BoundaryLabel = FString();
    FString BoundaryBegin = FString();
    FString BoundaryEnd = FString();
};
