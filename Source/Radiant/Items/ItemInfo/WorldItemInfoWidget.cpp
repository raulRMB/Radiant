// Copyright Radiant Studios


#include "WorldItemInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWorldItemInfoWidget::SetText(FName Name)
{
	NameText->SetText(FText::FromName(Name));
	BackgroundImage->SetDesiredSizeOverride(NameText->GetDesiredSize());
}
