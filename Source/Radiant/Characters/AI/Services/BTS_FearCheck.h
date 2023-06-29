// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_FearCheck.generated.h"


/**
 * 
 */
UCLASS()
class RADIANT_API UBTS_FearCheck : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Search)
	FBlackboardKeySelector FearKey;

	UPROPERTY(EditAnywhere, Category=Search)
	float DetectionRadius = 1000.f;

	UPROPERTY()
	TArray<class AAvatar*> PlayerActors;
public:
	UBTS_FearCheck();
	
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
	void OnPlayersUpdated(class AAvatar* Player, bool bRemoved);
};
