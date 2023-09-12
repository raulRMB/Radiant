// Copyright Radiant Studios

#include "ClientSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UClientSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UClientSubsystem::Setup()
{
	WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass()));
}

void UClientSubsystem::LoginUser(const FString& Name, const FString& Password)
{
	
}

void UClientSubsystem::RegisterUser(const FString& Email, const FString& Name, const FString& Password)
{

}

void UClientSubsystem::StartMatchmaking()
{
}

void UClientSubsystem::SetQueueName(const FString& Name)
{
	if (!bIsMatchmaking)
	{
		QueueName = Name;
	}
}

bool UClientSubsystem::IsUserMatchmaking()
{
	return bIsMatchmaking;
}

void UClientSubsystem::CancelMatchmaking()
{
	
}

void UClientSubsystem::GetMatchmakingTicketResult()
{
	
}

void UClientSubsystem::SwitchPage(FString PageName)
{
	OnWidgetSwitchPage.Broadcast();
	if (!WidgetManager)
	{
		WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass()));
	}
	if (WidgetManager)
	{
		WidgetManager->SwitchTo(PageName);
	}
}

// void UClientSubsystem::InitMultiplayerApi(const PlayFab::ClientModels::FLoginResult& Result)
// {
// 	// Initialize the PFMultiplayer library.
// 	HRESULT hr = S_OK;
// 	hr = PFMultiplayerInitialize("DAAB1", &g_pfmHandle);
// 	if (FAILED(hr))
// 	{
// 		// handle initialize failure
// 		RTPRINT("FAILED")
// 	}
//
// 	PFEntityKey entityKey;
// 	// Set an entity token for a local user. The token is used to authenticate PlayFab operations on behalf of this user. 
// 	// Tokens can expire, and this API token should be called again when this token is refreshed.
// 	char* result = TCHAR_TO_ANSI(*Result.EntityToken.Get()->Entity.Get()->Id);
// 	entityKey.id = result;
// 	result = TCHAR_TO_ANSI(*Result.EntityToken.Get()->Entity.Get()->Type);
// 	entityKey.type = result;
//
// 	hr = PFMultiplayerSetEntityToken(g_pfmHandle, &entityKey, TCHAR_TO_ANSI(*Result.EntityToken.Get()->EntityToken));
// 	if (FAILED(hr))
// 	{
// 		// handle set entity token failure
// 		RTPRINT("FAILED 2")
// 	}
// 	hr = AllowInvitations(&entityKey);
// 	if(FAILED(hr))
// 	{
// 		RTPRINT("FAILED 3")
// 	}
// 	
// 	PFLobbyInviteListenerStatusChangedStateChange invite;
// 	invite.listeningEntity = entityKey;
// }
// bool UClientSubsystem::HandleInvitationListenerStatusChange(const PFLobbyInviteListenerStatusChangedStateChange& invitationListenerStateChange)
// {
// 	PFLobbyInviteListenerStatus status;
// 	HRESULT hr = PFMultiplayerGetLobbyInviteListenerStatus(
// 		g_pfmHandle,
// 		&invitationListenerStateChange.listeningEntity,
// 		&status);
// 	if(FAILED(hr))
// 	{
// 		RTPRINT("Failed 4")
// 	}
//
// 	switch (status)
// 	{
// 	case PFLobbyInviteListenerStatus::NotListening:
// 		{
// 			RTPRINT("Idle")
// 			break;
// 		}
// 	case PFLobbyInviteListenerStatus::Listening:
// 		{
// 			RTPRINT("Listening")
// 			return true;
// 		}
// 	case PFLobbyInviteListenerStatus::NotAuthorized:
// 		{
// 			RTPRINT("Invitation listener not authorized!"); // this is likely an issue with the listener's entity token.
// 			break;
// 		}
// 	default:
// 		{
// 			RTPRINT("Error");
// 			break;
// 		}
// 	}
// 	return false;
// }
//
// HRESULT UClientSubsystem::AllowInvitations(const PFEntityKey* entity)
// {
// 	return PFMultiplayerStartListeningForLobbyInvites(g_pfmHandle, entity);
// }
//
// HRESULT SendInvite(PFLobbyHandle lobby, const PFEntityKey* sender, const PFEntityKey* receiver)
// {
// 	return PFLobbySendInvite(lobby, sender, receiver, nullptr);
// }
//
// void HandleInvitationNotification(const PFLobbyInviteReceivedStateChange& invite)
// {
// 	RTPRINT("invited to lobby");
// 	// pass invite.connectionString to PFMultiplayerJoinLobby
// }


void UClientSubsystem::Logout()
{
	bIsLoggedIn = false;
	EntityId = "";
	EntityType = "";
	SwitchPage(FString("LoginMenu"));
}