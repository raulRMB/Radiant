// Copyright Radiant Studios


#include "HeatSeeking.h"

#include "Character/Hero.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHeatSeeking::AHeatSeeking()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
    AActor::SetReplicateMovement(true);

	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AHeatSeeking::OnOvelapBegin);
	SetRootComponent(HitBox);
}

void AHeatSeeking::MulticastSetTarget_Implementation(AActor* NewTarget)
{
	Target = NewTarget;
}

// Called when the game starts or when spawned
void AHeatSeeking::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
		MulticastSetTarget(Target);
}

void AHeatSeeking::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetReplicateMovement(true);
}

void AHeatSeeking::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHeatSeeking, Target);
}

void AHeatSeeking::OnOvelapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHero* Hero = Cast<AHero>(GetInstigator());
	AHero* OtherHero = Cast<AHero>(OtherActor);

	if(HasAuthority())
	{
		if(ShouldHit(OtherActor))
		{
			for(TSubclassOf<UGameplayEffect> GameplayEffect : GameplayEffects)
			{
				UGameplayEffect* Effect = GameplayEffect.GetDefaultObject();
				Hero->GetAbilitySystemComponent()->ApplyGameplayEffectToTarget(Effect, OtherHero->GetAbilitySystemComponent());
			}
			Destroy();
		}
	}
}

// Called every frame
void AHeatSeeking::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Target)
	{
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		SetActorLocation(GetActorLocation() + (Direction.GetSafeNormal() * Speed * DeltaTime));
	}
}

