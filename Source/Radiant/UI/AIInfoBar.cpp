// Copyright Radiant Studios


#include "UI/AIInfoBar.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UAIInfoBar::ShowLevel(bool Value)
{
	Level->SetVisibility(Value ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	LevelBg->SetVisibility(Value ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UAIInfoBar::SetHealthPercent(float Percent)
{
	if(HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UAIInfoBar::SetLevel(int32 Value)
{
	if(Level)
	{
		Level->SetText(FText::FromString(FString::FromInt(Value)));
	}
}

void UAIInfoBar::SetColor(FColor Color)
{
	if(this)
		if(HealthBar)
		{
			HealthBar->SetFillColorAndOpacity(FLinearColor(Color));
		}
}
