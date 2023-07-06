// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Modes/Base/RTGameMode.h"
#include "KingOfTheHillMode.generated.h"


UCLASS()
class RADIANT_API AKingOfTheHillMode : public ARTGameMode
{
	GENERATED_BODY()

	virtual bool ReadyToEndMatch_Implementation() override;

	virtual void HandleMatchHasEnded() override;
};
