// Copyright Radiant Studios


#include "CraftingPanel.h"
#include "CraftingNode.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Data/CraftingItemData.h"
#include "Data/CraftingNodeDataAsset.h"
#include "Engine/DataTable.h"
#include "Util/Util.h"

void UCraftingPanel::Init()
{
	if(CraftingItemDataTable)
	{
		for(TPair<FName, uint8*> It : CraftingItemDataTable->GetRowMap())
		{
			if(FCraftingItemData* CraftingItemData = reinterpret_cast<FCraftingItemData*>(It.Value))
			{
				if(CraftingItemData->CraftingNodeDataAsset)
				{
					UCraftingNode* CraftingNode = CreateWidget<UCraftingNode>(this, CraftingNodeClass);
					CraftingNode->SetCraftingItemDataName(It.Key);
					CraftingNode->Init(CraftingItemData->CraftingNodeDataAsset->Icon, 1);
					if(UCanvasPanelSlot* PanelSlot = ButtonCanvasPanel->AddChildToCanvas(CraftingNode))
					{
						PanelSlot->SetAnchors(FAnchors(.2f, .0f, .2f, .0f));
						PanelSlot->SetSize(FVector2D(100.f));
						PanelSlot->SetAlignment(FVector2D(.5f));
						PanelSlot->SetPosition(FVector2D(0.f, ButtonCanvasPanel->GetChildrenCount() * 120.f));
					}
				}
			}
		}
	}
}

void UCraftingPanel::LoadCraftingItem(const FName ItemName)
{
	CurrentNodeCount = 0;
	CraftingCanvasPanel->ClearChildren();	
	if(!CraftingItemDataTable)
		return;
	
	if(FCraftingItemData* CraftingItemData = CraftingItemDataTable->FindRow<FCraftingItemData>(ItemName, TEXT("Crafting Panel Load Crafting Item")))
	{
		UCraftingNodeDataAsset* CraftingNodeDataAsset = CraftingItemData->CraftingNodeDataAsset;
		if(!CraftingNodeDataAsset)
		{
			return;
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
		CraftingNode->Init(Node->GetDataAsset()->Icon, Node->GetAmount());
		CraftingNode->SetIsLeaf(Node->GetChildCount() < 1);
		CraftingNode->SetParentNode(InParentNode);
		if(UCanvasPanelSlot* PanelSlot = CraftingCanvasPanel->AddChildToCanvas(CraftingNode))
		{
			float CanvasScale = CraftingCanvasPanel->GetCachedGeometry().Scale / RootNodeWidthScale;
			PanelSlot->SetAnchors(FAnchors(.8f, .5f, .8f, .5f));
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
	for(auto Child : CNDA->Materials)
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
	for(UWidget* Child : CraftingCanvasPanel->GetAllChildren())
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
				float CanvasScale = CraftingCanvasPanel->GetCachedGeometry().Scale / RootNodeWidthScale;
				Start = CraftingNode->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(1.f, .5f));
				End = Start + FVector2D(NodeEdgeLineOffset * NodeHorizontalSeparation, 0.f) * CanvasScale;
				UWidgetBlueprintLibrary::DrawLine(Context, Start, End,NodeEdgeLineColor, true, NodeEdgeLineThickness);
				Start = End;
				End = CraftingNode->GetParentNode()->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(0.f, .5f));
				UWidgetBlueprintLibrary::DrawLine(Context, Start, End, NodeEdgeLineColor, true, NodeEdgeLineThickness);
			}
		}
	}	
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);
}
