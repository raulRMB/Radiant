// Copyright Radiant Studios


#include "EndBox.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEndBox::AEndBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box"));
	check(Box);
	SetRootComponent(Box);
	Box->OnComponentBeginOverlap.AddDynamic(this, &AEndBox::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AEndBox::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEndBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEndBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
	if(HasAuthority())
	{
		TArray<AActor*> ConnectedPlayers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), ConnectedPlayers);
		for (AActor* Player : ConnectedPlayers)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(Player))
			{
				PlayerController->ClientTravel("/Game/Maps/LoginMenu", ETravelType::TRAVEL_Absolute);
			}
		}
		
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
	}
}

