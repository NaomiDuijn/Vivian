#include "UserSettings.h"

UUserSettings::UUserSettings(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	// Check if the config file has a value for MicSensitivity
	if (GConfig->GetFloat(TEXT("/Script/Vivian.UserSettings"), TEXT("MicSensitivity"), MicSensitivity, GGameUserSettingsIni))
	{
		// If it does, use that value
	}
	else
	{
		// If it doesn't, set the default value
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