// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RadiantGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARadiantGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode")
	int PlayerCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode")
	TArray<class ARadiantPlayerController*> PlayerControllers;
	
protected:
	virtual void OnPostLogin(AController* NewPlayer) override;
};
