// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/Abilities/RTAbility.h"
#include "RTAbilitySystemComponent.generated.h"

DECLARE_DELEGATE(FCancelActiveToggleableAbilitySignature)

UCLASS()
class RADIANT_API URTAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	FCancelActiveToggleableAbilitySignature OnCancelActiveToggleableAbility;
	
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
