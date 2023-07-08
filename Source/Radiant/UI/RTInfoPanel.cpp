// Copyright Radiant Studios


#include "UI/RTInfoPanel.h"

#include "AbilityWidget.h"
#include "RTHUD.h"
#include "Player/Avatar.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/AbilityDataAsset.h"
#include "Data/ItemData.h"
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

void URTInfoPanel::UpdateCooldowns() const
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

void URTInfoPanel::UpdateAbilities(TMap<EInventorySlot, FItemSlotInfo> AbilityData) const
{
	TArray<UWidget*> Slots = Abilities->GetAllChildren();
	for(int i = 0; i <= static_cast<uint32>(EInventorySlot::Six); i++)
	{
		if(AbilityData.Num() <= i)
		{
			break;
		}
		FName ItemName = AbilityData[static_cast<EInventorySlot>(i)].ItemName;
		
		if(FItemData* DataAsset = UUtil::GetItemDataFromName(ItemName))
		{
			if(i >= Slots.Num())
			{
				break;
			}
			UAbilityWidget* slot = Cast<UAbilityWidget>(Slots[i]);
			if(DataAsset)
			{
				if(DataAsset->AbilityData)
				{
					slot->SetData(DataAsset->AbilityData, AbilityData[static_cast<EInventorySlot>(i)].ItemAmount);
				}
			}
			else
			{
				slot->Reset();
			}
		}
	}
}

void URTInfoPanel::Init()
{
	Cast<ARTGameState>(UGameplayStatics::GetGameState(this))->OnHeroDeathDelegate.BindUObject(this, &URTInfoPanel::OnHeroDeath);
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
	if(bWon)
	{
		Victory->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		Defeat->SetVisibility(ESlateVisibility::Visible);
	}
}

void URTInfoPanel::HideLoadScreen() const
{
	if(LoadingScreen)
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
	if(UAbilityDragDropOperation* DragOp = Cast<UAbilityDragDropOperation>(InOperation))
		DragOp->WidgetReference->SetShouldDropItem(true);
}

void URTInfoPanel::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	if(UAbilityDragDropOperation* DragOp = Cast<UAbilityDragDropOperation>(InOperation))
		DragOp->WidgetReference->SetShouldDropItem(false);
}