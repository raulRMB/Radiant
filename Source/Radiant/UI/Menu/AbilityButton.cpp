// Copyright Radiant Studios


#include "UI/Menu/AbilityButton.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/AbilityDataAsset.h"

void UAbilityButton::NativeConstruct()
{
	RefreshButton();
	Super::NativeConstruct();
}

bool UAbilityButton::Initialize()
{
	RefreshButton();
	return Super::Initialize();
}

void UAbilityButton::RefreshButton()
{
	if(AbilityData)
	{
		if(Icon)
		{
			Icon->SetBrushFromTexture(AbilityData->Icon);
		}
		if(Name)
		{
			Name->SetText(AbilityData->Name);
		}
		if(Tooltip)
		{
			Tooltip->SetText(AbilityData->Tooltip);
		}
	}
}

void UAbilityButton::SynchronizeProperties()
{
	RefreshButton();
	Super::SynchronizeProperties();
}

void UAbilityButton::SetAbilityData(UAbilityDataAsset* Data)
{
	AbilityData = Data;
}
