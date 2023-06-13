// Copyright Radiant Studios


#include "Objectives/CaptureArea.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Interfaces/TeamMember.h"

// Sets default values
ACaptureArea::ACaptureArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<UBoxComponent>(FName("HitBox"));
	RootComponent = HitBox;
}

// Called when the game starts or when spawned
void ACaptureArea::BeginPlay()
{
	Super::BeginPlay();

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ACaptureArea::BeingOverlap);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &ACaptureArea::EndOverlap);
}

void ACaptureArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ACaptureArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACaptureArea::BeingOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		ITeamMember* TeamMember = Cast<ITeamMember>(OtherActor);
		if(TeamMember)
		{
			OnUpdatePlayersInArea.Broadcast(TeamMember->GetTeamId(), true);
		}
	}
}

void ACaptureArea::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(HasAuthority())
	{
		ITeamMember* TeamMember = Cast<ITeamMember>(OtherActor);
		if(TeamMember)
		{
			OnUpdatePlayersInArea.Broadcast(TeamMember->GetTeamId(), false);
		}
	}
}

