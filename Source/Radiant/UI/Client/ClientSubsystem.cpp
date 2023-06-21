// Copyright Radiant Studios


#include "ClientSubsystem.h"

#include "PlayFab.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabMultiplayerDataModels.h"
#include "PlayFabRuntimeSettings.h"
#include "WidgetManager.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabMultiplayerAPI.h"
#include "Kismet/GameplayStatics.h"

void UClientSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = TEXT("DAAB1");

	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	multiplayerAPI = IPlayFabModuleInterface::Get().GetMultiplayerAPI();
}

void UClientSubsystem::Setup()
{
	WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass()));
}

void UClientSubsystem::LoginUser(const FString& Name, const FString& Password)
{
	PlayFab::ClientModels::FLoginWithPlayFabRequest request;
	request.Username = Name;
	request.Password = Password;

	clientAPI->LoginWithPlayFab(request, PlayFab::UPlayFabClientAPI::FLoginWithPlayFabDelegate::CreateUObject(this,
		&UClientSubsystem::OnLoginSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UClientSubsystem::OnLoginError));
}

void UClientSubsystem::OnRegisterError(const PlayFab::FPlayFabCppError& PlayFabCppError)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *PlayFabCppError.ErrorMessage);
	OnRegisterErrorMessage.Broadcast(PlayFabCppError);
}

void UClientSubsystem::RegisterUser(const FString& Email, const FString& Name, const FString& Password)
{
	PlayFab::ClientModels::FRegisterPlayFabUserRequest request;
	request.Email = Email;
	request.Username = Name;
	request.Password = Password;

	clientAPI->RegisterPlayFabUser(request, PlayFab::UPlayFabClientAPI::FRegisterPlayFabUserDelegate::CreateUObject(this,
		&UClientSubsystem::OnRegisterSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UClientSubsystem::OnRegisterError));
}

void UClientSubsystem::OnLobbyError(const PlayFab::FPlayFabCppError& PlayFabCppError)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *PlayFabCppError.ErrorMessage);
	OnLobbyErrorMessage.Broadcast(PlayFabCppError);
}

void UClientSubsystem::StartMatchmaking()
{
	PlayFab::MultiplayerModels::FCreateMatchmakingTicketRequest Request;
	Request.GiveUpAfterSeconds = 120;
	Request.QueueName = QueueName;
	Request.Creator = PlayFab::MultiplayerModels::FMatchmakingPlayer();
	Request.Creator.Entity.Id = EntityId;
	Request.Creator.Entity.Type = EntityType;

	TArray<TSharedPtr<FJsonValue>> Latencies;

	TSharedPtr<FJsonObject> EastUsWrapper = MakeShareable(new FJsonObject());
	EastUsWrapper->SetStringField("region", "EastUs");
	EastUsWrapper->SetNumberField("latency", 50);
	TSharedPtr<FJsonValue> RegionWrapper = MakeShareable(new FJsonValueObject(EastUsWrapper));
	Latencies.Add(RegionWrapper);
	
	TSharedPtr<FJsonObject> DataObject = MakeShareable(new FJsonObject());
	DataObject->SetArrayField("latencies", Latencies);

	TSharedPtr<FJsonObject> AttributesObject = MakeShareable(new FJsonObject());
	
	TSharedPtr<PlayFab::MultiplayerModels::FMatchmakingPlayerAttributes> Attributes = MakeShareable( new PlayFab::MultiplayerModels::FMatchmakingPlayerAttributes(AttributesObject));
	Attributes->DataObject = MakeShareable(new FJsonValueObject(DataObject));
	Request.Creator.Attributes = Attributes;
	
	multiplayerAPI->CreateMatchmakingTicket(Request,
		PlayFab::UPlayFabMultiplayerAPI::FCreateMatchmakingTicketDelegate::CreateUObject(this, &UClientSubsystem::OnCreateMatchmakingTicketSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UClientSubsystem::OnLobbyError)
	);

	UE_LOG(LogTemp, Warning, TEXT("User has started matchmaking %s"), *Request.QueueName)
}

void UClientSubsystem::SetQueueName(const FString& Name)
{
	QueueName = Name;
}

void UClientSubsystem::OnLoginError(const PlayFab::FPlayFabCppError& PlayFabCppError)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *PlayFabCppError.ErrorMessage);
	OnLoginErrorMessage.Broadcast(PlayFabCppError);
}

void UClientSubsystem::CancelMatchmaking()
{
	PlayFab::MultiplayerModels::FCancelAllMatchmakingTicketsForPlayerRequest Request;
	Request.QueueName = QueueName;
	Request.Entity = MakeShareable(new PlayFab::MultiplayerModels::FEntityKey());
	Request.Entity->Id = EntityId;
	Request.Entity->Type = EntityType;
	
	multiplayerAPI->CancelAllMatchmakingTicketsForPlayer(Request,
		PlayFab::UPlayFabMultiplayerAPI::FCancelAllMatchmakingTicketsForPlayerDelegate::CreateUObject(this, &UClientSubsystem::OnCancelAllMatchmakingTicketsForPlayerSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UClientSubsystem::OnLobbyError)
	);
}

void UClientSubsystem::GetMatchmakingTicketResult()
{
	PlayFab::MultiplayerModels::FGetMatchmakingTicketRequest Request;
	Request.QueueName = QueueName;
	Request.TicketId = TicketId;
	
	multiplayerAPI->GetMatchmakingTicket(Request,
		PlayFab::UPlayFabMultiplayerAPI::FGetMatchmakingTicketDelegate::CreateUObject(this, &UClientSubsystem::OnGetMatchmakingTicketSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UClientSubsystem::OnError)
		);
}

void UClientSubsystem::OnGetUserDataSuccess(const PlayFab::ClientModels::FGetAccountInfoResult& GetAccountInfoResult)
{
	Username = GetAccountInfoResult.AccountInfo.Get()->Username;
	if(!WidgetManager)
		WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass()));
	if(WidgetManager)
		WidgetManager->SwitchTo(FString("LobbyMenu"));
	bIsLoggedIn = true;
}

void UClientSubsystem::OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result)
{
	EntityId = Result.EntityToken.Get()->Entity.Get()->Id;
	EntityType = Result.EntityToken.Get()->Entity.Get()->Type;
	PlayFab::ClientModels::FGetAccountInfoRequest request = PlayFab::ClientModels::FGetAccountInfoRequest();
	request.PlayFabId = Result.PlayFabId;
	clientAPI->GetAccountInfo(request,
		PlayFab::UPlayFabClientAPI::FGetAccountInfoDelegate::CreateUObject(this, &UClientSubsystem::OnGetUserDataSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UClientSubsystem::OnError));
}

void UClientSubsystem::Logout()
{
	bIsLoggedIn = false;
	EntityId = "";
	EntityType = "";
	if(WidgetManager)
		WidgetManager->SwitchTo(FString("LoginMenu"));

}

void UClientSubsystem::OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result)
{
	if(!WidgetManager)
		WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass()));
	if(WidgetManager)
		WidgetManager->SwitchTo(FString("LoginMenu"));
	
	UE_LOG(LogTemp, Warning, TEXT("Register Success"));
}

void UClientSubsystem::OnCreateMatchmakingTicketSuccess(
	const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result)
{
	PlayFab::MultiplayerModels::FGetMatchmakingTicketRequest Request;
	TicketId = Result.TicketId;
	
	GetWorld()->GetTimerManager().SetTimer(HGetTicketResult, this, &UClientSubsystem::GetMatchmakingTicketResult, 6.2f, true, -1);

	bIsMatchmaking = true;

	OnToggleQueueButtons.ExecuteIfBound(bIsMatchmaking, "");
	
	UE_LOG(LogTemp, Warning, TEXT("On Create Matchmaking Ticket success"));
}

void UClientSubsystem::OnGetMatchmakingTicketSuccess(
	const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result)
{
	if(Result.Status == "Matched")
	{
		GetWorld()->GetTimerManager().ClearTimer(HGetTicketResult);

		PlayFab::MultiplayerModels::FGetMatchRequest Request;
		Request.MatchId = Result.MatchId;
		Request.QueueName = QueueName;

		multiplayerAPI->GetMatch(Request, PlayFab::UPlayFabMultiplayerAPI::FGetMatchDelegate::CreateUObject(this, &UClientSubsystem::OnGetMatchSuccess),
			PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UClientSubsystem::OnError)
			);

		UE_LOG(LogTemp, Warning, TEXT("User has found a match!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("User is in queue"));
	}
}

FWidgetSwitchPage* UClientSubsystem::GetPageChangeDelegate()
{
	if(!WidgetManager)
		WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass()));
	if(WidgetManager)
		return &WidgetManager->OnWidgetSwitchPage;
	return nullptr;
}

void UClientSubsystem::OnGetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& Result)
{
	if(Result.pfServerDetails)
	{
		FString Address = Result.pfServerDetails->IPV4Address;
		TArray<PlayFab::MultiplayerModels::FPort> Ports = Result.pfServerDetails->Ports;
		if(!Address.IsEmpty() && Ports.Num() > 0)
		{
			Address.Append(":");
			Address.AppendInt(Ports[0].Num);
			UE_LOG(LogTemp, Warning, TEXT("Loading Address: %s"), *Address);
			UGameplayStatics::OpenLevel(GetWorld(), FName(Address));
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Server Address!"));
			bIsMatchmaking = false;
			OnToggleQueueButtons.ExecuteIfBound(bIsMatchmaking, "Failed To Find Server");
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Server Address!"));
		bIsMatchmaking = false;
		OnToggleQueueButtons.ExecuteIfBound(bIsMatchmaking, "Failed To Find Server");
	}
}

void UClientSubsystem::OnCancelAllMatchmakingTicketsForPlayerSuccess(
	const PlayFab::MultiplayerModels::FCancelAllMatchmakingTicketsForPlayerResult& Result)
{
	bIsMatchmaking = false;

	OnToggleQueueButtons.ExecuteIfBound(bIsMatchmaking, "");

	GetWorld()->GetTimerManager().ClearTimer(HGetTicketResult);
	
	UE_LOG(LogTemp, Warning, TEXT("User has cancelled all matchmaking tickets"));
}

void UClientSubsystem::OnError(const PlayFab::FPlayFabCppError& ErrorResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorResult.ErrorMessage);
}
