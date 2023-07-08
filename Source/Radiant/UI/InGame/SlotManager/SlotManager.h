// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Util/Enums/UISlotID.h"
#include "UI/UISlot.h"
#include "SlotManager.generated.h"

UCLASS()
class RADIANT_API USlotManager : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<EUISlotID, class UUISlot*> Slots;
	
public:
	UFUNCTION()
	void OnSlotChanged(const FName& Name, uint32 Amount) const;
private:
	UUISlot* GetSlot(const FName& Name) const;
	UUISlot* GetSlot(EUISlotID UISlotID) const;
	UUISlot* FindEmptySlot() const;
	FUISlotData CreateSlotData(const FName& Name, uint32 Amount) const;
};
