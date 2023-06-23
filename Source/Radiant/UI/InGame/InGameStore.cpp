// Copyright Radiant Studios


#include "InGameStore.h"

#include "StoreItem.h"
#include "Components/GridPanel.h"
#include "Data/ItemData.h"

void UInGameStore::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemTable && GridPanel)
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
			GridPanel->AddChildToGrid(StoreItem, GridPanel->GetChildrenCount() / 5, GridPanel->GetChildrenCount() % 5);
		}
	}
}
