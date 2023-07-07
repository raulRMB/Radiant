// Copyright Radiant Studios


#include "UI/AbilityWidget.h"

#include "RTHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Data/AbilityDataAsset.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Player/InventoryComponent.h"
#include "Player/RTPlayerController.h"
#include "Player/RTPlayerState.h"
#include "Util/AbilityDragDropOperation.h"

void UAbilityWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MaterialInstance = UMaterialInstanceDynamic::Create(Mat, this);
	AbilityCDMask->SetBrushResourceObject(MaterialInstance);
	bOn = false;
}

void UAbilityWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	UAbilityDragDropOperation* DragWidget = Cast<UAbilityDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UAbilityDragDropOperation::StaticClass()));
	SetVisibility(ESlateVisibility::HitTestInvisible);
	Ability->SetDesiredSizeOverride(FVector2D(80, 80));
	DragWidget->DefaultDragVisual = Ability;
	DragWidget->WidgetReference = this;
	DragWidget->Pivot = EDragPivot::CenterCenter;
	OutOperation = DragWidget;
}

FReply UAbilityWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if(Ability->Brush.GetResourceObject() == nullptr)
		return Reply;
	FEventReply ReplyResult = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return ReplyResult.NativeReply;
}

void UAbilityWidget::Reset()
{
	AbilityData = nullptr;
	Ability->SetBrushFromTexture(nullptr);
	Ability->SetToolTipText(FText::FromString(""));
	AbilityCDMask->SetToolTipText(FText::FromString(""));
	CooldownTag = FGameplayTag();
}

bool UAbilityWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                  UDragDropOperation* InOperation)
{
	UAbilityDragDropOperation* DragDropOperation = Cast<UAbilityDragDropOperation>(InOperation);
	if(DragDropOperation && DragDropOperation->WidgetReference != nullptr && DragDropOperation->WidgetReference->AbilityData != nullptr)
	{

		ARTPlayerState* PS = Cast<ARTPlayerState>(GetOwningPlayerState());
		if(PS)
		{
			PS->GetRTController()->GetHUD<ARTHUD>()->SwapHotbarSlot(DragDropOperation->WidgetReference->HotbarSlot, HotbarSlot);
		}
		UAbilityDataAsset* Temp = AbilityData;
		SetData(DragDropOperation->WidgetReference->AbilityData);
		if(Temp)
		{
			DragDropOperation->WidgetReference->SetData(Temp);
			DragDropOperation->WidgetReference->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			DragDropOperation->WidgetReference->Reset();	
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UAbilityWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	SetVisibility(ESlateVisibility::Visible);
}

void UAbilityWidget::SetOn(bool On)
{
	bOn = On;
	if(bOn)
		Ability->SetColorAndOpacity(FLinearColor::Gray);
	else
		Ability->SetColorAndOpacity(FLinearColor::White);
	AbilityCDMask->SetVisibility(static_cast<ESlateVisibility>(!bOn));
}

void UAbilityWidget::SetAbilityCoolDown(const float Percent)
{
	MaterialInstance->SetScalarParameterValue("Percent", Percent);
}

float UAbilityWidget::GetCooldownPercent(const float TimeRemaining, const float CooldownDuration)
{
	return (CooldownDuration - TimeRemaining) / CooldownDuration;
}

void UAbilityWidget::SetData(UAbilityDataAsset* Data)
{
	AbilityData = Data;
    Ability->SetBrushFromTexture(Data->Icon);
    Ability->SetToolTipText(Data->Tooltip);
    AbilityCDMask->SetToolTipText(Data->Tooltip);
	CooldownTag = Data->Ability.GetDefaultObject()->GetCooldownTag();
}

void UAbilityWidget::UpdateCooldown()
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

bool UAbilityWidget::GetCooldownRemaining(float& TimeRemaining,
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