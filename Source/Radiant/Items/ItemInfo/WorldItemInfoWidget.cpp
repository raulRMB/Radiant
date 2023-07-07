// Copyright Radiant Studios


#include "WorldItemInfoWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWorldItemInfoWidget::SetText(FName Name)
{
	NameText->SetText(FText::FromName(Name));
}

void UWorldItemInfoWidget::SetBackgroundSize()
{
	if(UCanvasPanelSlot* BackgroundSlot = Cast<UCanvasPanelSlot>(BackgroundImage->Slot))
	{
		BackgroundSlot->SetSize(NameText->GetDesiredSize());
	}
}

