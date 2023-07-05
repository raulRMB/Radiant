// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Util/Managers/GridManager.h"
#include "AbilityTask_BuildMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UAbilityTask_BuildMode : public UAbilityTask
{
	GENERATED_BODY()
	
	EEnvironmentType Type;

	UPROPERTY()
	class AActor* Actor;
public:
	UAbilityTask_BuildMode(const FObjectInitializer& ObjectInitializer);
	
	static UAbilityTask_BuildMode* BuildModeTask(UGameplayAbility* OwningAbility, FName TaskInstanceName, EEnvironmentType Type);

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityEnded) override;
	
	virtual void TickTask(float DeltaTime) override;
};
