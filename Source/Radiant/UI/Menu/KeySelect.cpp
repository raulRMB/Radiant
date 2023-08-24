// Copyright Radiant Studios


#include "UI/Menu/KeySelect.h"

#include "EnhancedInputSubsystems.h"
#include "Settings.h"
#include "Components/InputKeySelector.h"
#include "Components/TextBlock.h"
#include "Event/EventBroker.h"
#include "Player/RTPlayerController.h"

void UKeySelect::NativeConstruct()
{
	Super::NativeConstruct();
	Keybind->OnKeySelected.AddDynamic(this, &UKeySelect::RebindKey);
}

void UKeySelect::RebindKey(FInputChord SelectedKey)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer()))
	{
		Subsystem->AddPlayerMappedKey(Mapping.PlayerMappableOptions.Name, SelectedKey.Key);
		Cast<ARTPlayerController>(GetOwningLocalPlayer()->PlayerController)->SaveUserSettingsDelay();
		UEventBroker::Get(this)->KeybindChanged.Broadcast();
	}
}

void UKeySelect::SetMapping(FEnhancedActionKeyMapping NewMapping)
{
	Mapping = NewMapping;
	KeyText->SetText(Mapping.PlayerMappableOptions.DisplayName);
	Keybind->SetSelectedKey(Mapping.Key);
}
