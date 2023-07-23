#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CraftingItemData.generated.h"

USTRUCT()
struct FCraftingItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY()
	FName CraftingNodeName = NAME_None;
	
	UPROPERTY(EditAnywhere)
	class UCraftingNodeDataAsset* CraftingNodeDataAsset = nullptr;
};
