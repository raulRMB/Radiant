// Copyright Radiant Studios


#include "UI/Menu/LevelUp.h"

#include "AbilityButton.h"
#include "Components/ScrollBox.h"

void ULevelUp::NativeConstruct()
{
	Super::NativeConstruct();
}

bool ULevelUp::Initialize()
{
	return Super::Initialize();
}

void ULevelUp::RefreshList()
{
	AbilityList->ClearChildren();
	for(auto Ability : Abilities)
	{
		UAbilityButton* Button = CreateWidget<UAbilityButton>(GetWorld(), AbilityButtonClass);
		if(Ability && Button)
		{
			Button->SetAbilityData(Ability);
			Button->OnRemoveAbility.BindUObject(this, &ULevelUp::RemoveAbility);
			AbilityList->AddChild(Button);
		}
	}
}

void ULevelUp::RemoveAbility(UAbilityDataAsset* AbilityData)
{
	Abilities.Remove(AbilityData);
	RefreshList();
	SetVisibility(ESlateVisibility::Hidden);
}

void ULevelUp::SynchronizeProperties()
{
	Super::SynchronizeProperties();

}
