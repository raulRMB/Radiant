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
	class UCraftingNodeDataAsset* CraftingNodeData;

	UPROPERTY(EditAnywhere)
	FText Tooltip;

	UPROPERTY(EditAnywhere)
	class UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	uint8 bIsWeapon : 1;

	FItemData()
	{
		AbilityData = nullptr;
		CraftingNodeData = nullptr;
		Tooltip = FText::FromString("");
		Icon = nullptr;
		bIsWeapon = false;
	}
};
