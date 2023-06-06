// Copyright Radiant Studios


#include "UI/RTHUD.h"

#include "RTInfoPanel.h"

void ARTHUD::BeginPlay()
{
	Super::BeginPlay();
	InfoPanel = CreateWidget<URTInfoPanel>(GetWorld(), InfoPanelClass);
	InfoPanel->AddToViewport();
	InfoPanel->Init();
}

void ARTHUD::ShowEndScreen(bool won)
{
	InfoPanel->ShowEndScreen(won);
}