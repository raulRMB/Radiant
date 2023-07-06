// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Items/Inventory.h"
#include "Items/WorldItem.h"
#include "UObject/Interface.h"
#include "Carrier.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCarrier : public UInterface
{
	GENERATED_BODY()
};


class RADIANT_API ICarrier
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UInventory* GetInventory() const = 0;
	virtual FVector GetCarrierLocation() const = 0;
	
	void PickUpItem(const FName& ItemName);
	virtual void DropItem(const FName& ItemName);
};
