// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "PlayFab.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabMultiplayerDataModels.h"
#include "PlayFabError.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientSubsystem.generated.h"

DECLARE_DELEGATE_OneParam(FToggleQueueButtonsSignature, bool /*bIsMatchmaking*/)

/**
 * 
 */
UCLASS()
class RADIANT_API UClientSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FToggleQueueButtonsSignature OnToggleQueueButtons;
private:
	PlayFabClientPtr clientAPI = nullptr;
	PlayFabMultiplayerPtr multiplayerAPI = nullptr;
	PlayFabMatchmakerPtr matchmakerAPI = nullptr;

	FString SessionId;
	FString EntityId;
	FString EntityType;
	FString QueueName;
	FString TicketId;

	uint8 bIsMatchmaking : 1;
	uint8 bIsLoggedIn : 1;

	class AWidgetManager* WidgetManager = nullptr;
	FTimerHandle HGetTicketResult;
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	void Setup();
	
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

	bool GetIsLoggedIn() const { return bIsLoggedIn; }
	void SetIsLoggedIn(bool IsLoggedIn) { bIsLoggedIn = IsLoggedIn; }
private:
	void OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result);
	void OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result);

	void OnCreateMatchmakingTicketSuccess(const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result);
	
	void OnGetMatchmakingTicketSuccess(const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result);

	void OnGetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& Result);

	void OnCancelAllMatchmakingTicketsForPlayerSuccess(const PlayFab::MultiplayerModels::FCancelAllMatchmakingTicketsForPlayerResult& Result);
	
	void OnError(const PlayFab::FPlayFabCppError& ErrorResult);
};
