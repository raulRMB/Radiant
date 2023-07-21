// Copyright Radiant Studios


#include "CraftingPanel.h"
#include "CraftingNode.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Data/CraftingNodeDataAsset.h"
#include "Util/Util.h"

void UCraftingPanel::CreateCraftingNode(FGraphNode* Node)
{
	if(UCraftingNode* CraftingNode = CreateWidget<UCraftingNode>(this, CraftingNodeClass))
	{
		CraftingNode->Init(Node->GetDataAsset()->Icon, Node->GetAmount());
		if(UCanvasPanelSlot* PanelSlot = CanvasPanel->AddChildToCanvas(CraftingNode))
		{
			PanelSlot->SetAnchors(FAnchors(.8f, .5f, .8f, .5f));
			PanelSlot->SetSize(FVector2D(100.f));
			PanelSlot->SetAlignment(FVector2D(.5f));
			PanelSlot->SetPosition(Node->GetPosition() * 120.f);
		}
	}
}

void UCraftingPanel::Init()
{
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
