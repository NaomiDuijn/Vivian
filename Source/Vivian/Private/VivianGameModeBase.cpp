// Fill out your copyright notice in the Description page of Project Settings.


#include "VivianGameModeBase.h"

#include "ChatApiClient.h"
#include "MainInterface.h"
#include "TranscriptionApiClient.h"
#include "VivianGameState.h"

// In Unreal Engine, the GameMode acts as the Controller in the Model-View-Controller pattern.
AVivianGameModeBase::AVivianGameModeBase()
{
	GameStateClass = AVivianGameState::StaticClass();
	// Construct api classes
	ChatApiClient = CreateDefaultSubobject<UChatApiClient>(TEXT("ChatApiClient"));
	TranscriptionApiClient = CreateDefaultSubobject<UTranscriptionApiClient>(TEXT("TranscriptionApiClient"));
}

void AVivianGameModeBase::BeginPlay()
{
	// Check if the api clients are valid
	if(TranscriptionApiClient != nullptr)
		UE_LOG(LogTemp, Display, TEXT("TranscriptionApiClient is valid"));
	if(ChatApiClient != nullptr)
		UE_LOG(LogTemp, Display, TEXT("ChatApiClient is valid"));
		
	// Bind the api client events to the game mode functions
	TranscriptionApiClient->OnTranscriptionResponseProcessed.AddDynamic(this, &AVivianGameModeBase::Chat);
	ChatApiClient->OnChatResponseProcessed.AddDynamic(this, &AVivianGameModeBase::SetResponseText);
	Super::BeginPlay();
}

FString AVivianGameModeBase::GetResponseText() const
{
	return GetGameState<AVivianGameState>()->ResponseText;
}

void AVivianGameModeBase::SetResponseText(const FString& ResponseText)
{
	GetGameState<AVivianGameState>()->ResponseText = ResponseText;
	// Log the response to the console
    UE_LOG(LogTemp, Display, TEXT("GMBase - Response Text: %s"), *ResponseText);
	// Set the text of the text box in the main interface widget
	GetGameState<AVivianGameState>()->GetMainInterface()->SetTextOutput(ResponseText);
	GetGameState<AVivianGameState>()->GetMainInterface()->SetLoadingSpinnerVisibility(false);
	// Broadcast the event
	OnSetNewResponseText.Broadcast();
}
// Call the api client to SendAudioToOpenAI and then call the api client to SendOpenAIChatRequest
void AVivianGameModeBase::Transcribe() const
{
	// IMainInterfaceInteraction *MainInterface = GetGameState<AVivianGameState>()->MainInterface->Construct();
	GetGameState<AVivianGameState>()->GetMainInterface()->SetLoadingSpinnerVisibility(true);
	GetGameState<AVivianGameState>()->GetMainInterface()->SetTextOutput("");
	// Call the api client to SendAudioToOpenAI
	TranscriptionApiClient->SendAudioToOpenAI();
}
void AVivianGameModeBase::Chat(const FString& InputText)
{
	// Log the response to the console
	UE_LOG(LogTemp, Display, TEXT("GMBase - Chat Input Text: %s"), *InputText);
	ChatApiClient->SendOpenAIChatRequest(InputText);
}
