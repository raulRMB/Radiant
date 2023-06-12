// Copyright Radiant Studios


#include "Combat/PickUp.h"

#include "Components/BoxComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Player/Avatar.h"

APickUp::APickUp()
{
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void APickUp::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnBeginOverlap);
}


void APickUp::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AAvatar* Avatar = Cast<AAvatar>(OtherActor))
	{
		Avatar->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(GameplayEffect.GetDefaultObject(), 1, Avatar->GetAbilitySystemComponent()->MakeEffectContext());
		OnPickedUp.Execute();
		Destroy();
	}
}

void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

