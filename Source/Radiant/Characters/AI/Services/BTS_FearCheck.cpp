// Copyright Radiant Studios


#include "BTS_FearCheck.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Avatar.h"
#include "Util/Util.h"
#include "Util/Managers/ActorManager.h"


UBTS_FearCheck::UBTS_FearCheck()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Fear Check";
}

void UBTS_FearCheck::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	
	if(UActorManager* ActorManager = OwnerComp.GetAIOwner()->GetGameInstance()->GetSubsystem<UActorManager>())
	{
		ActorManager->OnPlayersUpdated.AddUObject(this, &UBTS_FearCheck::OnPlayersUpdated);
		ActorManager->GetAllPlayers(PlayerActors);
	}
}

void UBTS_FearCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	for(AActor* Player : PlayerActors)
	{
		if(!Player)
		{
			continue;
		}
		FVector PlayerVector = Player->GetActorLocation();
		FVector MyVector = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
		FVector FearDirection = MyVector - PlayerVector;
		FearDirection.Normalize();
		FearDirection *= 1000.f;

		if(FVector::DistSquared(PlayerVector, MyVector) < DetectionRadius * DetectionRadius)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(FearKey.SelectedKeyName, FearDirection + MyVector);
			return;
		}
	}
	
	OwnerComp.GetBlackboardComponent()->ClearValue(FearKey.SelectedKeyName);
}

void UBTS_FearCheck::OnPlayersUpdated(AAvatar* Player, bool bRemoved)
{
	if(bRemoved)
	{
		PlayerActors.Remove(Player);
	}
	else
	{
		PlayerActors.Add(Player);
	}
}
