// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "AbilityTask_DisplaceTarget.generated.h"

class UCharacterMovementComponent;
class UCurveVector;
class UGameplayTasksComponent;
enum class ERootMotionFinishVelocityMode : uint8;

USTRUCT()
struct FDisplaceTargetData
{
	GENERATED_BODY()
	UPROPERTY()
	class AActor* Target;
	UPROPERTY()
	FVector Location;
	UPROPERTY()
	float Duration;

	FDisplaceTargetData()
	{
		Target = nullptr;
		Location = FVector::ZeroVector;
		Duration = 0.0f;
	}
};

DECLARE_MULTICAST_DELEGATE(FDisplaceTargetSignature)


UCLASS()
class RADIANT_API UAbilityTask_DisplaceTarget : public UAbilityTask_ApplyRootMotionMoveToForce
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	class AActor* TargetToDisplace;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	static UAbilityTask_DisplaceTarget* DisplaceTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, FDisplaceTargetData Data);

	virtual void SharedInitAndApply() override;
};
