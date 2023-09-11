// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/GridPiece.h"
#include "NotificationActor.generated.h"

enum class ETeamId : uint8;

UCLASS()
class RADIANT_API ANotificationActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Box;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Root;

	ETeamId TeamId;
	FGridPiece GridPiece;

public:
	ANotificationActor();

protected:
	virtual void BeginPlay() override;

public:
	void SetTeamId(ETeamId NewTeamId);
	void SetGridPiece(const FGridPiece& NewGridPiece);
	ETeamId GetTeamId() const;
	FGridPiece GetGridPiece() const;
};
