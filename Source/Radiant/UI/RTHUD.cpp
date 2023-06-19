// Copyright Radiant Studios


#include "UI/RTHUD.h"

#include "CaptureAreaBar.h"
#include "RTInfoPanel.h"
#include "Util/Util.h"

void ARTHUD::BeginPlay()
{
	Super::BeginPlay();
	InfoPanel = CreateWidget<URTInfoPanel>(GetWorld(), InfoPanelClass);
	InfoPanel->AddToViewport(1000);
	InfoPanel->Init();
	SettingsPanel = CreateWidget<UUserWidget>(GetWorld(), SettingsPanelClass);
	SettingsPanel->SetVisibility(ESlateVisibility::Hidden);
	SettingsPanel->AddToViewport();
	CaptureAreaBar = CreateWidget<UCaptureAreaBar>(GetWorld(), CaptureAreaBarClass);
	check(CaptureAreaBar)
	CaptureAreaBar->AddToViewport();
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
