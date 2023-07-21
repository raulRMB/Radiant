#include "CraftingNode.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

FGraphNode::~FGraphNode()
{
	for(FGraphNode* Child : Children)
	{
		delete Child;
	}
}

void FGraphNode::InitPosition(float RootHalfWidth, int32 Count)
{
	float Offset = 0.f;
	if (Parent)
	{
		Offset = -RootHalfWidth + Count + static_cast<float>(GetWidth()) * .5f;
	}
	Position = { -static_cast<float>(Depth), Offset };
}

void UCraftingNode::Init(UTexture2D* Texture, uint8 Amount) const
{
	Icon->SetBrushFromTexture(Texture);
	if(Amount < 1)
	{
		AmountTextBlock->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	AmountTextBlock->SetText(FText::FromString(FString::FromInt(Amount)));
}
