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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	TArray<class ARadiantPlayerController*> PlayerControllers;

	UPROPERTY(EditAnywhere, Category="GameMode")
	int TeamSize = 1;
protected:
	virtual void OnPostLogin(AController* NewPlayer) override;
private:
	friend class UUtil;
	class AHero* GetHeroFromPlayerID(int PlayerID);

	virtual void HandleMatchHasStarted() override;
};
