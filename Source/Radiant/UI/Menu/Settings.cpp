// Copyright Radiant Studios


#include "UI/Menu/Settings.h"

#include "EnhancedInputSubsystems.h"
#include "KeySelect.h"
#include "Components/InputKeySelector.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Avatar.h"
#include "Util/UserSettings.h"
#include "Util/Util.h"

void USettings::OnCameraSpeedChanged(float Value)
{
	if(AAvatar* Player = UGameplayStatics::GetPlayerController(this, 0)->GetPawn<AAvatar>())
	{
		CameraSpeedText->SetText(RTPRINTF("%.0f", Value * 10000.f));
		Player->CameraMovementSpeed = Value * 10000.f;
	}
}

void USettings::NativeConstruct()
{
	Super::NativeConstruct();

	CameraSpeedSlider->OnValueChanged.AddUniqueDynamic(this, &USettings::OnCameraSpeedChanged);

	if(UGameplayStatics::DoesSaveGameExist("UserSettings", 0))
	{
		if(UUserSettings* Save = Cast<UUserSettings>(UGameplayStatics::LoadGameFromSlot("UserSettings", 0)))
		{
			CameraSpeedText->SetText(RTPRINTF("%.0f", Save->CameraSpeed));
			CameraSpeedSlider->SetValue(Save->CameraSpeed / 10000.f);
		}
	}
}

void USettings::RefreshKeybindList()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer()))
	{
		TArray<FEnhancedActionKeyMapping> Mappings = Subsystem->GetAllPlayerMappableActionKeyMappings();
		KeyList->ClearChildren();
		for(auto Mapping : Mappings)
		{
			UKeySelect* Key = CreateWidget<UKeySelect>(GetWorld(), KeySelectClass);
			if(KeyList && Key)
			{
				Key->SetMapping(Mapping);
				KeyList->AddChild(Key);
			}
		}
	}
}
