// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "WidgetManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientSubsystem.generated.h"

struct PFLobbyInviteListenerStatusChangedStateChange;
DECLARE_DELEGATE_TwoParams(FMatchmakingStatus, bool, FString)
DECLARE_MULTICAST_DELEGATE(FWidgetSwitchPage)

UCLASS()
class RADIANT_API UClientSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	FMatchmakingStatus OnMatchmakingStatusChanged;
	FWidgetSwitchPage OnWidgetSwitchPage;
	FString Username;
	FString QueueName = "1v1";

private:
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
	void SwitchPage(FString PageName);

	UFUNCTION()
	void Logout();

	bool GetIsMatchMaking() const { return bIsMatchmaking; }
	void SetIsMatchMaking(bool IsMatchmaking) { bIsMatchmaking = IsMatchmaking; }

	bool GetIsLoggedIn() const { return bIsLoggedIn; }
	void SetIsLoggedIn(bool IsLoggedIn) { bIsLoggedIn = IsLoggedIn; }

private:
	/********************************* C++ API ***************************************/

	//void InitMultiplayerApi(const PlayFab::ClientModels::FLoginResult& Result);
	//bool HandleInvitationListenerStatusChange(const PFLobbyInviteListenerStatusChangedStateChange& invitationListenerStateChange);
	//HRESULT AllowInvitations(const PFEntityKey* entity);
	
	/********************************* C++ API END ***********************************/
};
