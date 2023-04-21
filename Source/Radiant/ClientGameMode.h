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

DECLARE_DELEGATE_OneParam(FToggleQueueButtonsSignature, bool /*bIsMatchmaking*/)

/**
 * 
 */
UCLASS()
class RADIANT_API AClientGameMode : public ARadiantGameModeBase
{
	GENERATED_BODY()
public:
	FToggleQueueButtonsSignature OnToggleQueueButtons;
private:
	PlayFabClientPtr clientAPI = nullptr;
	PlayFabMultiplayerPtr multiplayerAPI = nullptr;
	PlayFabMatchmakerPtr matchmakerAPI = nullptr;

	FString EntityId;
	FString EntityType;
	FString QueueName;
	FString TicketId;

	FTimerHandle HGetTicketResult;

	uint8 bIsMatchmaking : 1;

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
		void CancelMatchmaking();
	
	UFUNCTION()
		void GetMatchmakingTicketResult();

	bool GetIsMatchMaking() const { return bIsMatchmaking; }
	void SetIsMatchMaking(bool IsMatchmaking) { bIsMatchmaking = IsMatchmaking; }
private:
	void OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result);
	void OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result);

	void OnCreateMatchmakingTicketSuccess(const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result);
	
	void OnGetMatchmakingTicketSuccess(const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result);

	void OnGetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& Result);

	void OnCancelAllMatchmakingTicketsForPlayerSuccess(const PlayFab::MultiplayerModels::FCancelAllMatchmakingTicketsForPlayerResult& Result);
	
	void OnError(const PlayFab::FPlayFabCppError& ErrorResult);
};
