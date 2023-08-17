// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ItemData.h"
#include "CraftingPanel.generated.h"

UCLASS()
class RADIANT_API UCraftingPanel : public UUserWidget
{
	GENERATED_BODY()

	float PanelSlotSize = 60.f;
	int32 PanelSlotRowSize = 9;
	float PanelSlotPadding = 10.f;
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* RecipeTree;

	UPROPERTY(meta=(BindWidget))
	class UCheckBox* AbilityFilter;
	
	UPROPERTY(meta=(BindWidget))
	UCheckBox* GearFilter;

	UPROPERTY(meta=(BindWidget))
	UCheckBox* MaterialFilter;

	UPROPERTY(meta=(BindWidget))
	UCheckBox* WeaponFilter;

	UPROPERTY(meta=(BindWidget))
	UCheckBox* GeneralFilter;
	
	UPROPERTY(meta=(BindWidget))
	UCheckBox* SupportFilter;

	UPROPERTY(meta=(BindWidget))
	UCheckBox* DamageFilter;

	UPROPERTY(meta=(BindWidget))
	UCheckBox* TankFilter;
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* RecipeList;

	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* AggregateList;

	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* InventoryGrid;

	UPROPERTY(meta=(BindWidget))
	class UGearSlot* HelmetSlot;
	
	UPROPERTY(meta=(BindWidget))
	class UGearSlot* ChestSlot;
	
	UPROPERTY(meta=(BindWidget))
	class UGearSlot* LegsSlot;

	UPROPERTY(meta=(BindWidget))
	class UGearSlot* JewelrySlot;
	
	UPROPERTY(meta=(BindWidget))
	class UGearSlot* GlovesSlot;

	UPROPERTY(meta=(BindWidget))
	class UGearSlot* BootsSlot;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCraftingNode> CraftingNodeClass;

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTable;

	int32 CurrentNodeCount = 0;

	UPROPERTY(EditAnywhere)
	float NodeEdgeLineThickness = 1.f;
	UPROPERTY(EditAnywhere)
	float NodeScale = 100.f;
	UPROPERTY(EditAnywhere)
	float NodeHorizontalSeparation = 100.f;
	UPROPERTY(EditAnywhere)
	float NodeVerticalSeparation = 100.f;
	UPROPERTY(EditAnywhere)
	FLinearColor NodeEdgeLineColor = FLinearColor::White;
	UPROPERTY(EditAnywhere)
	float NodeEdgeLineOffset = .25f; // Percentage of the HorizontalSeparation
	UFUNCTION()
	void OnFilterChanged(bool bIsChecked);
	void NativeConstruct() override;
	bool ShouldIncludeClass(FItemData* ItemData);
	bool ShouldInclude(FItemData* ItemData);
	UPROPERTY(EditAnywhere)
	int32 MaximumNodeWidth = 5;
	float RootNodeWidthScale = 1.f;

	UPROPERTY(EditAnywhere)
	FVector2D RootTreeAnchor;

	UPROPERTY(VisibleAnywhere)
	TMap<FName, class UCraftingNode*> NodeMap;

	UPROPERTY(EditAnywhere)
	class UInventoryComponent* InventoryComponent;
public:
	UUniformGridPanel* GetInventoryGrid() const { return InventoryGrid; }
	UGearSlot* GetHelmetSlot() const { return HelmetSlot; }
	UGearSlot* GetChestSlot() const { return ChestSlot; }
	UGearSlot* GetLegsSlot() const { return LegsSlot; }
	UGearSlot* GetJewelrySlot() const { return JewelrySlot; }
	UGearSlot* GetGlovesSlot() const { return GlovesSlot; }
	UGearSlot* GetBootsSlot() const { return BootsSlot; }
	void Init(class UInventoryComponent* Inventory);
	void LoadCraftingItem(const FName ItemName);
	UCraftingNode* CreateCraftingNode(class UCraftingNode* InParentCraftingNode, class FGraphNode* Node);
	void AddGraphNode(class UCraftingNodeDataAsset* CNDA, class FGraphNode* Node, int32& Depth);
	void TraverseInitPositions(float RootHalfWidth, FGraphNode* Node);
	void TraverseCreateNode(class UCraftingNode* InParentCraftingNode, class FGraphNode* Node);
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                       UDragDropOperation* InOperation) override;
};
