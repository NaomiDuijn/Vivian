// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainInterface.h"
#include "VivianGameState.generated.h"

// In Unreal Engine, the GameState acts as the Model in the Model-View-Controller pattern.
UCLASS()
class VIVIAN_API AVivianGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AVivianGameState();
	// Getter for MainInterface
	UFUNCTION(BlueprintCallable)
	UMainInterface* GetMainInterface() const;
	virtual ~AVivianGameState() override = default;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ResponseText;
	// Start play
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<UMainInterface> MainInterfaceClass;
	// MainInterface widget class
	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	UMainInterface* MainInterface;
};
