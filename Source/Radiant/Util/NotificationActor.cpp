// Copyright Radiant Studios


#include "NotificationActor.h"

#include "Util.h"
#include "Components/BoxComponent.h"

ANotificationActor::ANotificationActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetCollisionObjectType(ECC_Pawn);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Vision, ECollisionResponse::ECR_Overlap);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Box->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	Box->SetupAttachment(RootComponent);
}

void ANotificationActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANotificationActor::SetTeamId(ETeamId NewTeamId)
{
	TeamId = NewTeamId;
}

void ANotificationActor::SetGridPiece(const FGridPiece& NewGridPiece)
{
	GridPiece = NewGridPiece;
}

ETeamId ANotificationActor::GetTeamId() const
{
	return TeamId;
}

FGridPiece ANotificationActor::GetGridPiece() const
{
	return GridPiece;
}

