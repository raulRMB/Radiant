// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "PlayFab.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabError.h"
#include "RegisterMenu.h"
#include "RadiantGameModeBase.h"
#include "ClientGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API AClientGameMode : public ARadiantGameModeBase
{
	GENERATED_BODY()

	PlayFabClientPtr clientAPI = nullptr;
	PlayFabMultiplayerPtr multiplayerAPI = nullptr;
	PlayFabMatchmakerPtr matchmakerAPI = nullptr;

	FString EntityId;
	FString EntityType;

	class AWidgetManager* WidgetManager = nullptr;
public:
	AClientGameMode();
	
	virtual void BeginPlay() override;

	UFUNCTION()
		void LoginUser(const FString& Username, const FString& Password);

	UFUNCTION()
		void RegisterUser(const FString& Email, const FString& Username, const FString& Password);

private:
	void OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result);
	void OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result);
	
	void OnError(const PlayFab::FPlayFabCppError& ErrorResult);
};
