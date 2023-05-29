// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VivianGameModeBase.generated.h"


class UMainInterface;
class UTranscriptionApiClient;
class UChatApiClient;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSetNewResponseText);

// In Unreal Engine, the GameMode acts as the Controller in the Model-View-Controller pattern.
UCLASS()
class VIVIAN_API AVivianGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AVivianGameModeBase();
	virtual ~AVivianGameModeBase() override = default;
	UFUNCTION(BlueprintPure, Category = "Response")
	FString GetResponseText() const;
	UFUNCTION(BlueprintCallable, Category = "Response")
	void SetResponseText(const FString& NewResponseText);
	// Send the audio to the Transcription API
	UFUNCTION(BlueprintCallable, Category = "Api")
	void Transcribe() const;
	// Send a text message to the Chat API
	UFUNCTION(BlueprintCallable, Category = "Api")
	void Chat(const FString& InputText);
	// Custom event to be called when the response text is ready
	UPROPERTY(BlueprintAssignable)
	FSetNewResponseText OnSetNewResponseText;
	// Start play
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Api", meta = (AllowPrivateAccess = "true"))
	UTranscriptionApiClient* TranscriptionApiClient;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Api", meta = (AllowPrivateAccess = "true"))
	UChatApiClient* ChatApiClient;
};
