// Copyright Radiant Studios


#include "InGameStore.h"

#include "StoreItem.h"
#include "Components/GridPanel.h"
#include "Components/TextBlock.h"
#include "Data/ItemData.h"
#include "Player/Avatar.h"
#include "Util/AbilityDragDropOperation.h"
#include "Util/Util.h"

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
}

UUniformGridPanel* UInGameStore::GetInventoryGrid() const
{
	return InventoryGrid;
}

void UInGameStore::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ItemTable && StoreGrid)
	{
		static const FString ContextString(TEXT("Store Item Button Native Construct"));
		for (FName RowName : ItemTable->GetRowNames())
		{
			UStoreItem* StoreItem = CreateWidget<UStoreItem>(this, StoreItemClass);
			FItemData* ItemData = ItemTable->FindRow<FItemData>(RowName, ContextString);
			if(ItemData->AbilityData)
			{
				StoreItem->Init(RowName);
			}
			StoreGrid->AddChildToGrid(StoreItem, StoreGrid->GetChildrenCount() / 5, StoreGrid->GetChildrenCount() % 5);
		}
	}
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
