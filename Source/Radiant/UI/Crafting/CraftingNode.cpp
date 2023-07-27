#include "CraftingNode.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Event/EventBroker.h"
#include "Player/InventoryComponent.h"
#include "Player/RTPlayerState.h"
#include "UI/DynamicButton.h"
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

void UCraftingNode::OnItemChanged(const FName& Name, uint32 Amount)
{
	CheckHasMaterials();
}

void UCraftingNode::Init(UTexture2D* Texture, uint8 Amount, UInventoryComponent* NewInventoryComponent)
{
	InventoryComponent = NewInventoryComponent;
	UEventBroker::Get(this)->ItemChanged.AddUObject(this, &UCraftingNode::OnItemChanged);
	if(Icon)
	{
		Icon->SetBrushFromTexture(Texture);
	}
	if(Amount < 2)
	{
		AmountTextBlock->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	AmountTextBlock->SetText(FText::FromString(FString::FromInt(Amount)));
}

void UCraftingNode::InitIngredientList(UCraftingNodeDataAsset* DataAsset)
{
	for(TPair<UCraftingNodeDataAsset*, uint32> Material : DataAsset->Materials)
	{
		if(IngredientList.Contains(Material.Key->Name))
		{
			IngredientList[Material.Key->Name] += Material.Value;
			continue;
		}
		IngredientList.Add(Material.Key->Name, Material.Value);
	}
}

void UCraftingNode::SetCraftingItemDataName(const FName InCraftingItemDataName)
{
	CraftingItemDataName = InCraftingItemDataName;

	if(FItemData* ItemData = UUtil::GetItemDataFromName(CraftingItemDataName))
	{
		if(ItemData->CraftingNodeData)
		{
			InitIngredientList(ItemData->CraftingNodeData);
		}
	}

	CheckHasMaterials();
}


bool UCraftingNode::CheckHasMaterials()
{
	bool bHasMaterials = true;

	if(InventoryComponent == nullptr)
	{
		return false;
	}
	
	for(TPair<FName, uint32> Ingredient : IngredientList)
	{
		if(Ingredient.Value > InventoryComponent->GetItemAmount(Ingredient.Key))
		{
			bHasMaterials = false;
			break;
		}
	}
	SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, bHasMaterials ? 1.f : .5f));
	return bHasMaterials;
}

void UCraftingNode::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnLeftClick.AddUObject(this, &UCraftingNode::OnMouseLeftClicked);
	Button->OnRightClick.AddUObject(this, &UCraftingNode::OnMouseRightClicked);
}

void UCraftingNode::OnMouseLeftClicked()
{	
	if(CraftingItemDataName.IsValid())
	{
		GetOwningPlayer()->GetHUD<ARTHUD>()->ReloadCraftingPanel(CraftingItemDataName);
	}
}

void UCraftingNode::OnMouseRightClicked()
{
	if(CheckHasMaterials())
	{
		for(auto Ingredient : IngredientList)
		{
			InventoryComponent->S_ItemUsed(Ingredient.Key, Ingredient.Value);
		}
		GetOwningPlayerState<ARTPlayerState>()->S_BuyAbility(CraftingItemDataName, 1);
	}
}

