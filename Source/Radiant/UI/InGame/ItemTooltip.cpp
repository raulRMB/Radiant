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

void UItemTooltip::ShowTooltip()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UItemTooltip::NativeConstruct()
{
	Super::NativeConstruct();
	ShowTooltip();
}

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
	FSlateFontInfo FontInfo = TextBlock->GetFont();
	FontInfo.Size = 11;
	TextBlock->SetFont(FontInfo);
	return TextBlock;
}

void UItemTooltip::Init(FItemData* ItemData, FText Name)
{
	ItemName->SetText(FText::FromName(ItemData->DisplayName));
	Description->SetText(ItemData->Tooltip);
	ItemType->SetText(UUtil::ItemTypeToText(ItemData->ItemType));
	ItemType->SetColorAndOpacity(UUtil::ItemTypeToColor(ItemData->ItemType));
	ClassType->SetText(UUtil::ClassTypeToText(ItemData->ClassType));
	ClassType->SetColorAndOpacity(UUtil::ClassTypeToColor(ItemData->ClassType));
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
