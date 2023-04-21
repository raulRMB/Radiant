// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "PlayFab.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabMultiplayerDataModels.h"
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
	FString QueueName;
	FString TicketId;

	FTimerHandle HGetTicketResult;

	class AWidgetManager* WidgetManager = nullptr;
public:
	AClientGameMode();
	
	virtual void BeginPlay() override;

	UFUNCTION()
		void LoginUser(const FString& Username, const FString& Password);

	UFUNCTION()
		void RegisterUser(const FString& Email, const FString& Username, const FString& Password);

	UFUNCTION(Exec)
		void StartMatchmaking();

	UFUNCTION()
		void GetMatchmakingTicketResult();

private:
	void OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result);
	void OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result);

	void OnCreateMatchmakingTicketSuccess(const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result);
	
	void OnGetMatchmakingTicketSuccess(const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result);

	void OnGetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& Result);
	
	void OnError(const PlayFab::FPlayFabCppError& ErrorResult);
};
