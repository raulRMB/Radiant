// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RadiantGameInstance.generated.h"


UCLASS()
class RADIANT_API URadiantGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void OnStart() override;

protected:
	UFUNCTION()
	void OnGSDKShutdown();

	UFUNCTION()
	bool OnGSDKHealthCheck();

	UFUNCTION()
	void OnGSDKServerActive();

	UFUNCTION()
	void OnGSDKReadyForPlayers();
};
