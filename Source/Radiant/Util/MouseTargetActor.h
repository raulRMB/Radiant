// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/Interface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "MouseTargetActor.generated.h"


UCLASS()
class AMouseTargetActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, EditAnywhere, meta = (ExposeOnSpawn = true), BlueprintReadWrite, category = "Targeting")
	FVector MouseVec;
	
	virtual void ConfirmTargetingAndContinue() override
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		FGameplayAbilityTargetData_LocationInfo* TargetData = new FGameplayAbilityTargetData_LocationInfo(); //** USE OF new() IS **REQUIRED** **
		TargetData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		TargetData->TargetLocation.LiteralTransform = FTransform(MouseVec);
		TargetDataHandle.Add(TargetData);
		TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	}
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AMouseTargetActor, MouseVec);
	}

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	
};

