// Copyright Radiant Studios


#include "CraftingPanel.h"
#include "CraftingNode.h"
#include "Components/Button.h"
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
	Count = 0;
	CraftingCanvasPanel->ClearChildren();
	
	if(!CraftingItemDataTable)
		return;
	FCraftingItemData* CraftingItemData = CraftingItemDataTable->FindRow<FCraftingItemData>(ItemName, TEXT("Crafting Panel Load Crafting Item"));
	UCraftingNodeDataAsset* CraftingNodeDataAsset = CraftingItemData->CraftingNodeDataAsset;
	if(!CraftingNodeDataAsset)
	{
		return;
	}
	
	FGraphNode* Root = new FGraphNode(nullptr, CraftingNodeDataAsset, 1, 0, 1, 0);
	int32 Depth = 0;
	AddGraphNode(CraftingNodeDataAsset, Root, Depth);

	TraverseInitPositions(static_cast<float>(Root->GetWidth()) * .5f, Root);

	TraverseCreateNode(Root);

	delete Root;
}

void UCraftingPanel::CreateCraftingNode(FGraphNode* Node)
{
	if(UCraftingNode* CraftingNode = CreateWidget<UCraftingNode>(this, CraftingNodeClass))
	{
		CraftingNode->Init(Node->GetDataAsset()->Icon, Node->GetAmount());
		if(UCanvasPanelSlot* PanelSlot = CraftingCanvasPanel->AddChildToCanvas(CraftingNode))
		{
			PanelSlot->SetAnchors(FAnchors(.8f, .5f, .8f, .5f));
			PanelSlot->SetSize(FVector2D(100.f));
			PanelSlot->SetAlignment(FVector2D(.5f));
			PanelSlot->SetPosition(Node->GetPosition() * 120.f);
		}
	}
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
			if(Node->GetParent())
			{
				Node->GetParent()->Grow();
			}
		}
		AddGraphNode(Child.Key, ChildNode, Depth);
		Idx++;
	}
	Depth--;
}

void UCraftingPanel::TraverseInitPositions(float RootHalfWidth, FGraphNode* Node)
{
	Node->InitPosition(RootHalfWidth, Count);
	for(auto Child : **Node)
	{
		TraverseInitPositions(RootHalfWidth, Child);
	}
	if(Node->GetChildCount() == 0)
		Count++;
	RTLOGP4("Name: %s, Depth: %d, Width: %d, Index: %d", *Node->GetDataAsset()->Name.ToString(), Node->GetDepth(), Node->GetWidth(), Node->GetIndex());
}

void UCraftingPanel::TraverseCreateNode(FGraphNode* Node)
{
	CreateCraftingNode(Node);
	for(auto Child : **Node)
	{
		TraverseCreateNode(Child);
	}
}
