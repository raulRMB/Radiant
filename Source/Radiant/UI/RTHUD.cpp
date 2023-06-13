// Copyright Radiant Studios


#include "UI/RTHUD.h"

#include "CaptureAreaBar.h"
#include "RTInfoPanel.h"
#include "Util/Util.h"

void ARTHUD::BeginPlay()
{
	Super::BeginPlay();
	InfoPanel = CreateWidget<URTInfoPanel>(GetWorld(), InfoPanelClass);
	InfoPanel->AddToViewport();
	InfoPanel->Init();
	SettingsPanel = CreateWidget<UUserWidget>(GetWorld(), SettingsPanelClass);
	SettingsPanel->SetVisibility(ESlateVisibility::Hidden);
	SettingsPanel->AddToViewport();
	CaptureAreaBar = CreateWidget<UCaptureAreaBar>(GetWorld(), CaptureAreaBarClass);
	CaptureAreaBar->AddToViewport();
	RTLOG("HUD BEGIN PLAY")
	RTPRINT("HUD BEGIN PLAY")
}

void ARTHUD::ShowEndScreen(bool won)
{
	InfoPanel->ShowEndScreen(won);
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
	InfoPanel->HideLoadScreen();
}

void ARTHUD::SetFPS(float FPS)
{
	InfoPanel->SetFPS(FPS);
}
