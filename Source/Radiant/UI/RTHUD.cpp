// Copyright Radiant Studios


#include "UI/RTHUD.h"

#include "CaptureAreaBar.h"
#include "Minimap.h"
#include "RTInfoPanel.h"
#include "Event/EventBroker.h"
#include "InGame/InGameStore.h"
#include "Menu/LevelUp.h"
#include "Menu/Settings.h"
#include "Player/Avatar.h"
#include "Player/InventoryComponent.h"
#include "Player/RTPlayerController.h"
#include "Util/Util.h"

void ARTHUD::BeginPlay()
{
	Super::BeginPlay();
	InfoPanel = CreateWidget<URTInfoPanel>(GetWorld(), InfoPanelClass);
	InfoPanel->AddToViewport(1000);
	InfoPanel->Init();
	SettingsPanel = CreateWidget<USettings>(GetWorld(), SettingsPanelClass);
	SettingsPanel->SetVisibility(ESlateVisibility::Hidden);
	LevelUpPanel = CreateWidget<UUserWidget>(GetWorld(), LevelUpClass);
	LevelUpPanel->SetVisibility(ESlateVisibility::Hidden);
	LevelUpPanel->AddToViewport();
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

	UEventBroker::Get(this)->ItemChanged.AddUObject(this, &ARTHUD::OnItemAdded);
	
}

void ARTHUD::ShowEndScreen(bool won)
{
	InfoPanel->ShowEndScreen(won);
}

ARTHUD::ARTHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARTHUD::UpdateAbilities(const TMap<EInventorySlot, UAbilityDataAsset*>& Abilities)
{	
	InfoPanel->UpdateAbilities(Abilities);
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

void ARTHUD::ShowLevelUpScreen()
{
	if(ULevelUp* LevelUp = Cast<ULevelUp>(LevelUpPanel))
	{
		if(LevelUp->ShouldShow())
		{
			LevelUp->RefreshList();
			LevelUp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
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
	if(bStoreOpen)
	{
		bStoreOpen = false;
		StoreUI->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		ToggleSettings();
	}
}

void ARTHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	Minimap->DrawDynamic();
}

FGameplayTag ARTHUD::GetAbilityTrigger(EInventorySlot Slot) const
{
	if(HotBarAbilities.Contains(Slot))
	{
		return HotBarAbilities[Slot]->Ability.GetDefaultObject()->GetTriggerTag();
	}
	return FGameplayTag();
}

void ARTHUD::SwapHotbarSlot(EInventorySlot One, EInventorySlot Two)
{
	if(HotBarAbilities.Contains(One) && HotBarAbilities.Contains(Two))
	{
		UAbilityDataAsset* Temp = HotBarAbilities[One];
		HotBarAbilities[One] = HotBarAbilities[Two];
		HotBarAbilities[Two] = Temp;
	} else if(HotBarAbilities.Contains(One))
	{
		HotBarAbilities.Add(Two, HotBarAbilities[One]);
		HotBarAbilities.Remove(One);
	} else if(HotBarAbilities.Contains(Two))
	{
		HotBarAbilities.Add(One, HotBarAbilities[Two]);
		HotBarAbilities.Remove(Two);
	}
}

void ARTHUD::OnItemAdded(const FInventoryItem& InventoryItem)
{
	for (int i = 0; i <= static_cast<uint32>(EInventorySlot::Six); i++)
	{
		EInventorySlot Slot = static_cast<EInventorySlot>(i);
		if(!HotBarAbilities.Contains(Slot))
		{
			HotBarAbilities.Add(Slot, InventoryItem.AbilityData);
			break;
		}
	}
	UpdateAbilities(HotBarAbilities);
}
