// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Modes/RTGameMode.h"
#include "ClientGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API AClientGameMode : public ARTGameMode
{
	GENERATED_BODY()

public:
	AClientGameMode();

	virtual void BeginPlay() override;
};
