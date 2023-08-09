#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTooltipData.generated.h"

USTRUCT()
struct FTooltipData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UTexture2D* Icon = nullptr;

	UPROPERTY(EditAnywhere)
	FLinearColor Color = FLinearColor::White;
};
