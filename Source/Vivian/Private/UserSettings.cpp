#include "UserSettings.h"

UUserSettings::UUserSettings(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	MicSensitivity = 0.2f;
}

void UUserSettings::SetMicSensitivity(float NewValue)
{
	MicSensitivity = NewValue;
}

float UUserSettings::GetMicSensitivity() const
{
	return MicSensitivity;
}

UUserSettings* UUserSettings::GetBetterGameUserSettings()
{
	return Cast<UUserSettings>(UGameUserSettings::GetGameUserSettings());
}