// Copyright Radiant Studios


#include "UI/RTHUD.h"

#include "CaptureAreaBar.h"
#include "Minimap.h"
#include "RTInfoPanel.h"
#include "Data/ItemData.h"
#include "InGame/InGameStore.h"
#include "Menu/Settings.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerController.h"
#include "Enums/ItemSlotID.h"
#include "InGame/SlotManager/SlotManager.h"
#include "Util/Util.h"

void ARTHUD::BeginPlay()
{
	Super::BeginPlay();
	InfoPanel = CreateWidget<URTInfoPanel>(GetWorld(), InfoPanelClass);
	InfoPanel->AddToViewport(1000);
	InfoPanel->Init();
	SettingsPanel = CreateWidget<USettings>(GetWorld(), SettingsPanelClass);
	SettingsPanel->SetVisibility(ESlateVisibility::Hidden);
	CaptureAreaBar = CreateWidget<UCaptureAreaBar>(GetWorld(), CaptureAreaBarClass);
	check(CaptureAreaBar)
	CaptureAreaBar->AddToViewport();
	CaptureAreaBar->SetVisibility(ESlateVisibility::Hidden);
	SettingsPanel->AddToViewport();
	StoreUI = CreateWidget<UInGameStore>(GetWorld(), StoreUIClass);
	StoreUI->AddToViewport();
	StoreUI->SetVisibility(ESlateVisibility::Hidden);
	Minimap = Cast<UMinimap>(CreateWidget<UMinimap>(GetWorld(), MinimapClass));
	Minimap->AddToViewport();
	Minimap->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SlotManager = NewObject<USlotManager>(this, USlotManager::StaticClass());
	SlotManager->InitSlots(InfoPanel->GetHotbarHorizontalBox(), StoreUI->GetInventoryGrid(), ItemSlotClass);
}

void ARTHUD::ShowEndScreen(bool won)
{
	InfoPanel->ShowEndScreen(won);
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
	StoreUI->SetVisibility(bStoreOpen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void ARTHUD::BindUIItems()
{
	auto PS = GetOwningPlayerController()->GetPlayerState<ARTPlayerState>();
	StoreUI->Init(PS);
	InfoPanel->Bind(PS);
}

void ARTHUD::Escape()
{
	if (bStoreOpen)
	{
		bStoreOpen = false;
		StoreUI->SetVisibility(ESlateVisibility::Hidden);
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

	Minimap->DrawDynamic();
}

FGameplayTag ARTHUD::GetAbilityTrigger(EItemSlotID Slot) const
{
	if (SlotManager)
	{
		return SlotManager->GetAbilityTrigger(Slot);
	}
	return FGameplayTag();
}
