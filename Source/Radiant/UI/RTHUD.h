// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Util/Enums/HotbarSlot.h"
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

	UPROPERTY(EditAnywhere, Category=UI, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UUserWidget> MinimapClass;

	UPROPERTY()
	URTInfoPanel* InfoPanel;
	UPROPERTY()
	class USettings* SettingsPanel;
	UPROPERTY()
	UUserWidget* LevelUpPanel;

	UPROPERTY()
	class UMinimap* Minimap;

	UPROPERTY(EditAnywhere, Category=UI, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UUserWidget> StoreUIClass;
	UPROPERTY()
	class UInGameStore* StoreUI;
	uint8 bStoreOpen : 1;
	uint8 bStoreInitialized : 1;

	uint8 bSettingsOpen : 1;
public:
	TObjectPtr<class UCaptureAreaBar> CaptureAreaBar;

	FGiveAbilityFromButtonSignature GiveAbilityFromButton;
public:
	ARTHUD();
	
	void UpdateAbilities(TMap<EHotBarSlot, UAbilityDataAsset*> Abilities);
	void ShowEndScreen(bool won);
	void ToggleSettings();
	void HideLoadScreen();
	void SetFPS(float FPS);
	void SetMS(float MS);
	void ShowLevelUpScreen();
	void ToggleStore();
	UFUNCTION()
	void BindUIItems();
	void Escape();

	virtual void Tick(float DeltaSeconds) override;
};
