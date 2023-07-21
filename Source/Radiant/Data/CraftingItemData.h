#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CraftingItemData.generated.h"

USTRUCT()
struct FCraftingItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UCraftingNodeDataAsset* CraftingNodeDataAsset;
};
