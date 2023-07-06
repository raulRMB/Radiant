// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_GetRandomLocation.generated.h"


UCLASS()
class RADIANT_API UBTT_GetRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Search)
	FBlackboardKeySelector RandomLocationKey;

	UPROPERTY(EditAnywhere, Category=Search)
	float Radius = 1000.f;

	UPROPERTY(EditAnywhere, Category=Search)
	float AcceptanceRadius = 10.f;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
