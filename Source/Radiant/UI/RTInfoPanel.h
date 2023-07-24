// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RTInfoPanel.generated.h"

UCLASS()
class RADIANT_API URTInfoPanel : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HotbarHorizontalBox;

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

	UPROPERTY(meta=(BindWidget))
	class UWeaponSlot* WeaponSlot;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
	void UpdateProperties(float DeltaTime) const;
	void UpdateCooldowns() const;
	FText FormatText(float CurrentHealth, float MaxHealth) const;

	class UHorizontalBox* GetHotbarHorizontalBox();

	UFUNCTION(BlueprintCallable)
	void Init();
	void UpdateRadianite(float X) const;
	void Bind(class ARTPlayerState* PS);

	UFUNCTION()
	void OnHeroDeath(uint32 RedScore, uint32 BlueScore) const;

	void ShowEndScreen(bool bWon) const;
	void HideLoadScreen() const;
	void SetFPS(float FPS) const;
	void SetMS(float MS) const;
	class UWeaponSlot* GetWeaponSlot() const;
private:
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                               UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
