// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "RadiantGameModeBase.h"
#include "ClientGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API AClientGameMode : public ARadiantGameModeBase
{
	GENERATED_BODY()

public:
	AClientGameMode();

	virtual void BeginPlay() override;
};
