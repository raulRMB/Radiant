// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameStore.generated.h"


UCLASS()
class RADIANT_API UInGameStore : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UGridPanel* StoreGrid;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RadianiteText;

	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* InventoryGrid;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UStoreItem> StoreItemClass;
	
	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTable;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* BuyAmountText;

	UPROPERTY(meta=(BindWidget))
	class UButton* BuyIncrement;

	UPROPERTY(meta=(BindWidget))
	class UButton* BuyIncrementBig;

	UPROPERTY(meta=(BindWidget))
	class UButton* BuyDecrement;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* BuyDecrementBig;

	UPROPERTY(meta=(BindWidget))
	class UCraftingPanel* CraftingPanel;
	
	UFUNCTION()
	void OnBuyIncrement();
	UFUNCTION()
	void OnBuyIncrementBig();
	UFUNCTION()
	void OnBuyDecrement();
	UFUNCTION()
	void OnBuyDecrementBig();
	virtual void NativeConstruct() override;

	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION()
	void UpdateRadianite(float RadianiteAmount);
public:	
	void Init(class ARTPlayerState* PS);
	int32 BuyAmount = 1;
	UUniformGridPanel* GetInventoryGrid() const;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UCraftingPanel* GetCraftingPanel() const;
};
