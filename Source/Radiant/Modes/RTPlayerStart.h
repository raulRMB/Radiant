// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
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
	uint8 TeamId : 1;

	UPROPERTY(Replicated, EditAnywhere, Category="PlayerStart")
	uint8 bOccupied : 1;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
