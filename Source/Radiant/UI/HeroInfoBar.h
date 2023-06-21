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
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Level;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Username;
public:
	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Interface;
	void SetHealthPercent(float Percent);
	void SetManaPercent(float Percent);
	void SetHealthColor(const FLinearColor& Color);
	void SetXPPercent(float Percent);
	void SetLevel(int32 Value);
	virtual void NativeConstruct() override;
	void SetOverheadText(const FString& String);
};
