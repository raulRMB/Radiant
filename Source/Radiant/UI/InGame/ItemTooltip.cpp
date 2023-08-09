#include "ItemTooltip.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "..\..\Data\TooltipData.h"
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
	
	return TextBlock;
}

void UItemTooltip::Init()
{
	for(FTooltipStatInfo& StatInfo : StatInfos)
	{
		FTooltipData* TooltipData = ItemTooltipData->FindRow<FTooltipData>(StatInfo.CellType, "");
		if(!TooltipData)
		{
			continue;
		}
		UImage* Image = NewObject<UImage>(this);
		Image->SetBrushFromTexture(TooltipData->Icon);
		Image->Brush.SetImageSize(FVector2D(32.f, 32.f));
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
