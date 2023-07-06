// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RTAIController.generated.h"


UCLASS()
class RADIANT_API ARTAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARTAIController();

	virtual void OnPossess(APawn* InPawn) override;
	UBlackboardComponent* GetBlackboardComponent();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Behavior)
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Behavior)
	class UBlackboardComponent* BlackboardComponent;
};
