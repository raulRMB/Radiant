// Copyright Radiant Studios


#include "UI/RTInfoPanel.h"

#include "GearSlot.h"
#include "Components/HorizontalBox.h"
#include "ItemSlot.h"
#include "PlayerStatsPanel.h"
#include "Player/Avatar.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameState.h"
#include "Player/RTPlayerState.h"
#include "Util/AbilityDragDropOperation.h"
#include "Util/Util.h"

void URTInfoPanel::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	UpdateProperties(DeltaTime);

	if(WeaponSlot)
	{
		WeaponSlot->UpdateCooldown();
	}
}

void URTInfoPanel::UpdateProperties(float DeltaTime) const
{
	if (auto State = GetOwningPlayerState<ARTPlayerState>())
	{
		if (URTAvatarAttributeSet* Attributes = State->GetAttributeSet())
		{
			UpdateCooldowns();
			HealthBar->SetPercent(Attributes->GetHealth() / Attributes->GetMaxHealth());
			ManaBar->SetPercent(Attributes->GetMana() / Attributes->GetMaxMana());
			HealthAmount->SetText(FormatText(Attributes->GetHealth(), Attributes->GetMaxHealth()));
			ManaAmount->SetText(FormatText(Attributes->GetMana(), Attributes->GetMaxMana()));
		}
	}
}

void URTInfoPanel::UpdateCooldowns() const
{
	for(UWidget* Widget : HotbarHorizontalBox->GetAllChildren())
	{
		if (UItemSlot* ItemSlot = Cast<UItemSlot>(Widget))
		{
			ItemSlot->UpdateCooldown();
		}
	}
}

FText URTInfoPanel::FormatText(float CurrentHealth, float MaxHealth) const
{
	return FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth));
}

void URTInfoPanel::TogglePlayerStats() const
{
	PlayerStats->SetVisibility(PlayerStats->GetVisibility() == ESlateVisibility::Visible
		? ESlateVisibility::Hidden
		: ESlateVisibility::Visible);
}

UHorizontalBox* URTInfoPanel::GetHotbarHorizontalBox()
{
	return HotbarHorizontalBox;
}

void URTInfoPanel::Init()
{
	Cast<ARTGameState>(UGameplayStatics::GetGameState(this))->OnHeroDeathDelegate.BindUObject(
		this, &URTInfoPanel::OnHeroDeath);
}

void URTInfoPanel::UpdateRadianite(float Amount) const
{
	Radianite->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Amount)));
}

void URTInfoPanel::Bind(ARTPlayerState* PS)
{
	if (PS)
	{
		UpdateRadianite(PS->GetAttributeSet()->GetRadianite());
		PS->OnUpdateRadianite.AddUObject(this, &URTInfoPanel::UpdateRadianite);
		PlayerStats->Bind(PS);
	}
}

void URTInfoPanel::OnHeroDeath(uint32 RedScore, uint32 BlueScore) const
{
	RedTeam->SetText(FText::FromString(FString::FromInt(RedScore)));
	BlueTeam->SetText(FText::FromString(FString::FromInt(BlueScore)));
}

void URTInfoPanel::ShowEndScreen(bool bWon) const
{
	if (bWon)
	{
		Victory->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Defeat->SetVisibility(ESlateVisibility::Visible);
	}
}

void URTInfoPanel::HideLoadScreen() const
{
	if (LoadingScreen)
	{
		LoadingScreen->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URTInfoPanel::SetFPS(float FPS) const
{
	FPSCounter->SetText(FText::FromString(FString::Printf(TEXT("FPS: %.0f"), FPS)));
}

void URTInfoPanel::SetMS(float MS) const
{
	MSCounter->SetText(RTPRINTF("MS: %.0f", MS));
}

UGearSlot* URTInfoPanel::GetWeaponSlot() const
{
	return WeaponSlot;
}

void URTInfoPanel::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if (UAbilityDragDropOperation* DragOp = Cast<UAbilityDragDropOperation>(InOperation))
	{
		DragOp->WidgetReference->SetShouldDropItem(true);
	}
}

void URTInfoPanel::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                     UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	if (UAbilityDragDropOperation* DragOp = Cast<UAbilityDragDropOperation>(InOperation))
	{
		DragOp->WidgetReference->SetShouldDropItem(false);
	}
}
