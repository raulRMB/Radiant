// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Killable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKillable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RADIANT_API IKillable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual TArray<TSubclassOf<class UGameplayAbility>> GetDeathAbilities() const = 0;
	virtual void Die();
	virtual void GiveDeathAbilities();
};
