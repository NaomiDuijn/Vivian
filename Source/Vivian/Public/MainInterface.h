// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/MultiLineEditableTextBox.h>

#include "MainInterface.generated.h"

UCLASS()
class VIVIAN_API UMainInterface : public UUserWidget 
{
	GENERATED_BODY()
public:
	UMainInterface(const FObjectInitializer& ObjectInitializer);
	virtual ~UMainInterface() override = default;
	// The response text box in the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMultiLineEditableTextBox* TextOutput;
	// Loading spinner in the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWidget* LoadingSpinner;
	// Set TextOutput
	UFUNCTION(BlueprintCallable)
	void SetTextOutput(const FString& NewText) ;
	// Set LoadingSpinner
	UFUNCTION(BlueprintCallable)
	void SetLoadingSpinnerVisibility(bool bVisible) ;
};
