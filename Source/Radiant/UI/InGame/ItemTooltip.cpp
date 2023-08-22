#include "ItemTooltip.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "..\..\Data\TooltipData.h"
#include "Data/AbilityDataAsset.h"
#include "Data/GearData.h"
#include "Util/Util.h"

void UItemTooltip::AddStatInfo(FTooltipStatInfo StatInfo)
{
	StatInfos.Add(StatInfo);
}

UTextBlock* UItemTooltip::CreateTextBlock(UObject* Outer, FTooltipStatInfo& StatInfo)
{
	UTextBlock* TextBlock = NewObject<UTextBlock>(Outer);
	FText Text;
	switch (StatInfo.ModOp)
	{
	case EGameplayModOp::Additive:
		Text = RTPRINTF("%.0f", StatInfo.Value);
		break;
	case EGameplayModOp::Multiplicitive:
		float Value = (StatInfo.Value - 1.f) * 100.f;
		Text = RTPRINTF("%.0f%%", Value);
		break;
	}
	TextBlock->SetText(Text);
	TextBlock->Font.Size = 11;
	return TextBlock;
}

void UItemTooltip::Init(FItemData* ItemData, FText Name)
{
	ItemName->SetText(Name);
	Description->SetText(ItemData->Tooltip);
	for(FTooltipStatInfo& StatInfo : StatInfos)
	{
		FTooltipData* TooltipData = ItemTooltipData->FindRow<FTooltipData>(StatInfo.CellType, "");
		if(!TooltipData)
		{
			continue;
		}
		
		UImage* Image = NewObject<UImage>(this);
		Image->SetBrushFromTexture(TooltipData->Icon);
		Image->Brush.SetImageSize(FVector2D(20.f, 20.f));
		Image->SetColorAndOpacity(TooltipData->Color);
		UHorizontalBox* HorizontalBox = NewObject<UHorizontalBox>(this);
		if(UHorizontalBoxSlot* HorizontalBoxSlot = HorizontalBox->AddChildToHorizontalBox(Image))
		{
			HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Left);
			HorizontalBoxSlot->SetVerticalAlignment(VAlign_Center);
		}
		UTextBlock* TextBlock = CreateTextBlock(this, StatInfo);
		HorizontalBox->AddChild(TextBlock);
		VerticalBox->AddChild(HorizontalBox);
	}
}
