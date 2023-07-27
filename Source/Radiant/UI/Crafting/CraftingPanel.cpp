// Copyright Radiant Studios


#include "CraftingPanel.h"
#include "CraftingNode.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Data/CraftingNodeDataAsset.h"
#include "Data/ItemData.h"
#include "Engine/DataTable.h"
#include "Player/RTPlayerState.h"
#include "UI/ItemSlot.h"
#include "Util/AbilityDragDropOperation.h"
#include "Util/Util.h"

void UCraftingPanel::Init(UInventoryComponent* Inventory)
{
	InventoryComponent = Inventory;
	
	if(ItemTable)
	{
		for(TPair<FName, uint8*> It : ItemTable->GetRowMap())
		{
			if(FItemData* ItemData = reinterpret_cast<FItemData*>(It.Value))
			{
				if(ItemData->CraftingNodeData)
				{
					if(ItemData->CraftingNodeData->Materials.Num() <= 0)
					{
						continue;
					}
					UCraftingNode* CraftingNode = CreateWidget<UCraftingNode>(this, CraftingNodeClass);
					ItemData->CraftingNodeData->Name = It.Key;
					CraftingNode->Init(ItemData->CraftingNodeData->Icon, 1, InventoryComponent);
					CraftingNode->SetCraftingItemDataName(It.Key);
					NodeMap.Add(It.Key, CraftingNode);
					if(UCanvasPanelSlot* PanelSlot = RecipeList->AddChildToCanvas(CraftingNode))
					{
						PanelSlot->SetAnchors(FAnchors(.0f));
						PanelSlot->SetSize(FVector2D(40.f * RecipeList->GetCachedGeometry().Scale));
						PanelSlot->SetAlignment(FVector2D(0.f));
						FVector2D Position = FVector2D((RecipeList->GetChildrenCount() - 1) % 8, (RecipeList->GetChildrenCount() - 1) / 8);
						PanelSlot->SetPosition(Position * 45.f);
						PanelSlot->SetPosition(PanelSlot->GetPosition() + FVector2D(20.f));
					}
				}
			}
		}
	}
}

void UCraftingPanel::LoadCraftingItem(const FName ItemName)
{
	CurrentNodeCount = 0;
	RecipeTree->ClearChildren();
	AggregateList->ClearChildren();
	if(!ItemTable)
		return;
	
	if(FItemData* CraftingItemData = ItemTable->FindRow<FItemData>(ItemName, TEXT("Crafting Panel Load Crafting Item")))
	{
		UCraftingNodeDataAsset* CraftingNodeDataAsset = CraftingItemData->CraftingNodeData;
		if(!CraftingNodeDataAsset)
		{
			return;
		}

		for(int32 i = 0 ; i < CraftingNodeDataAsset->Aggregates.Num(); i++)
		{
			if(!CraftingNodeDataAsset->Aggregates[i])
			{
				continue;
			}
			if(UCraftingNode* CraftingNode = CreateWidget<UCraftingNode>(this, CraftingNodeClass))
			{
				CraftingNode->Init(CraftingNodeDataAsset->Aggregates[i]->Icon, 1, InventoryComponent);
				CraftingNode->SetCraftingItemDataName(CraftingNodeDataAsset->Aggregates[i]->Name);
				if(USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>())
				{
					SizeBox->AddChild(CraftingNode);
					SizeBox->SetWidthOverride(50.f);
					SizeBox->SetHeightOverride(50.f);
					AggregateList->AddChildToUniformGrid(SizeBox, i, 0);
				}
			}
		}
		
		FGraphNode* Root = new FGraphNode(nullptr, CraftingNodeDataAsset, 1, 0, 1, 0);
		int32 Depth = 0;
		AddGraphNode(CraftingNodeDataAsset, Root, Depth);
		
		if(Root->GetWidth() - MaximumNodeWidth > 0)
		{
			RootNodeWidthScale = static_cast<float>(Root->GetWidth()) / static_cast<float>(MaximumNodeWidth);
		}
		else
		{
			RootNodeWidthScale = 1.f;
		}

		TraverseInitPositions(static_cast<float>(Root->GetWidth()) * .5f, Root);
		TraverseCreateNode(nullptr, Root);
		
		delete Root;
	}
}

UCraftingNode* UCraftingPanel::CreateCraftingNode(UCraftingNode* InParentNode, FGraphNode* Node)
{
	if(UCraftingNode* CraftingNode = CreateWidget<UCraftingNode>(this, CraftingNodeClass))
	{
		CraftingNode->Init(Node->GetDataAsset()->Icon, Node->GetAmount(), InventoryComponent);
		CraftingNode->SetIsLeaf(Node->GetChildCount() < 1);
		CraftingNode->SetCraftingItemDataName(Node->GetDataAsset()->Name);
		CraftingNode->SetParentNode(InParentNode);
		if(UCanvasPanelSlot* PanelSlot = RecipeTree->AddChildToCanvas(CraftingNode))
		{
			float CanvasScale = RecipeTree->GetCachedGeometry().Scale / RootNodeWidthScale;
			PanelSlot->SetAnchors(FAnchors(RootTreeAnchor.X, RootTreeAnchor.Y, RootTreeAnchor.X, RootTreeAnchor.Y));
			PanelSlot->SetSize(FVector2D(CanvasScale * NodeScale));
			PanelSlot->SetAlignment(FVector2D(.5f));
			PanelSlot->SetPosition(Node->GetPosition() * FVector2D(NodeScale + NodeHorizontalSeparation, NodeScale + NodeVerticalSeparation) * CanvasScale);
		}
		return CraftingNode;
	}
	return nullptr;
}

void UCraftingPanel::AddGraphNode(UCraftingNodeDataAsset* CNDA, FGraphNode* Node, int32& Depth)
{
	Depth++;
	int Idx = 0;
	for(TPair<UCraftingNodeDataAsset*, uint8>& Child : CNDA->Materials)
	{
		FGraphNode* ChildNode = new FGraphNode(Node, Child.Key, Child.Value, Depth, 1, Idx);
		Node->AddChild(ChildNode);
		if(Node->GetChildCount() > 1)
		{
			Node->Grow();
		}
		AddGraphNode(Child.Key, ChildNode, Depth);
		Idx++;
	}
	Depth--;
}

void UCraftingPanel::TraverseInitPositions(float RootHalfWidth, FGraphNode* Node)
{
	Node->InitPosition(RootHalfWidth, CurrentNodeCount);
	for(auto Child : **Node)
	{
		TraverseInitPositions(RootHalfWidth, Child);
	}
	if(Node->GetChildCount() == 0)
		CurrentNodeCount++;
}

void UCraftingPanel::TraverseCreateNode(UCraftingNode* InParentCraftingNode, FGraphNode* Node)
{
	UCraftingNode* MyCraftingNode = CreateCraftingNode(InParentCraftingNode, Node);
	for(auto Child : **Node)
	{
		TraverseCreateNode(MyCraftingNode, Child);
	}
}

int32 UCraftingPanel::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if(RecipeTree)
	{
		for(UWidget* Child : RecipeTree->GetAllChildren())
		{
			if(UCraftingNode* CraftingNode = Cast<UCraftingNode>(Child))
			{
				FVector2D Start, End;
				// if(!CraftingNode->IsLeaf())
				// {
				// 	// Draw Left Line
				// 	Start = CraftingNode->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(0.f, .5f));
				// 	End = Start - FVector2D(25.f, 0.f);
				// 	UWidgetBlueprintLibrary::DrawLine(Context, Start, End,FLinearColor::White, true, 1.f);
				// }
				if(CraftingNode->GetParentNode())
				{
					// Draw Right Line
					float CanvasScale = 1.0 / RootNodeWidthScale;
					Start = CraftingNode->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(1.f, .5f));
					End = Start + FVector2D(NodeEdgeLineOffset * NodeHorizontalSeparation, 0.f) * CanvasScale;
					UWidgetBlueprintLibrary::DrawLine(Context, Start, End,NodeEdgeLineColor, true, NodeEdgeLineThickness);
					Start = End;
					End = CraftingNode->GetParentNode()->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(0.f, .5f));
					UWidgetBlueprintLibrary::DrawLine(Context, Start, End, NodeEdgeLineColor, true, NodeEdgeLineThickness);
				}
			}
		}	
	}
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);
}

void UCraftingPanel::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if(UAbilityDragDropOperation* DragOp = Cast<UAbilityDragDropOperation>(InOperation))
		DragOp->WidgetReference->SetShouldDropItem(true);
}

void UCraftingPanel::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	if(UAbilityDragDropOperation* DragOp = Cast<UAbilityDragDropOperation>(InOperation))
		DragOp->WidgetReference->SetShouldDropItem(false);
}