// Copyright Radiant Studios


#include "UI/Menu/Settings.h"

#include "EnhancedInputSubsystems.h"
#include "KeySelect.h"
#include "Components/InputKeySelector.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void USettings::NativeConstruct()
{
	Super::NativeConstruct();
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
				Key->AddToViewport();
			}
		}
	}

}
