// Copyright Radiant Studios


#include "UI/RTInfoPanel.h"

#include "AbilityWidget.h"
#include "Player/Avatar.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/AbilityDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameState.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"


class ARTPlayerState;

void URTInfoPanel::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	UpdateProperties(DeltaTime);
}

void URTInfoPanel::UpdateProperties(float DeltaTime)
{
	if(auto State = GetOwningPlayerState<ARTPlayerState>())
	{
		if(URTAvatarAttributeSet* Attributes = State->GetAttributeSetBase())
		{
			UpdateCooldowns();
			HealthBar->SetPercent(Attributes->GetHealth() / Attributes->GetMaxHealth());
			ManaBar->SetPercent(Attributes->GetMana() / Attributes->GetMaxMana());
			HealthAmount->SetText(FormatText(Attributes->GetHealth(), Attributes->GetMaxHealth()));
			ManaAmount->SetText(FormatText(Attributes->GetMana(), Attributes->GetMaxMana()));
		}
	}
}

void URTInfoPanel::UpdateCooldowns()
{
	auto Slots = Abilities->GetAllChildren();
	for(int i = 0; i < Slots.Num(); i++)
	{
		UAbilityWidget* AbilityWidget = Cast<UAbilityWidget>(Slots[i]);
		if(AbilityWidget)
		{
			AbilityWidget->UpdateCooldown();
		}
	}
}

FText URTInfoPanel::FormatText(float CurrentHealth, float MaxHealth) const
{
	return FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth));
}

void URTInfoPanel::UpdateAbilities(TArray<UAbilityDataAsset*> AbilityData)
{
	int32 iconCounter = 0;
	auto Slots = Abilities->GetAllChildren();
	for(int i = 0; i < AbilityData.Num(); i++)
	{
		if(AbilityData[i]->IgnoreHud)
		{
			continue;
		}
		if(iconCounter < Slots.Num())
		{
			Cast<UAbilityWidget>(Slots[iconCounter])->SetData(AbilityData[i]);
			iconCounter++;
		}
	}
}

void URTInfoPanel::Init()
{
	Cast<ARTGameState>(UGameplayStatics::GetGameState(this))->OnHeroDeathDelegate.BindUObject(this, &URTInfoPanel::OnHeroDeath);
}

void URTInfoPanel::UpdateRadianite(float Amount)
{
	Radianite->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Amount)));
}

void URTInfoPanel::Bind(ARTPlayerState* PS)
{
	if (PS)
	{
		PS->OnUpdateRadianite.AddUObject(this, &URTInfoPanel::UpdateRadianite);
	}
}

void URTInfoPanel::OnHeroDeath(uint32 RedScore, uint32 BlueScore)
{
	RedTeam->SetText(FText::FromString(FString::FromInt(RedScore)));
	BlueTeam->SetText(FText::FromString(FString::FromInt(BlueScore)));
}

void URTInfoPanel::ShowEndScreen(bool bWon)
{
	if(bWon)
	{
		Victory->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		Defeat->SetVisibility(ESlateVisibility::Visible);
	}
}

void URTInfoPanel::HideLoadScreen()
{
	if(LoadingScreen)
	{
		LoadingScreen->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URTInfoPanel::SetFPS(float FPS)
{
	FPSCounter->SetText(FText::FromString(FString::Printf(TEXT("FPS: %.0f"), FPS)));
}

void URTInfoPanel::SetMS(float MS)
{
	MSCounter->SetText(RTPRINTF("MS: %.0f", MS));
}
