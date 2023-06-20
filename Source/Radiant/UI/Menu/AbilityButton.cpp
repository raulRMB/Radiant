// Copyright Radiant Studios


#include "UI/Menu/AbilityButton.h"

#include "LevelUp.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/AbilityDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "UI/RTHUD.h"
#include "Util/Util.h"

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
	Button->OnClicked.AddUniqueDynamic(this, &UAbilityButton::OnButtonClicked);	
}

void UAbilityButton::OnButtonClicked()
{
	OnRemoveAbility.ExecuteIfBound(AbilityData);
	
	if(ARTHUD* HUD = GetWorld()->GetFirstPlayerController()->GetHUD<ARTHUD>())
	{
		HUD->GiveAbilityFromButton.ExecuteIfBound(AbilityData);
	}
}
