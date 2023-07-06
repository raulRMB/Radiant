// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Player/RTPlayerState.h"
#include "RTInfoPanel.generated.h"

UCLASS()
class RADIANT_API URTInfoPanel : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* Abilities;

	UPROPERTY(meta=(BindWidget))
	class UImage* LoadingScreen;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RedTeam;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* BlueTeam;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HealthAmount;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Radianite;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* ManaBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ManaAmount;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Victory;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Defeat;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* FPSCounter;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MSCounter;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
public:
	void UpdateProperties(float DeltaTime);
	void UpdateCooldowns();
	FText FormatText(float CurrentHealth, float MaxHealth) const;

	UFUNCTION(BlueprintCallable)
	void Init();
	void UpdateRadianite(float X);
	void Bind(ARTPlayerState* PS);

	UFUNCTION()
	void OnHeroDeath(uint32 RedScore, uint32 BlueScore);
	
	void UpdateAbilities(TMap<EInventorySlot, UAbilityDataAsset*> Abilities);
	
	void ShowEndScreen(bool bWon);
	void HideLoadScreen();
	void SetFPS(float FPS);
	void SetMS(float MS);
};
