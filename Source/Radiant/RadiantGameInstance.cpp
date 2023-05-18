// Copyright Radiant Studios


#include "RadiantGameInstance.h"
#include "PlayFabGSDK.h"
#include "GSDKUtils.h"
#include "AbilitySystemGlobals.h"

void URadiantGameInstance::Init()
{
	Super::Init();

	if(IsDedicatedServerInstance())
	{
		FOnGSDKShutdown_Dyn OnGsdkShutdown;
		OnGsdkShutdown.BindDynamic(this, &URadiantGameInstance::OnGSDKShutdown);
		FOnGSDKHealthCheck_Dyn OnGsdkHealthCheck;
		OnGsdkHealthCheck.BindDynamic(this, &URadiantGameInstance::OnGSDKHealthCheck);
		FOnGSDKServerActive_Dyn OnGSDKServerActive;
		OnGSDKServerActive.BindDynamic(this, &URadiantGameInstance::OnGSDKServerActive);
		FOnGSDKReadyForPlayers_Dyn OnGSDKReadyForPlayers;
		OnGSDKReadyForPlayers.BindDynamic(this, &URadiantGameInstance::OnGSDKReadyForPlayers);

		UGSDKUtils::RegisterGSDKShutdownDelegate(OnGsdkShutdown);
		UGSDKUtils::RegisterGSDKHealthCheckDelegate(OnGsdkHealthCheck);
		UGSDKUtils::RegisterGSDKServerActiveDelegate(OnGSDKServerActive);
		UGSDKUtils::RegisterGSDKReadyForPlayers(OnGSDKReadyForPlayers);
	}
#if UE_SERVER
	UGSDKUtils::SetDefaultServerHostPort();
#endif
	
	UAbilitySystemGlobals::Get().InitGlobalData();
}

void URadiantGameInstance::OnStart()
{
	Super::OnStart();
	
	UE_LOG(LogTemp, Warning, TEXT("Reached onStart!"));
	UGSDKUtils::ReadyForPlayers();
}

void URadiantGameInstance::OnGSDKShutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("Shutdown!"));
	FPlatformMisc::RequestExit(false);
}

bool URadiantGameInstance::OnGSDKHealthCheck()
{
	UE_LOG(LogTemp, Warning, TEXT("Healthy!"));
	return true;
}

void URadiantGameInstance::OnGSDKServerActive()
{
	/**
	 * Server is transitioning to an active state.
	 * Optional: Add in the implementation any code that is needed for the game server when
	 * this transition occurs.
	 */
	UE_LOG(LogTemp, Warning, TEXT("Active!"));
}

void URadiantGameInstance::OnGSDKReadyForPlayers()
{
	/**
	 * Server is transitioning to a StandBy state. Game initialization is complete and the game is ready
	 * to accept players.
	 * Optional: Add in the implementation any code that is needed for the game server before
	 * initialization completes.
	 */
	UE_LOG(LogTemp, Warning, TEXT("Finished Initialization - Moving to StandBy!"));
}
