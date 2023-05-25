// Copyright Radiant Studios


#include "UI/HeroInfoBar.h"

#include "Components/ProgressBar.h"

void UHeroInfoBar::SetHealthPercent(float Percent)
{
	HealthBar->SetPercent(Percent);
}

void UHeroInfoBar::SetManaPercent(float Percent)
{
	ManaBar->SetPercent(Percent);
}

void UHeroInfoBar::SetHealthColor(const FLinearColor& Color)
{
	HealthBar->SetFillColorAndOpacity(Color);
}
