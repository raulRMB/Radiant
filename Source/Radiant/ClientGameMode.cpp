// Copyright Radiant Studios


#include "ClientGameMode.h"

#include "PlayFab.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabMultiplayerDataModels.h"
#include "PlayFabRuntimeSettings.h"
#include "WidgetManager.h"
#include "Blueprint/UserWidget.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabMultiplayerAPI.h"
#include "Kismet/GameplayStatics.h"

AClientGameMode::AClientGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = TEXT("DAAB1");

	WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass()));
	check(WidgetManager);
	
	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	multiplayerAPI = IPlayFabModuleInterface::Get().GetMultiplayerAPI();
	matchmakerAPI = IPlayFabModuleInterface::Get().GetMatchmakerAPI();
}

void AClientGameMode::LoginUser(const FString& Username, const FString& Password)
{
	PlayFab::ClientModels::FLoginWithPlayFabRequest request;
	request.Username = Username;
	request.Password = Password;

	clientAPI->LoginWithPlayFab(request, PlayFab::UPlayFabClientAPI::FLoginWithPlayFabDelegate::CreateUObject(this,
		&AClientGameMode::OnLoginSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AClientGameMode::OnError));
}

void AClientGameMode::RegisterUser(const FString& Email, const FString& Username, const FString& Password)
{
	PlayFab::ClientModels::FRegisterPlayFabUserRequest request;
	request.Email = Email;
	request.Username = Username;
	request.Password = Password;
	
	clientAPI->RegisterPlayFabUser(request, PlayFab::UPlayFabClientAPI::FRegisterPlayFabUserDelegate::CreateUObject(this,
		&AClientGameMode::OnRegisterSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AClientGameMode::OnError));
}

void AClientGameMode::StartMatchmaking()
{
	PlayFab::MultiplayerModels::FCreateMatchmakingTicketRequest Request;
	Request.GiveUpAfterSeconds = 120;
	Request.QueueName = TEXT("1v1Test");
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
		PlayFab::UPlayFabMultiplayerAPI::FCreateMatchmakingTicketDelegate::CreateUObject(this, &AClientGameMode::OnCreateMatchmakingTicketSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AClientGameMode::OnError)
	);

	UE_LOG(LogTemp, Warning, TEXT("You are now match making for %s"), *Request.QueueName)
}

void AClientGameMode::GetMatchmakingTicketResult()
{
	PlayFab::MultiplayerModels::FGetMatchmakingTicketRequest Request;
	Request.QueueName = QueueName;
	Request.TicketId = TicketId;
	
	multiplayerAPI->GetMatchmakingTicket(Request,
		PlayFab::UPlayFabMultiplayerAPI::FGetMatchmakingTicketDelegate::CreateUObject(this, &AClientGameMode::OnGetMatchmakingTicketSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AClientGameMode::OnError)
		);
}

void AClientGameMode::OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result)
{
	EntityId = Result.EntityToken.Get()->Entity.Get()->Id;
    EntityType = Result.EntityToken.Get()->Entity.Get()->Type;

	WidgetManager->SwitchTo(FString("LobbyMenu"));
	
	UE_LOG(LogTemp, Log, TEXT("Congratulations, you are now logged into PlayFab"));
}

void AClientGameMode::OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result)
{
	WidgetManager->SwitchTo(FString("LoginMenu"));
	
	UE_LOG(LogTemp, Log, TEXT("Congratulations, you are now registered with PlayFab"));
}

void AClientGameMode::OnCreateMatchmakingTicketSuccess(
	const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result)
{
	PlayFab::MultiplayerModels::FGetMatchmakingTicketRequest Request;
	QueueName = "1v1Test";
	TicketId = Result.TicketId;
	
	GetWorld()->GetTimerManager().SetTimer(HGetTicketResult, this, &AClientGameMode::GetMatchmakingTicketResult, 6.2f, true, -1);

	UE_LOG(LogTemp, Warning, TEXT("On Create Matchmaking Ticket success"));
}

void AClientGameMode::OnGetMatchmakingTicketSuccess(
	const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("On Get Matchmaking Ticket success"));
	
	if(Result.Status == "Matched")
	{
		GetWorld()->GetTimerManager().ClearTimer(HGetTicketResult);

		PlayFab::MultiplayerModels::FGetMatchRequest Request;
		Request.MatchId = Result.MatchId;
		Request.QueueName = QueueName;

		multiplayerAPI->GetMatch(Request, PlayFab::UPlayFabMultiplayerAPI::FGetMatchDelegate::CreateUObject(this, &AClientGameMode::OnGetMatchSuccess),
			PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AClientGameMode::OnError)
			);
	}
}

void AClientGameMode::OnGetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Found Match!"));

	FString Address = Result.pfServerDetails->IPV4Address ;
	Address.Append(":");
	Address.AppendInt(Result.pfServerDetails->Ports[0].Num);
	UE_LOG(LogTemp, Warning, TEXT("Address: %s"), *Address);
	UGameplayStatics::OpenLevel(GetWorld(), FName(Address));
	
}

void AClientGameMode::OnError(const PlayFab::FPlayFabCppError& ErrorResult)
{
	UE_LOG(LogTemp, Log, TEXT("Error: %s"), *ErrorResult.ErrorMessage);
}
