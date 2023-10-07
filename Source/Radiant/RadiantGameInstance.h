// Copyright Radiant Studios

#pragma once

#define UE_ALLOW_MAP_OVERRIDE_IN_SHIPPING 1

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RadiantGameInstance.generated.h"

UCLASS()
class RADIANT_API URadiantGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTable;

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
