// Copyright Radiant Studios

#include "SocketActor.h"
#include "SocketIOClientComponent.h"
#include "SIOJsonValue.h"
#include "Util.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Kismet/GameplayStatics.h"

void ASocketActor::OnConnected(FString SocketId, FString SessionId, bool bIsReconnection)
{
	RTPRINT("Connected to socket");
}

void ASocketActor::OnDisconnected(TEnumAsByte<ESIOConnectionCloseReason> Reason)
{
	RTPRINT("Disconnected from socket");
}

ASocketActor::ASocketActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SocketIOClientComponent = CreateDefaultSubobject<USocketIOClientComponent>(TEXT("SocketIOClientComponent"));
}

void ASocketActor::JoinQueue(FString Queue)
{
	USIOJsonObject* Json = USIOJsonObject::ConstructJsonObject(GetWorld());
	Json->SetStringField("queue", Queue);
	USIOJsonValue* JsonValue = USIOJsonValue::ConstructJsonValueObject(Json, GetWorld());
	SocketIOClientComponent->Emit(FString("joinQueue"), JsonValue);
}

TFunction<void(const FString&, const TSharedPtr<FJsonValue>&)> ASocketActor::OnMatchFound()
{
	auto Callback = [&](const FString& Event, const TSharedPtr<FJsonValue>& Message)
	{
		FString Port = Message->AsObject()->GetStringField("port");
		FString IP = Message->AsObject()->GetStringField("ip");
		FName Address = FName(IP + ":" + Port);
		UGameplayStatics::OpenLevel(GetWorld(), FName(Address));
	};
	return Callback;
}

void ASocketActor::BeginPlay()
{
	Super::BeginPlay();
	SocketIOClientComponent->OnConnected.AddDynamic(this, &ASocketActor::OnConnected);
	SocketIOClientComponent->OnDisconnected.AddDynamic(this, &ASocketActor::OnDisconnected);
	SocketIOClientComponent->Connect(SocketIOClientComponent->URLParams.AddressAndPort);
	SocketIOClientComponent->OnNativeEvent(FString("matchFound"), OnMatchFound());
	//
	// SocketIOClientComponent->OnNativeEvent(FString("matchFound"), [&](const FString& Event, const TSharedPtr<FJsonValue>& Message)
	// {
	// 	RTPRINT("Match found");
	// });
}

