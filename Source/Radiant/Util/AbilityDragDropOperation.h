// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "AbilityDragDropOperation.generated.h"

UCLASS()
class RADIANT_API UAbilityDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAbilityWidget* WidgetReference;
};
