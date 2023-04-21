// Copyright Radiant Studios


#include "ClientGameMode.h"

#include "PlayFab.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabRuntimeSettings.h"
#include "WidgetManager.h"
#include "Blueprint/UserWidget.h"
#include "Core/PlayFabClientAPI.h"
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

void AClientGameMode::OnError(const PlayFab::FPlayFabCppError& ErrorResult)
{
	UE_LOG(LogTemp, Log, TEXT("Error: %s"), *ErrorResult.ErrorMessage);
}
