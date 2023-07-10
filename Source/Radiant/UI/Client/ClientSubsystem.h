// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "PFEntityKey.h"
#include "PlayFab.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabMultiplayerDataModels.h"
#include "PlayFabError.h"
#include "WidgetManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientSubsystem.generated.h"

DECLARE_DELEGATE_TwoParams(FMatchmakingStatus, bool, FString)
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayFabLoginErrorMessage, const PlayFab::FPlayFabCppError&)
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayFabRegisterErrorMessage, const PlayFab::FPlayFabCppError&)
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayFabLobbyErrorMessage, const PlayFab::FPlayFabCppError&)
DECLARE_MULTICAST_DELEGATE(FWidgetSwitchPage)

UCLASS()
class RADIANT_API UClientSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FMatchmakingStatus OnMatchmakingStatusChanged;
	FPlayFabLoginErrorMessage OnLoginErrorMessage;
	FPlayFabLoginErrorMessage OnRegisterErrorMessage;
	FPlayFabLobbyErrorMessage OnLobbyErrorMessage;
	FWidgetSwitchPage OnWidgetSwitchPage;
	FString Username;
	FString QueueName = "1v1";
private:
	PlayFabClientPtr clientAPI = nullptr;
	PlayFabMultiplayerPtr multiplayerAPI = nullptr;
	PlayFabMatchmakerPtr matchmakerAPI = nullptr;

	FString SessionId;
	FString EntityId;
	FString EntityType;
	FString TicketId;

	uint8 bIsMatchmaking : 1;
	uint8 bIsLoggedIn : 1;

	UPROPERTY()
	class AWidgetManager* WidgetManager = nullptr;
	FTimerHandle HGetTicketResult;
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	void Setup();
	UFUNCTION(BlueprintCallable)
		void SetQueueName(const FString& QueueName);
	UFUNCTION(BlueprintCallable)
		bool IsUserMatchmaking();
	void OnLoginError(const PlayFab::FPlayFabCppError& PlayFabCppError);
	UFUNCTION()
		void LoginUser(const FString& Username, const FString& Password);

	void OnRegisterError(const PlayFab::FPlayFabCppError& PlayFabCppError);
	UFUNCTION()
		void RegisterUser(const FString& Email, const FString& Username, const FString& Password);

	void OnLobbyError(const PlayFab::FPlayFabCppError& PlayFabCppError);
	UFUNCTION(Exec)
		void StartMatchmaking();

	UFUNCTION()
		void CancelMatchmaking();
	
	UFUNCTION()
		void GetMatchmakingTicketResult();
	void SwitchPage(FString PageName);

	UFUNCTION()
		void Logout();

	bool GetIsMatchMaking() const { return bIsMatchmaking; }
	void SetIsMatchMaking(bool IsMatchmaking) { bIsMatchmaking = IsMatchmaking; }

	bool GetIsLoggedIn() const { return bIsLoggedIn; }
	void SetIsLoggedIn(bool IsLoggedIn) { bIsLoggedIn = IsLoggedIn; }
private:
	void OnGetUserDataSuccess(const PlayFab::ClientModels::FGetAccountInfoResult& GetAccountInfoResult);
	void OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result);
	
	void OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result);

	void OnCreateMatchmakingTicketSuccess(const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result);
	
	void OnGetMatchmakingTicketSuccess(const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result);

	void OnGetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& Result);

	void OnCancelAllMatchmakingTicketsForPlayerSuccess(const PlayFab::MultiplayerModels::FCancelAllMatchmakingTicketsForPlayerResult& Result);
	
	void OnError(const PlayFab::FPlayFabCppError& ErrorResult);

	/********************************* C++ API ***************************************/
	
	void InitMultiplayerApi(const PlayFab::ClientModels::FLoginResult &Result);
	
	/********************************* C++ API END ***********************************/
};
