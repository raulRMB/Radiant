// Copyright Radiant Studios


#include "UI/RTInfoPanel.h"
#include "Components/HorizontalBox.h"
#include "ItemSlot.h"
#include "Player/Avatar.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameState.h"
#include "Player/RTPlayerState.h"
#include "Util/AbilityDragDropOperation.h"
#include "Util/Util.h"


class ARTPlayerState;

void URTInfoPanel::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	UpdateProperties(DeltaTime);
}

void URTInfoPanel::UpdateProperties(float DeltaTime) const
{
	if (auto State = GetOwningPlayerState<ARTPlayerState>())
	{
		if (URTAvatarAttributeSet* Attributes = State->GetAttributeSetBase())
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
	auto Slots = HotbarHorizontalBox->GetAllChildren();
	for (int i = 0; i < Slots.Num(); i++)
	{
		UItemSlot* AbilityWidget = Cast<UItemSlot>(Slots[i]);
		if (AbilityWidget)
		{
			AbilityWidget->UpdateCooldown();
		}
	}
}

FText URTInfoPanel::FormatText(float CurrentHealth, float MaxHealth) const
{
	return FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth));
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
		PS->OnUpdateRadianite.AddUObject(this, &URTInfoPanel::UpdateRadianite);
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
