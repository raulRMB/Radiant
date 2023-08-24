// Copyright Radiant Studios


#include "UI/ItemSlot.h"

#include "GearSlot.h"
#include "RTHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/AbilityDataAsset.h"
#include "Event/EventBroker.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "InGame/ItemTooltip.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerController.h"
#include "Player/RTPlayerState.h"
#include "Util/AbilityDragDropOperation.h"
#include "Util/Util.h"

void UItemSlot::NativeConstruct()
{
	Super::NativeConstruct();
	MaterialInstance = UMaterialInstanceDynamic::Create(Mat, this);
	CooldownMask->SetBrushResourceObject(MaterialInstance);
	CooldownMask->SetVisibility(ESlateVisibility::Hidden);
	AmountText->SetVisibility(ESlateVisibility::HitTestInvisible);
	bIsOnCooldown = false;
}

void UItemSlot::ListenForKeybindChanges()
{
	UEventBroker::Get(this)->KeybindChanged.AddUObject(this, &UItemSlot::SetKeybindText);
}

bool UItemSlot::IsHotBarSlot()
{
	return SlotID <= EItemSlotID::HotBarLast && SlotID >= EItemSlotID::HotBarFirst;
}

void UItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	SetVisibility(ESlateVisibility::HitTestInvisible);
	if(IsWeaponSlotWithCooldowns())
	{
		SetVisibility(ESlateVisibility::Visible);
		return;
	}
	UAbilityDragDropOperation* DragWidget = Cast<UAbilityDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UAbilityDragDropOperation::StaticClass()));
	Icon->SetDesiredSizeOverride(FVector2D(80, 80));
	DragWidget->DefaultDragVisual = Icon;
	DragWidget->WidgetReference = this;
	DragWidget->Pivot = EDragPivot::CenterCenter;
	OutOperation = DragWidget;
	UEventBroker::Get(this)->DragStatusChanged.Broadcast(true);
}

FReply UItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if(Icon->Brush.GetResourceObject() == nullptr)
		return Reply;
	FEventReply ReplyResult = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return ReplyResult.NativeReply;
}

EItemType UItemSlot::GetItemType()
{
	return ItemSlotData.ItemType;
}

EClassType UItemSlot::GetItemClass()
{
	return ItemSlotData.ClassType;
}

bool UItemSlot::IsWeaponSlotWithCooldowns()
{
	if(SlotID == EItemSlotID::WeaponSlot && ItemSlotData.ItemType == EItemType::Weapon && ItemSlotData.ClassType != EClassType::General)
	{
		if(GetWorld()->GetFirstPlayerController()->GetPlayerState<ARTPlayerState>()->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Cooldown")))
		{
			return true;
		}
	}
	return false;
}

bool UItemSlot::HoldsValidClassForWeapon()
{
	auto Class = GetWorld()->GetFirstPlayerController()->GetPlayerState<ARTPlayerState>()->CurrentClass;
	return bIsEmpty || GetItemClass() == Class || GetItemClass() == EClassType::General;
}

bool UItemSlot::IsHotbarSlotOnCooldown()
{
	return IsHotBarSlot() && bIsOnCooldown;
}

bool UItemSlot::HoldsAbility()
{
	return bIsEmpty || ItemSlotData.ItemType == EItemType::Ability;
}

bool UItemSlot::SwapWith(UItemSlot* ItemSlot)
{
	if(ItemSlot)
	{
		if(IsWeaponSlotWithCooldowns() || ItemSlot->IsWeaponSlotWithCooldowns())
		{
			return false;
		}
		if((IsHotBarSlot() && !ItemSlot->HoldsValidClassForWeapon()) || (ItemSlot->IsHotBarSlot() && !HoldsValidClassForWeapon()))
		{
			return false;
		}
		if((IsHotBarSlot() && !ItemSlot->HoldsAbility()) || (ItemSlot->IsHotBarSlot() && !HoldsAbility()))
		{
			return false;
		}
		if((!ItemSlot->IsHotBarSlot() && IsHotbarSlotOnCooldown()) || (!IsHotBarSlot() && ItemSlot->IsHotbarSlotOnCooldown()))
		{
			return false;
		}
		if(UGearSlot* GearSlot = Cast<UGearSlot>(ItemSlot))
		{
			if(!GearSlot->CheckCanSwapWith(this))
			{
				return false;
			}
		}
		if(UGearSlot* GearSlot = Cast<UGearSlot>(this))
		{
			if(!GearSlot->CheckCanSwapWith(ItemSlot))
			{
				return false;
			}
		}
		FItemSlotData TempItemSlotData = ItemSlot->ItemSlotData;
		ItemSlot->SetData(ItemSlotData);
		SetData(TempItemSlotData);
		bool bTempIsEmpty = ItemSlot->bIsEmpty;
		ItemSlot->SetEmpty(bIsEmpty);
		SetEmpty(bTempIsEmpty);
		return true;
	}
	return false;
}

void UItemSlot::Reset()
{
	bIsEmpty = true;
	AmountText->SetText(FText::FromString(""));
	AmountText->SetVisibility(ESlateVisibility::Hidden);
	Icon->SetBrushFromTexture(nullptr);
	Icon->SetToolTipText(FText::FromString(""));
	CooldownMask->SetToolTipText(FText::FromString(""));
	ItemSlotData = FItemSlotData();
	CooldownTag = FGameplayTag();
	Trigger = FGameplayTag();
}

FGameplayTag UItemSlot::GetAbilityTrigger()
{
	return Trigger;
}

bool UItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                             UDragDropOperation* InOperation)
{
	UAbilityDragDropOperation* DragDropOperation = Cast<UAbilityDragDropOperation>(InOperation);
	if(DragDropOperation && DragDropOperation->WidgetReference != nullptr)
	{
		SwapWith(DragDropOperation->WidgetReference);
		DragDropOperation->WidgetReference->SetVisibility(ESlateVisibility::Visible);
	}
	UEventBroker::Get(this)->DragStatusChanged.Broadcast(false);
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UItemSlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	SetVisibility(ESlateVisibility::Visible);
	if(bShouldDropItem && !IsWeaponSlotWithCooldowns())
	{
		if(AAvatar* Avatar = GetOwningPlayerPawn<AAvatar>())
		{
			Avatar->DropItem(ItemSlotData.ItemName);
		}
	}
	UEventBroker::Get(this)->DragStatusChanged.Broadcast(false);
}

void UItemSlot::SetOn(bool On)
{
	bIsOnCooldown = On;
	if(bIsOnCooldown)
		Icon->SetColorAndOpacity(FLinearColor::Gray);
	else
		Icon->SetColorAndOpacity(FLinearColor::White);
	CooldownMask->SetVisibility(static_cast<ESlateVisibility>(!bIsOnCooldown));
}

void UItemSlot::SetAbilityCoolDown(const float Percent)
{
	MaterialInstance->SetScalarParameterValue("Percent", Percent);
}

void UItemSlot::SetKeybindText()
{
	KeybindBackground->SetVisibility(ESlateVisibility::HitTestInvisible);
	Keybind->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer()))
	{
		FKey Mapping = Subsystem->GetPlayerMappedKey(UUtil::GetMappingNameFromSlot(SlotID));
		if(Mapping.GetFName() == NAME_None)
		{
			Keybind->SetText(FText::FromName(UUtil::GetDefaultKeybindFromSlot(SlotID)));
		} else
		{
			Keybind->SetText(FText::FromName(Mapping.GetFName()));
		}
	}
}

float UItemSlot::GetCooldownPercent(const float TimeRemaining, const float CooldownDuration)
{
	return (CooldownDuration - TimeRemaining) / CooldownDuration;
}

void UItemSlot::SetData(const FItemSlotData& Data)
{
	ItemSlotData = Data;
	if(Data.ItemAmount > 0)
	{
		AmountText->SetVisibility(ESlateVisibility::Visible);
		AmountText->SetText(FText::FromString(FString::FromInt(Data.ItemAmount)));
	}
	else
	{
		AmountText->SetVisibility(ESlateVisibility::Hidden);
	}
	
	//AmountText->SetToolTipText(Data.Tooltip);
	Icon->SetBrushFromTexture(Data.Icon);
	//Icon->SetToolTipText(Data.Tooltip);
	//CooldownMask->SetToolTipText(Data.Tooltip);
	SetToolTip(UUtil::InitTooltip(this, Tooltip, Data.ItemName));
	CooldownTag = Data.CooldownTag;
	Trigger = Data.AbilityTrigger;
}

void UItemSlot::UpdateCooldown()
{
	float TimeRemaining = 0.f;
	float CooldownDuration = 0.f;
	SetOn(false);
	if(GetCooldownRemaining(TimeRemaining, CooldownDuration))
	{
		float Percent = GetCooldownPercent(TimeRemaining, CooldownDuration);
		SetAbilityCoolDown(Percent);
		SetOn(true);
	}
}

bool UItemSlot::GetCooldownRemaining(float& TimeRemaining,
	float& CooldownDuration)
{
	if(!GetOwningPlayerState())
	{
		return false;
	}
	URTAbilitySystemComponent* AbilitySystemComponent = Cast<URTAbilitySystemComponent>(GetOwningPlayerState<ARTPlayerState>()->GetAbilitySystemComponent());
	if (AbilitySystemComponent && CooldownTag.IsValid())
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(CooldownTag);
		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}
			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;
			return true;
		}
	}
	return false;
}