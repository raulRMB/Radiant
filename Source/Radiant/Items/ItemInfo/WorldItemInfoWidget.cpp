// Copyright Radiant Studios


#include "WorldItemInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWorldItemInfoWidget::SetText(FString Name)
{
	NameText->SetText(FText::FromString(Name));
	BackgroundImage->SetDesiredSizeOverride(NameText->GetDesiredSize());
}
