// Copyright Radiant Studios


#include "UI/HeroInfoBar.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHeroInfoBar::NativeConstruct()
{
	Super::NativeConstruct();
	InitXPMaterial();
}

void UHeroInfoBar::InitXPMaterial()
{
	if (XPBar && Interface)
	{
		MaterialInstance = UMaterialInstanceDynamic::Create(Interface, this);
		XPBar->SetBrushResourceObject(MaterialInstance);
		MaterialInstance->SetScalarParameterValue("Percent", 0);
	}
}

void UHeroInfoBar::SetOverheadText(const FString& String)
{
	if (Username)
	{
		Username->SetText(FText::FromString(String));
	}
}

void UHeroInfoBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(DamageTakenBar->GetPercent() >= HealthBar->GetPercent())
	{
		DamageTakenBar->SetPercent(DamageTakenBar->GetPercent() - (DamageIndicatorLerpRate * InDeltaTime));
	}
}

void UHeroInfoBar::SetHealthPercent(float Percent)
{
	if(Percent < HealthBar->GetPercent())
	{
		DamageTakenBar->SetPercent(HealthBar->GetPercent());
	}
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
	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue("Percent", Percent);
	}
}

void UHeroInfoBar::SetLevel(int32 Value)
{
	Level->SetText(FText::FromString(FString::FromInt(Value)));
}
