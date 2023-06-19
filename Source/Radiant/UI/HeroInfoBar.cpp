// Copyright Radiant Studios


#include "UI/HeroInfoBar.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UHeroInfoBar::NativeConstruct()
{
	Super::NativeConstruct();
	if(Interface && XPBar)
	{
		MaterialInstance = UMaterialInstanceDynamic::Create(Interface, this);
		XPBar->SetBrushResourceObject(MaterialInstance);
		MaterialInstance->SetScalarParameterValue("Percent", 0);
	}
}

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

void UHeroInfoBar::SetXPPercent(const float Percent)
{
	MaterialInstance->SetScalarParameterValue("Percent", Percent);
}

