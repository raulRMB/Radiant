// Copyright Radiant Studios


#include "InGameStore.h"

#include "StoreItem.h"
#include "Components/GridPanel.h"
#include "Components/TextBlock.h"
#include "Data/ItemData.h"
#include "Player/Avatar.h"
#include "Util/Util.h"

void UInGameStore::UpdateRadianite(float RadianiteAmount)
{
	RadianiteText->SetText(RTPRINTF("Radianite: %.0f", RadianiteAmount));
}

void UInGameStore::Init(AAvatar* Avatar)
{
	if (Avatar)
	{
		Avatar->OnUpdateRadianite.AddUObject(this, &UInGameStore::UpdateRadianite);
	}
}

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
