#include "UserSettings.h"

UUserSettings::UUserSettings(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	// Check if the config file has a value for MicSensitivity, if not, set it to 0.1
	if (!GConfig->GetFloat(TEXT("/Script/Vivian.UserSettings"), TEXT("MicSensitivity"), MicSensitivity, GGameUserSettingsIni))
	{
		MicSensitivity = 0.1f;
	}
}

void UUserSettings::SetMicSensitivity(float NewValue)
{
	MicSensitivity = NewValue;
}

float UUserSettings::GetMicSensitivity() const
{
	return MicSensitivity;
}

UUserSettings* UUserSettings::GetVivianGameUserSettings()
{
	return Cast<UUserSettings>(GetGameUserSettings());
}