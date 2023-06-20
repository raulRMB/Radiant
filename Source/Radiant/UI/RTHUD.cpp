// Copyright Radiant Studios


#include "UI/RTHUD.h"

#include "CaptureAreaBar.h"
#include "RTInfoPanel.h"
#include "Menu/LevelUp.h"
#include "Menu/Settings.h"
#include "Util/Util.h"

void ARTHUD::BeginPlay()
{
	Super::BeginPlay();
	InfoPanel = CreateWidget<URTInfoPanel>(GetWorld(), InfoPanelClass);
	InfoPanel->AddToViewport(1000);
	InfoPanel->Init();
	SettingsPanel = CreateWidget<UUserWidget>(GetWorld(), SettingsPanelClass);
	SettingsPanel->SetVisibility(ESlateVisibility::Hidden);
	LevelUpPanel = CreateWidget<UUserWidget>(GetWorld(), LevelUpClass);
	LevelUpPanel->SetVisibility(ESlateVisibility::Hidden);
	LevelUpPanel->AddToViewport();
	CaptureAreaBar = CreateWidget<UCaptureAreaBar>(GetWorld(), CaptureAreaBarClass);
	check(CaptureAreaBar)
	CaptureAreaBar->AddToViewport();
	SettingsPanel->AddToViewport();
}

void ARTHUD::ShowEndScreen(bool won)
{
	InfoPanel->ShowEndScreen(won);
}

void ARTHUD::UpdateAbilities(TArray<UAbilityDataAsset*> Abilities)
{
	InfoPanel->UpdateAbilities(Abilities);
}

void ARTHUD::ToggleSettings(bool on)
{
	Cast<USettings>(SettingsPanel)->RefreshKeybindList();
	if(on)
	{
		SettingsPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SettingsPanel->SetVisibility(ESlateVisibility::Hidden);
	}
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
