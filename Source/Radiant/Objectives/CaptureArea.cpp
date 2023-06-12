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

	PointsText = CreateDefaultSubobject<UTextRenderComponent>(FName("PointsText"));
	PointsText->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACaptureArea::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACaptureArea::CheckCapture, 3.0f, true);

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ACaptureArea::BeingOverlap);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &ACaptureArea::EndOverlap);
}

void ACaptureArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

// Called every frame
void ACaptureArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ACaptureArea::BeingOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ITeamMember* TeamMember = Cast<ITeamMember>(OtherActor);
	if(TeamMember)
	{
		TeamMember->GetTeamId() == 1 ? TeamOneCount++ : TeamZeroCount++;
	}
}

void ACaptureArea::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ITeamMember* TeamMember = Cast<ITeamMember>(OtherActor);
	if(TeamMember)
	{
		TeamMember->GetTeamId() == 1 ? TeamOneCount-- : TeamZeroCount--;
	}
}

void ACaptureArea::CheckCapture()
{
	if(TeamOneCount == 0)
	{		
		TeamPoints += TeamZeroCount;
	}
	else if(TeamZeroCount == 0)
	{
		TeamPoints -= TeamOneCount;
	}

	if(!HasAuthority())
	{
		PointsText->SetText(FText::FromString(FString::Printf(TEXT("TeamPoints %s"), *FString::FromInt(TeamPoints))));
	}
}

