// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/Abilities/RTAbility.h"
#include "RTAbilitySystemComponent.generated.h"


UCLASS()
class RADIANT_API URTAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
