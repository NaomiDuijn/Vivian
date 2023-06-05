// Fill out your copyright notice in the Description page of Project Settings.


#include "VivianGameState.h"

// In Unreal Engine, the GameState acts as the Model in the Model-View-Controller pattern.
AVivianGameState::AVivianGameState()
{}

void AVivianGameState::BeginPlay()
{
	if(IsValid(MainInterfaceClass))
	{
		// Initialize MainInterface widget
		MainInterface = CreateWidget<UMainInterface>(GetWorld(), MainInterfaceClass);
		if(MainInterface != nullptr)
			UE_LOG(LogTemp, Display, TEXT("Main Interface is valid"));
	}
	else
		UE_LOG(LogTemp, Display, TEXT("Main Interface is not valid"));
	Super::BeginPlay();
}

UMainInterface* AVivianGameState::GetMainInterface() const
{
	return MainInterface;
}

