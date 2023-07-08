// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilityWidget.h"
#include "GameFramework/HUD.h"
#include "Util/Enums/InventorySlot.h"
#include "RTHUD.generated.h"

DECLARE_DELEGATE_OneParam(FGiveAbilityFromButtonSignature, class UAbilityDataAsset*)

USTRUCT()
struct FItemSlotInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FName ItemName;
	UPROPERTY()
	uint32 ItemAmount;

	FItemSlotInfo(const FName& Name = NAME_None, uint32 Amount = 0)
	{
		ItemName = Name;
		ItemAmount = Amount;
	}
};

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

	UPROPERTY()
	TMap<EInventorySlot, FItemSlotInfo> HotBarAbilities;

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTable;	
public:
	TObjectPtr<class UCaptureAreaBar> CaptureAreaBar;

	FGiveAbilityFromButtonSignature GiveAbilityFromButton;
public:
	ARTHUD();
	
	void UpdateAbilities(const TMap<EInventorySlot, FItemSlotInfo>& Abilities);
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

	UAbilityDataAsset* GetAbilityDataAsset(EInventorySlot Slot) const;

	virtual void Tick(float DeltaSeconds) override;

	struct FGameplayTag GetAbilityTrigger(EInventorySlot Slot) const;
	void SwapHotbarSlot(EInventorySlot One, EInventorySlot Two);

private:
	UFUNCTION()
	void OnItemChanged(const FName& Name, uint32 Amount);
};

