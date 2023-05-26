// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RTGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARTGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	uint32 TeamSize = 2;
	uint32 TeamCount = 2;
	uint32 PlayersLoaded = 0;
	void PlayerLoaded();
	
protected:
	virtual void OnPostLogin(AController* NewPlayer) override;
	void PlayersAreLoaded() const;

private:
	virtual bool ReadyToStartMatch_Implementation() override;

	virtual void HandleMatchHasStarted() override;
};
