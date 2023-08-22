// Copyright Radiant Studios


#include "MapHitBox.h"
#include "Components/CapsuleComponent.h"
#include "Characters/RTCharacter.h"
#include "Util/Util.h"


// Sets default values
AMapHitBox::AMapHitBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	SetRootComponent(CapsuleComponent);
}

// Called when the game starts or when spawned
void AMapHitBox::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AMapHitBox::OnEndOverlap);
}

void AMapHitBox::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ARTCharacter* Character = Cast<ARTCharacter>(OtherActor))
	{
		FVector ToCenter = Character->GetActorLocation() - GetActorLocation();
		FVector Dir = ToCenter.GetSafeNormal() * (CapsuleComponent->GetScaledCapsuleRadius() + Character->GetCapsuleComponent()->GetScaledCapsuleRadius());
		Character->SetActorLocation(Dir + GetActorLocation());
	}
}

// Called every frame
void AMapHitBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* for(ARTCharacter* Character : Characters)
	{
		if(IsValid(Character))
		{
			FVector CharacterLocation = Character->GetActorLocation();
			FVector Dir = GetActorLocation() - CharacterLocation;
			Dir.Normalize();
			Character->SetActorLocation(CharacterLocation + Dir * MoveSpeed *  DeltaTime);
		}
		else
		{
			Characters.Remove(Character);
		}
	} */
}

