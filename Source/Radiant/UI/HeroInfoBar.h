// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeroInfoBar.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UHeroInfoBar : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ManaBar;
	UPROPERTY(meta = (BindWidget))
	class UImage* XPBar;
public:
	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Interface;
	void SetHealthPercent(float Percent);
	void SetManaPercent(float Percent);
	void SetHealthColor(const FLinearColor& Color);
	void SetXPPercent(float Percent);
	virtual void NativeConstruct() override;
};
