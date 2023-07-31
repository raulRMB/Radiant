#pragma once

#include "CoreMinimal.h"
#include "ItemSlot.h"
#include "Enums/GearType.h"
#include "GearSlot.generated.h"

UCLASS()
class RADIANT_API UGearSlot  : public UItemSlot
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	EGearType GearType;
	
	UPROPERTY(EditAnywhere)
	class UTexture2D* EmptyGearIcon;
	
	virtual bool CheckCanSwapWith(UItemSlot* ItemSlot);
	virtual void SetEmpty(const bool Empty) override;
	virtual void SetData(const FItemSlotData& Data) override;
};
