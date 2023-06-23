// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTHUD.generated.h"

DECLARE_DELEGATE_OneParam(FGiveAbilityFromButtonSignature, class UAbilityDataAsset*)

/**
 * 
 */
UCLASS()
class RADIANT_API ARTHUD : public AHUD
{
	GENERATED_BODY()
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class URTInfoPanel> InfoPanelClass;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> SettingsPanelClass;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> LevelUpClass;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> CaptureAreaBarClass;

	UPROPERTY()
	URTInfoPanel* InfoPanel;
	UPROPERTY()
	UUserWidget* SettingsPanel;
	UPROPERTY()
	UUserWidget* LevelUpPanel;

	UPROPERTY(EditAnywhere, Category=UI, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UUserWidget> StoreUIClass;
	UPROPERTY()
	class UInGameStore* StoreUI;
	uint8 bStoreOpen : 1;	
public:
	TObjectPtr<class UCaptureAreaBar> CaptureAreaBar;

	FGiveAbilityFromButtonSignature GiveAbilityFromButton;
public:
	void UpdateAbilities(TArray<class UAbilityDataAsset*> Abilities);
	void ShowEndScreen(bool won);
	UFUNCTION(BlueprintCallable, Exec)
	void ToggleSettings(bool on);
	void HideLoadScreen();
	void SetFPS(float FPS);
	void SetMS(float MS);
	void ShowLevelUpScreen();
	void ToggleStore();
};
