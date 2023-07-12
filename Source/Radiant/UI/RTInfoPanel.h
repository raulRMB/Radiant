// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Player/RTPlayerState.h"
#include "Components/HorizontalBox.h"
#include "RTInfoPanel.generated.h"

UCLASS()
class RADIANT_API URTInfoPanel : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* HotbarHorizontalBox;

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
	void UpdateProperties(float DeltaTime) const;
	void UpdateCooldowns() const;
	FText FormatText(float CurrentHealth, float MaxHealth) const;

	UHorizontalBox* GetHotbarHorizontalBox();

	UFUNCTION(BlueprintCallable)
	void Init();
	void UpdateRadianite(float X) const;
	void Bind(ARTPlayerState* PS);

	UFUNCTION()
	void OnHeroDeath(uint32 RedScore, uint32 BlueScore) const;

	void ShowEndScreen(bool bWon) const;
	void HideLoadScreen() const;
	void SetFPS(float FPS) const;
	void SetMS(float MS) const;

private:
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                               UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
