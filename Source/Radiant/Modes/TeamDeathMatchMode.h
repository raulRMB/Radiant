// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Modes/Base/RTGameMode.h"
#include "TeamDeathMatchMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ATeamDeathMatchMode : public ARTGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	uint32 KillsToWin = 3;

public:
	bool ReadyToStartMatch_Implementation();
};
