// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Modes/RTGameMode.h"
#include "TestGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ATestGameMode : public ARTGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 TestTeamSize = 1;

	UPROPERTY(EditAnywhere)
	int32 TestTeamCount = 1;
	
	
	virtual void BeginPlay() override;
};
