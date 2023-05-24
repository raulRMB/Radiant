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
public:
	void SetHealthPercent(float Percent);
	void SetManaPercent(float Percent);
};
