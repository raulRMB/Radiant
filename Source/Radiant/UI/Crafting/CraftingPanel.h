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
	class UCanvasPanel* CraftingCanvasPanel;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* ButtonCanvasPanel;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCraftingNode> CraftingNodeClass;

	UPROPERTY(EditAnywhere)
	class UDataTable* CraftingItemDataTable;
	
	FVector2d PositionOffset;

	int32 Count = 0;
public:
	void Init();
	void LoadCraftingItem(const FName ItemName);
	void CreateCraftingNode(class FGraphNode* Node);
	void AddGraphNode(class UCraftingNodeDataAsset* CNDA, class FGraphNode* Node, int32& Depth);
	void TraverseInitPositions(float RootHalfWidth, FGraphNode* Node);
	void TraverseCreateNode(class FGraphNode* Node);
};
