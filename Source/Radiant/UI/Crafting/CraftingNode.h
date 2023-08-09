﻿#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CraftingNodeDataAsset.h"
#include "Event/EventBroker.h"
#include "CraftingNode.generated.h"

class FGraphNode
{
	FGraphNode* Parent;
	class UCraftingNodeDataAsset* DataAsset;
	TArray<FGraphNode*> Children;		
	int32 Amount;
	FVector2D Position;
	int32 Width;
	int32 Depth;
	int32 Index;
public:
	FGraphNode(FGraphNode* InParent = nullptr, UCraftingNodeDataAsset* InDataAsset = nullptr, const int32 InAmount = 0, const int32 InDepth = 0, const int32 InWidth = 0, const int32 InIndex = 0)
		:
			Parent(InParent),
			DataAsset(InDataAsset),
			Amount(InAmount),
			Position({static_cast<float>(InDepth), 0.}),
			Width(InWidth),
			Depth(InDepth),
			Index(InIndex)
		{}
	
	~FGraphNode();
	void Grow();
	void AddChild(FGraphNode* Child) { Children.Add(Child); }
	FGraphNode* GetParent() const { return Parent; }
	int32 GetWidth() const { return Width; }
	int32 GetIndex() const { return Index; }
	int32 GetChildCount() const { return Children.Num(); }
	int32 GetDepth() const { return Depth; }
	int32 GetAmount() const { return Amount; }
	TArray<FGraphNode*>& operator*() { return Children; }
	UCraftingNodeDataAsset* GetDataAsset() const { return DataAsset; }
	void InitPosition(float RootHalfWidth, int32 Count);
	FVector2D GetPosition() const { return Position; }
	void SetPosition(FVector2D InPosition) { Position = InPosition; }
};

UCLASS()
class UCraftingNode : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AmountTextBlock;

	UPROPERTY(meta=(BindWidget))
	class UDynamicButton* Button;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UItemTooltip> ItemTooltipClass;

	UPROPERTY()
	FName CraftingItemDataName;

	uint8 bIsLeaf : 1;

	UPROPERTY()
	class UCraftingNode* ParentNode;

	UPROPERTY()
	TMap<FName, uint32> IngredientList;

	UPROPERTY()
	class UInventoryComponent* InventoryComponent;

	UFUNCTION()
	void OnItemChanged(const FName& Name, uint32 Amount);

public:	
	void Init(class UTexture2D* Texture, uint8 Amount, class UInventoryComponent* NewInventoryComponent);

	void InitIngredientList(class UCraftingNodeDataAsset* DataAsset);
	
	void SetCraftingItemDataName(const FName InCraftingItemDataName);
	void SetInventoryComponent(class UInventoryComponent* InInventoryComponent) { InventoryComponent = InInventoryComponent; }
	
	bool IsLeaf() const { return bIsLeaf; }
	void SetIsLeaf(bool IsLeaf) { bIsLeaf = IsLeaf; }
	
	UCraftingNode* GetParentNode() const { return ParentNode; }
	void SetParentNode(UCraftingNode* InParentNode) { ParentNode = InParentNode; }

	bool CheckHasMaterials();
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnMouseLeftClicked();
	
	UFUNCTION()
	void OnMouseRightClicked();
	
	void InitTooltip();
};
