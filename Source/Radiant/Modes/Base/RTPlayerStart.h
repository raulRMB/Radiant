// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Util/Enums/TeamId.h"
#include "RTPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARTPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, EditAnywhere, Category="PlayerStart")
	ETeamId TeamId = ETeamId::Neutral;

	UPROPERTY(Replicated, EditAnywhere, Category="PlayerStart")
	uint8 bOccupied : 1;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
