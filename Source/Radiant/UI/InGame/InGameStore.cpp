// Copyright Radiant Studios


#include "InGameStore.h"

#include "StoreItem.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Data/ItemData.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerState.h"
#include "Util/AbilityDragDropOperation.h"
#include "Util/Util.h"
#include "Player/RTPlayerState.h"
#include "UI/Crafting/CraftingPanel.h"

void UInGameStore::UpdateRadianite(float RadianiteAmount)
{
	RadianiteText->SetText(RTPRINTF("Radianite: %.0f", RadianiteAmount));
}

void UInGameStore::Init(ARTPlayerState* PS)
{
	if (PS)
	{
		PS->OnUpdateRadianite.AddUObject(this, &UInGameStore::UpdateRadianite);
	}

	CraftingPanel->Init();
}

UUniformGridPanel* UInGameStore::GetInventoryGrid() const
{
	return InventoryGrid;
}

void UInGameStore::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	for(UWidget* Widget : InventoryGrid->GetAllChildren())
	{
		if (UItemSlot* ItemSlot = Cast<UItemSlot>(Widget))
		{
			ItemSlot->UpdateCooldown();
		}
	}
}

UCraftingPanel* UInGameStore::GetCraftingPanel() const
{
	return CraftingPanel;
}

void UInGameStore::OnBuyIncrement()
{
	BuyAmount++;
	BuyAmountText->SetText(FText::FromString(FString::FromInt(BuyAmount)));
}

void UInGameStore::OnBuyIncrementBig()
{
	BuyAmount += 5;
	BuyAmountText->SetText(FText::FromString(FString::FromInt(BuyAmount)));
}

void UInGameStore::OnBuyDecrement()
{
	BuyAmount = FMath::Max(1, BuyAmount - 1);
	BuyAmountText->SetText(FText::FromString(FString::FromInt(BuyAmount)));
}

void UInGameStore::OnBuyDecrementBig()
{
	BuyAmount = FMath::Max(1, BuyAmount - 5);
	BuyAmountText->SetText(FText::FromString(FString::FromInt(BuyAmount)));
}

void UInGameStore::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ItemTable && StoreGrid)
	{
		static const FString ContextString(TEXT("Store Item Button Native Construct"));
		TArray<FName> Rows = ItemTable->GetRowNames();
		Rows.Sort([this](const FName& One, const FName& Two) {
				FItemData* OneData = ItemTable->FindRow<FItemData>(One, ContextString);
				FItemData* TwoData = ItemTable->FindRow<FItemData>(Two, ContextString);
				return OneData->AbilityData->Price < TwoData->AbilityData->Price;
			});
		for (FName RowName : Rows)
		{
			UStoreItem* StoreItem = CreateWidget<UStoreItem>(this, StoreItemClass);
			FItemData* ItemData = ItemTable->FindRow<FItemData>(RowName, ContextString);
			if(ItemData->AbilityData)
			{
				StoreItem->Init(RowName);
				StoreItem->SetStore(this);
			}
			StoreGrid->AddChildToGrid(StoreItem, StoreGrid->GetChildrenCount() / 5, StoreGrid->GetChildrenCount() % 5);
		}
	}
	BuyIncrement->OnClicked.AddDynamic(this, &UInGameStore::OnBuyIncrement);
	BuyIncrementBig->OnClicked.AddDynamic(this, &UInGameStore::OnBuyIncrementBig);
	BuyDecrement->OnClicked.AddDynamic(this, &UInGameStore::OnBuyDecrement);
	BuyDecrementBig->OnClicked.AddDynamic(this, &UInGameStore::OnBuyDecrementBig);
}

void UInGameStore::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if(UAbilityDragDropOperation* DragOp = Cast<UAbilityDragDropOperation>(InOperation))
		DragOp->WidgetReference->SetShouldDropItem(true);
}

void UInGameStore::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	if(UAbilityDragDropOperation* DragOp = Cast<UAbilityDragDropOperation>(InOperation))
		DragOp->WidgetReference->SetShouldDropItem(false);
}
