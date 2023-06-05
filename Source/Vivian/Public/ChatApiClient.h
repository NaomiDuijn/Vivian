// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "ChatApiClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChatResponseProcessed, const FString&, ChatResponseText);
// Use the Game config file to read the API key and base conversation context
UCLASS(Config=Game)
class VIVIAN_API UChatApiClient: public UObject
{
	GENERATED_BODY()
public:
	UChatApiClient(const FObjectInitializer& ObjectInitializer);
	virtual ~UChatApiClient() override = default;
	// Send a text message to the Chat API
	UFUNCTION(BlueprintCallable)
	void SendOpenAIChatRequest(const FString& InputText);
	// Custom event to be called when the response text is ready
	UPROPERTY(BlueprintAssignable)
	FChatResponseProcessed OnChatResponseProcessed;

protected:
	void HandleAPIResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

private:
	FHttpModule* Http;
	// Read the API key from the config file
	UPROPERTY(VisibleAnywhere, Config)
	FString ApiKey;
	// Read the Chat API URL from the config file
	UPROPERTY(VisibleAnywhere, Config)
	FString ChatApiUrl;
	// Read the base conversation context from the config file
	UPROPERTY(VisibleAnywhere, Config)
	FString BaseConversationContext;
};
