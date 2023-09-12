// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "SocketIONative.h"
#include "GameFramework/Actor.h"
#include "SocketActor.generated.h"

UCLASS()
class RADIANT_API ASocketActor : public AActor
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere)
	class USocketIOClientComponent* SocketIOClientComponent;

public:
	UFUNCTION()
	void OnConnected(FString SocketId, FString SessionId, bool bIsReconnection);
	UFUNCTION()
	void OnDisconnected(TEnumAsByte<ESIOConnectionCloseReason> Reason);
	ASocketActor();
	void JoinQueue(FString Queue);
	TFunction<void(const FString&, const TSharedPtr<FJsonValue>&)> OnMatchFound();
protected:
	virtual void BeginPlay() override;
};
