// Copyright Radiant Studios


#include "UI/RTHUD.h"

#include "CaptureAreaBar.h"
#include "RTInfoPanel.h"
#include "InGame/InGameStore.h"
#include "Menu/LevelUp.h"
#include "Menu/Settings.h"
#include "Player/Avatar.h"
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
	SettingsPanel->AddToViewport();
	StoreUI = CreateWidget<UInGameStore>(GetWorld(), StoreUIClass);
	StoreUI->AddToViewport();
	StoreUI->SetVisibility(ESlateVisibility::Hidden);
	
}

void ARTHUD::ShowEndScreen(bool won)
{
	InfoPanel->ShowEndScreen(won);
}

void ARTHUD::UpdateAbilities(TArray<UAbilityDataAsset*> Abilities)
{
	InfoPanel->UpdateAbilities(Abilities);
}

void ARTHUD::ToggleSettings()
{
	SettingsPanel->RefreshKeybindList();
	bSettingsOpen = !bSettingsOpen;
	SettingsPanel->SetVisibility(bSettingsOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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
			LevelUp->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ARTHUD::ToggleStore()
{
	bStoreOpen = !bStoreOpen;
	StoreUI->SetVisibility(bStoreOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void ARTHUD::BindUIItems()
{
	StoreUI->Init(Cast<AAvatar>(GetOwningPawn()));
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
