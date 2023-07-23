#include "CraftingNode.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Event/EventBroker.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "Player/RTPlayerState.h"
#include "UI/RTHUD.h"
#include "Util/Util.h"

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
	Button->OnHovered.AddDynamic(this, &UCraftingNode::OnButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &UCraftingNode::OnButtonUnhovered);
}

void UCraftingNode::OnButtonClicked()
{
	if(CraftingItemDataName.IsValid())
	{
		GetOwningPlayer()->GetHUD<ARTHUD>()->ReloadCraftingPanel(CraftingItemDataName);
	}
}

void UCraftingNode::OnButtonHovered()
{
	HoveredHandle = UEventBroker::Get(this)->RightMouseButtonClicked.AddUObject(this, &UCraftingNode::OnMouseRightClick);
}

void UCraftingNode::OnButtonUnhovered()
{
	UEventBroker::Get(this)->RightMouseButtonClicked.Remove(HoveredHandle);
}

void UCraftingNode::OnMouseRightClick()
{
	GetOwningPlayerState<ARTPlayerState>()->S_BuyAbility(CraftingItemDataName, 1);
	UEventBroker::Get(this)->RightMouseButtonClicked.Remove(HoveredHandle);
}

