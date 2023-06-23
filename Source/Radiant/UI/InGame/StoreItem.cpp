// Copyright Radiant Studios


#include "UI/InGame/StoreItem.h"

#include "AbilitySystemComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/AbilityDataAsset.h"
#include "Data/ItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerController.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void UStoreItem::OnClicked()
{
	if(ensureMsgf(AbilityData, TEXT("No ability data found in store item button")))
	{
		if(ensureMsgf(GetWorld(), TEXT("No world found in store item button")))
		{
			if(ARTPlayerController* PC = Cast<ARTPlayerController>(GetWorld()->GetFirstPlayerController()))
			{
				PC->GetPlayerState<ARTPlayerState>()->S_GiveAbility(AbilityData);
			}
		}
	}
}

void UStoreItem::Init(const FName& Name)
{
	if (ItemTable)
	{
		static const FString ContextString(TEXT("Store Item Button Native Construct")); 
		if (FItemData* ItemData = ItemTable->FindRow<FItemData>(Name, ContextString, true))
		{
			Price->SetText(RTPRINTF("%.0f", ItemData->AbilityData->Price));
			Icon->SetBrushFromTexture(ItemData->AbilityData->Icon);
			AbilityData = ItemData->AbilityData;
		}
	}
	if(ensure(Button))
	{
		Button->OnClicked.AddUniqueDynamic(this, &UStoreItem::OnClicked);
	}
}
