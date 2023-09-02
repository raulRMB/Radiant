// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "ItemSlot.h"
#include "GameFramework/HUD.h"
#include "Enums/ItemSlotID.h"
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
	TSubclassOf<class UUserWidget> CaptureAreaBarClass;

	UPROPERTY(EditAnywhere, Category=UI, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UUserWidget> MinimapClass;

	UPROPERTY(EditAnywhere, Category=UI, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UUserWidget> CraftingPanelClass;

	UPROPERTY()
	URTInfoPanel* InfoPanel;
	UPROPERTY()
	class USettings* SettingsPanel;

	UPROPERTY()
	class USlotManager* SlotManager;

	UPROPERTY()
	class UMinimap* Minimap;

	UPROPERTY(EditAnywhere, Category=UI, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UUserWidget> StoreUIClass;
	
	uint8 bStoreOpen : 1;
	uint8 bStoreInitialized : 1;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	class UCraftingPanel* CraftingPanel;

	UPROPERTY(meta=(BindWidget))
	class UGearSlot* WeaponSlotWidget;
	
	uint8 bSettingsOpen : 1;

	UPROPERTY()
	TMap<EItemSlotID, FItemSlotInfo> HotBarAbilities;

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTable;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UItemSlot> ItemSlotClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> TeamInfoDisplayClass;
	UPROPERTY()
	class UTeamInfoDisplay* TeamInfoDisplay;
	
public:
	TObjectPtr<class UCaptureAreaBar> CaptureAreaBar;

	FGiveAbilityFromButtonSignature GiveAbilityFromButton;

	UGearSlot* GetWeaponSlot() const { return WeaponSlotWidget; }
	void TogglePlayerStats();
	UMinimap* GetMinimap();

public:
	ARTHUD();

	void ShowEndScreen(bool won);
	void ToggleSettings();
	void HideLoadScreen();
	void SetFPS(float FPS);
	void SetMS(float MS);
	void ToggleStore();
	UFUNCTION()
	void BindUIItems();
	void Escape();

	UAbilityDataAsset* GetAbilityDataAsset(EItemSlotID Slot) const;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void InitCraftingPanel(class UInventoryComponent* Inventory) const;

	struct FGameplayTag GetAbilityTrigger(EItemSlotID Slot) const;
	void ReloadCraftingPanel(FName ItemName);
};
