// // Copyright Radiant Studios
//
//
// #include "UI/InGame/StoreItem.h"
//
// #include "AbilitySystemComponent.h"
// #include "InGameStore.h"
// #include "Components/Button.h"
// #include "Components/Image.h"
// #include "Components/TextBlock.h"
// #include "Data/AbilityDataAsset.h"
// #include "Data/ItemData.h"
// #include "Kismet/GameplayStatics.h"
// #include "Player/Avatar.h"
// #include "Player/RTPlayerController.h"
// #include "Player/RTPlayerState.h"
// #include "Util/Util.h"
//
// void UStoreItem::OnClicked()
// {
// 	if(ARTPlayerController* PC = Cast<ARTPlayerController>(GetWorld()->GetFirstPlayerController()))
// 	{
// 		PC->GetPlayerState<ARTPlayerState>()->S_BuyAbility(FName(ItemName), Store->BuyAmount);
// 	}
// }
//
// void UStoreItem::SetStore(UInGameStore* InStore)
// {
// 	Store = InStore;
// }
//
// void UStoreItem::Init(const FName& Name)
// {
// 	ItemName = Name.ToString();
// 	if (ItemTable)
// 	{
// 		static const FString ContextString(TEXT("Store Item Button Native Construct")); 
// 		if (FItemData* ItemData = ItemTable->FindRow<FItemData>(Name, ContextString, true))
// 		{
// 			Price->SetText(RTPRINTF("%.0f", ItemData->AbilityData->Price));
// 			Icon->SetBrushFromTexture(ItemData->AbilityData->Icon);
// 			if(ensure(Button))
// 			{
// 				Button->SetToolTipText(ItemData->AbilityData->Tooltip);
// 				Button->OnClicked.AddUniqueDynamic(this, &UStoreItem::OnClicked);
// 			}
// 		}
// 	}
// }
