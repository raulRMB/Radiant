// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "HeroInfoBar.generated.h"


UCLASS()
class RADIANT_API UHeroInfoBar : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	uint32 Ticks = 0;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* DamageTakenBar;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ManaBar;
	UPROPERTY(meta = (BindWidget))
	class UImage* XPBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Level;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Username;
	
public:
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageIndicatorLerpRate = 1.6f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Interface;
	void SetHealthPercent(float Percent);
	void SetTicks(uint32 X) { this->Ticks = X; }
	void SetManaPercent(float Percent);
	void SetHealthColor(const FLinearColor& Color);
	void SetXPPercent(float Percent);
	void SetLevel(int32 Value);
	virtual void NativeConstruct() override;
	void InitXPMaterial();
	void SetOverheadText(const FString& String);
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
