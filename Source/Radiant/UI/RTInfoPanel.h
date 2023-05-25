// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "RTInfoPanel.generated.h"

UENUM(BlueprintType)
enum class EAbilityID : uint8
{
	Ability1		UMETA(DisplayName="Ability1"),
	Ability2		UMETA(DisplayName="Ability2"),
	Ability3		UMETA(DisplayName="Ability3"),
	Ability4		UMETA(DisplayName="Ability4"),
	Ability5		UMETA(DisplayName="Ability5"),
	Ability6		UMETA(DisplayName="Ability6")
};

USTRUCT()
struct FUIAbility
{
	GENERATED_BODY()
	uint8 bOn : 1;
	UMaterialInstanceDynamic* MaterialInstance{};
	class UImage* AbilityImage{};
	class UImage* MaskImage{};

	FUIAbility(UMaterialInterface* Interface = nullptr, UObject* Context = nullptr, UImage* Ability = nullptr, UImage* Mask = nullptr)
		: AbilityImage(Ability), MaskImage(Mask)
	{
		MaterialInstance = UMaterialInstanceDynamic::Create(Interface, Context);
		bOn = false;
	}

	void SetOn(bool On)
	{
		bOn = On;
		if(bOn)
			AbilityImage->SetColorAndOpacity(FLinearColor::Gray);
		else
			AbilityImage->SetColorAndOpacity(FLinearColor::White);
		MaskImage->SetVisibility(static_cast<ESlateVisibility>(!bOn));
	}
};

/**
 * 
 */
UCLASS()
class RADIANT_API URTInfoPanel : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UImage* Ability1;
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability1CDMask;

	UPROPERTY(meta=(BindWidget))
	class UImage* Ability2;
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability2CDMask;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability3;
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability3CDMask;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability4;
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability4CDMask;

	UPROPERTY(meta=(BindWidget))
	class UImage* Ability5;
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability5CDMask;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability6;
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability6CDMask;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* Mat;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* ManaBar;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
	TArray<FUIAbility> Abilities;
	UPROPERTY(EditAnywhere, Category="Cooldowns")
	TArray<FGameplayTagContainer> Tags;
public:
	void SetAbilityCoolDown(EAbilityID Abilty, const float Percent);
	void AbilityCooldowns();
	void UpdateProperties(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void Init();

	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

	float GetCooldownPercent(const float TimeRemaining, const float CooldownDuration);

};
