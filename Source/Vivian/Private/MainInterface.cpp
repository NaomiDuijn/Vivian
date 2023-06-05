// Fill out your copyright notice in the Description page of Project Settings.


#include "MainInterface.h"


UMainInterface::UMainInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

// Set TextOutput to new text
void UMainInterface::SetTextOutput(const FString& NewText)
{
	TextOutput->SetText(FText::FromString(NewText));
    UE_LOG(LogTemp, Display, TEXT("Main Interface - Set text to: %s"), *NewText);
}
// Set LoadingSpinner visibility
void UMainInterface::SetLoadingSpinnerVisibility(const bool bVisible)
{
	if(bVisible)
		LoadingSpinner->SetVisibility(ESlateVisibility::Visible);
	else
		LoadingSpinner->SetVisibility(ESlateVisibility::Collapsed);
}
