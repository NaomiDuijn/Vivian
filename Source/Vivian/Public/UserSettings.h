#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "UserSettings.generated.h"

UCLASS()
class VIVIAN_API UUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetMicSensitivity(float NewValue);

	UFUNCTION(BlueprintPure)
	float GetMicSensitivity() const;

	UFUNCTION(BlueprintCallable)
	static UUserSettings* GetVivianGameUserSettings();

protected:
	UPROPERTY(Config)
	float MicSensitivity;
};
