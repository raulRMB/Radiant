// Copyright Radiant Studios


#include "UI/ItemSlot.h"

#include "RTHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/AbilityDataAsset.h"
#include "Event/EventBroker.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
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
	bIsOnCooldown = false;
}

void UItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	UAbilityDragDropOperation* DragWidget = Cast<UAbilityDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UAbilityDragDropOperation::StaticClass()));
	SetVisibility(ESlateVisibility::HitTestInvisible);
	Icon->SetDesiredSizeOverride(FVector2D(80, 80));
	DragWidget->DefaultDragVisual = Icon;
	DragWidget->DefaultDragVisual->SetVisibility(ESlateVisibility::HitTestInvisible);
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

bool UItemSlot::SwapWith(UItemSlot* ItemSlot)
{
	if(ItemSlot)
	{
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
	}
	UEventBroker::Get(this)->DragStatusChanged.Broadcast(false);
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UItemSlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	SetVisibility(ESlateVisibility::Visible);
	if(bShouldDropItem)
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
	Icon->SetBrushFromTexture(Data.Icon);
	Icon->SetToolTipText(Data.Tooltip);
	CooldownMask->SetToolTipText(Data.Tooltip);
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