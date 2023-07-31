// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UAbilityDataAsset* AbilityData;

	UPROPERTY(EditAnywhere)
	class UGearData* GearData;

	UPROPERTY(EditAnywhere)
	class UCraftingNodeDataAsset* CraftingNodeData;

	UPROPERTY(EditAnywhere)
	FText Tooltip;

	UPROPERTY(EditAnywhere)
	class UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	uint8 bIsGear : 1;

	FItemData()
	{
		AbilityData = nullptr;
		GearData = nullptr;
		CraftingNodeData = nullptr;
		Tooltip = FText::FromString("");
		Icon = nullptr;
		bIsGear = false;
	}
};
