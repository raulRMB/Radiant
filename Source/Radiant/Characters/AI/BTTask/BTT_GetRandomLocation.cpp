// Copyright Radiant Studios


#include "BTT_GetRandomLocation.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_GetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Component = OwnerComp.GetBlackboardComponent();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation RandomNavLocation;
	NavSystem->GetRandomReachablePointInRadius(OwnerComp.GetOwner()->GetActorLocation(), Radius, RandomNavLocation);
	if(ensureMsgf(Component, TEXT("UBTT_GetRandomLocation::ExecuteTask: Blackboard component is null")))
	{
		Component->SetValueAsVector(RandomLocationKey.SelectedKeyName, RandomNavLocation.Location);
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
