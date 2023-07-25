// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingPanel.generated.h"

UCLASS()
class RADIANT_API UCraftingPanel : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* RecipeTree;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* RecipeList;

	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* AggregateList;

	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* InventoryGrid;
	
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
