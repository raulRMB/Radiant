// Copyright Radiant Studios


#include "UI/HeroInfoBar.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
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

int32 UHeroInfoBar::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	auto Box = HealthBar->GetCachedGeometry().GetRenderBoundingRect();
	auto Size = HealthBar->GetCachedGeometry().GetLocalSize();
	if(Ticks > 0)
	{
		auto TickSize = Size.X / Ticks;
		for(uint32 i = 1; i < Ticks; i++)
		{
			const float Thickness = i % 10 == 0 ? 1.5f : 0.7f;
			double Y =  i % 10 == 0 ? Size.Y / 1.5 : Size.Y / 2.5f;
			const float X = i * TickSize;
			UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(X, 1.f), FVector2D(X, Y), FLinearColor::Black, true, Thickness);
		}
	}
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);
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
