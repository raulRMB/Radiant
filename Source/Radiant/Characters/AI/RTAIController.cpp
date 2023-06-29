// Copyright Radiant Studios


#include "Characters/AI/RTAIController.h"

#include "RTAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ARTAIController::ARTAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ARTAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(!InPawn)
		return;
	if(ARTAICharacter* AICharacter = Cast<ARTAICharacter>(InPawn))
	{
		if(UBehaviorTree* BehaviorTree = AICharacter->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(AICharacter->GetBehaviorTree()->BlackboardAsset));
			BehaviorTreeComponent->StartTree(*BehaviorTree);
		}
	}
}

UBlackboardComponent* ARTAIController::GetBlackboardComponent()
{
	return BlackboardComponent;
}

void ARTAIController::BeginPlay()
{
	Super::BeginPlay();
}

