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

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* GearTypeText;
	
	virtual bool CheckCanSwapWith(UItemSlot* ItemSlot);
	virtual void OnAfterFill() override;
	virtual void OnBeforeEmpty() override;
	virtual void OnBeforeSwap() override;
	virtual void OnBeforeItemDropped() override;
	virtual void OnAfterSwap() override;
	virtual void NativeConstruct() override;
};
