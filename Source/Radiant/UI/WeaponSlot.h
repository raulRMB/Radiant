// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "UI/ItemSlot.h"
#include "WeaponSlot.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UWeaponSlot : public UItemSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UTexture2D* EmptyHandsIcon;

	virtual void SetEmpty(const bool Empty) override;
public:
	bool CheckCanSwapWith(UItemSlot* ItemSlot);
	virtual void SetData(const FItemSlotData& Data) override;
};
