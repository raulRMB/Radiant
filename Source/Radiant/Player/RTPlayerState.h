// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "RTPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARTPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	class URTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class URTHeroAttributeSetBase* AttributeSetBase;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	ARTPlayerState();
	
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	class URTHeroAttributeSetBase* GetAttributeSetBase() const;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	int32 TargetId = -1;
	UPROPERTY(Replicated, VisibleAnywhere)
	int32 TeamId = -1;
};
