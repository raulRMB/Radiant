// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enums/ClassType.h"
#include "Enums/ItemType.h"
#include "ItemData.generated.h"

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName DisplayName;

	UPROPERTY(EditAnywhere)
	bool Enabled = true;
	
	UPROPERTY(EditAnywhere)
	EItemType ItemType;

	UPROPERTY(EditAnywhere)
	EClassType ClassType;
	
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

	FItemData()
	{
		AbilityData = nullptr;
		GearData = nullptr;
		CraftingNodeData = nullptr;
		Tooltip = FText::FromString("");
		Icon = nullptr;
	}
};
