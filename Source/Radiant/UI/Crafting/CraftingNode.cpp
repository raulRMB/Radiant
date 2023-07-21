#include "CraftingNode.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/RTHUD.h"

FGraphNode::~FGraphNode()
{
	for(FGraphNode* Child : Children)
	{
		delete Child;
	}
}

void FGraphNode::Grow()
{
	if(Parent)
	{
		Parent->Grow();
	}
	Width++;
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
	if(Icon)
	{
		Icon->SetBrushFromTexture(Texture);
	}
	if(Amount < 2)
	{
		AmountTextBlock->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if(AmountTextBlock)
	{
		AmountTextBlock->SetText(FText::FromString(FString::FromInt(Amount)));
	}
}

void UCraftingNode::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.AddUniqueDynamic(this, &UCraftingNode::OnButtonClicked);
}

void UCraftingNode::OnButtonClicked()
{
	if(CraftingItemDataName.IsValid())
	{
		GetOwningPlayer()->GetHUD<ARTHUD>()->ReloadCraftingPanel(CraftingItemDataName);
	}
}
