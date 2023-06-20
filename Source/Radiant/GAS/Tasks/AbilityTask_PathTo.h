// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PathTo.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FPathToSignature, bool);

/**
 * 
 */
UCLASS()
class RADIANT_API UAbilityTask_PathTo : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_PathTo();
	
	static UAbilityTask_PathTo* PathTo(class UGameplayAbility* OwningAbility, FName TaskInstanceName, FHitResult HitResult);

	FPathToSignature OnPathToComplete;

	void OnUnitDied();
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY(Replicated)
	class AActor* Target;
	UPROPERTY(Replicated)
	uint8 bFollow = 1;
	UPROPERTY(Replicated)
	FVector Location;
};
