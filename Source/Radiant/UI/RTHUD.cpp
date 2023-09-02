// Copyright Radiant Studios


#include "UI/RTHUD.h"

#include "CaptureAreaBar.h"
#include "Minimap.h"
#include "RTInfoPanel.h"
#include "TeamInfoDisplay.h"
#include "Crafting/CraftingPanel.h"
#include "Data/ItemData.h"
#include "Player/RTPlayerState.h"
#include "Menu/Settings.h"
#include "Player/RTPlayerController.h"
#include "Enums/ItemSlotID.h"
#include "InGame/SlotManager/SlotManager.h"
#include "Util/Util.h"

void ARTHUD::BeginPlay()
{
	Super::BeginPlay();
	InfoPanel = CreateWidget<URTInfoPanel>(GetOwningPlayerController(), InfoPanelClass);
	InfoPanel->AddToViewport(1000);
	InfoPanel->Init();
	SettingsPanel = CreateWidget<USettings>(GetOwningPlayerController(), SettingsPanelClass);
	SettingsPanel->SetVisibility(ESlateVisibility::Hidden);
	SettingsPanel->AddToViewport();
	CaptureAreaBar = CreateWidget<UCaptureAreaBar>(GetOwningPlayerController(), CaptureAreaBarClass);
	CaptureAreaBar->AddToViewport();
	CaptureAreaBar->SetVisibility(ESlateVisibility::Hidden);
	// StoreUI = CreateWidget<UInGameStore>(GetWorld(), StoreUIClass);
	// StoreUI->AddToViewport();
	// StoreUI->SetVisibility(ESlateVisibility::Hidden);
	CraftingPanel = CreateWidget<UCraftingPanel>(GetOwningPlayerController(), CraftingPanelClass);
	CraftingPanel->AddToViewport();
	CraftingPanel->SetVisibility(ESlateVisibility::Hidden);
	Minimap = Cast<UMinimap>(CreateWidget<UMinimap>(GetWorld(), MinimapClass));
	Minimap->BindGridUpdate();
	Minimap->AddToViewport();
	Minimap->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if(ARTPlayerController* RTPC = Cast<ARTPlayerController>(GetOwningPlayerController()))
	{
		CraftingPanel->Init(RTPC->GetInventory());
	}
	
	SlotManager = NewObject<USlotManager>(this, USlotManager::StaticClass());
	SlotManager->InitSlots(InfoPanel, CraftingPanel, ItemSlotClass);

	TeamInfoDisplay = CreateWidget<UTeamInfoDisplay>(GetOwningPlayerController(), TeamInfoDisplayClass);
	TeamInfoDisplay->Init();
	TeamInfoDisplay->SetVisibility(ESlateVisibility::HitTestInvisible);
	TeamInfoDisplay->AddToViewport();
}

void ARTHUD::ShowEndScreen(bool won)
{
	InfoPanel->ShowEndScreen(won);
}

void ARTHUD::TogglePlayerStats()
{
	InfoPanel->TogglePlayerStats();
}

UMinimap* ARTHUD::GetMinimap()
{
	return Minimap;
}

ARTHUD::ARTHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARTHUD::ToggleSettings()
{
	SettingsPanel->RefreshKeybindList();
	bSettingsOpen = !bSettingsOpen;
	SettingsPanel->SetVisibility(bSettingsOpen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);

	Cast<ARTPlayerController>(GetOwningPlayerController())->SaveUserSettingsDelay();
}

void ARTHUD::HideLoadScreen()
{
	check(InfoPanel)
	InfoPanel->HideLoadScreen();
}

void ARTHUD::SetFPS(float FPS)
{
	InfoPanel->SetFPS(FPS);
}

void ARTHUD::SetMS(float MS)
{
	InfoPanel->SetMS(MS);
}

void ARTHUD::ToggleStore()
{
	bStoreOpen = !bStoreOpen;
	CraftingPanel->SetVisibility(bStoreOpen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void ARTHUD::BindUIItems()
{
	auto PS = GetOwningPlayerController()->GetPlayerState<ARTPlayerState>();
	// StoreUI->Init(PS);
	InfoPanel->Bind(PS);
}

void ARTHUD::Escape()
{
	if (bStoreOpen)
	{
		bStoreOpen = false;
		CraftingPanel->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		ToggleSettings();
	}
}

UAbilityDataAsset* ARTHUD::GetAbilityDataAsset(EItemSlotID Slot) const
{
	if (HotBarAbilities.Contains(Slot))
	{
		UAbilityDataAsset* AbilityDataAsset = ItemTable->FindRow<FItemData>(
			HotBarAbilities[Slot].ItemName, TEXT("HUD GetAbilityDataAsset"))->AbilityData;
		return AbilityDataAsset;
	}
	return nullptr;
}

void ARTHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Minimap->DrawDynamic();
}

void ARTHUD::InitCraftingPanel(UInventoryComponent* Inventory) const
{
	CraftingPanel->Init(Inventory);
}

FGameplayTag ARTHUD::GetAbilityTrigger(EItemSlotID Slot) const
{
	if (SlotManager)
	{
		return SlotManager->GetAbilityTrigger(Slot);
	}
	return FGameplayTag();
}

void ARTHUD::ReloadCraftingPanel(FName ItemName)
{
	CraftingPanel->LoadCraftingItem(ItemName);
}
