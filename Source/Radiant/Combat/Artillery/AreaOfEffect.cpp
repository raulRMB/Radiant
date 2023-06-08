// Copyright Radiant Studios


#include "Combat/Artillery/AreaOfEffect.h"

#include "Character/Hero.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAreaOfEffect::AAreaOfEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	HitBox = CreateDefaultSubobject<USphereComponent>(FName("Mesh"));
	SetRootComponent(HitBox);

	TimerDisplay = CreateDefaultSubobject<UStaticMeshComponent>(FName("TimerDisplay"));
	TimerDisplay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TimerDisplay->SetupAttachment(HitBox);
}

void AAreaOfEffect::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AAreaOfEffect::ApplyGameplayEffects()
{
	if(HasAuthority())
	{
		for(auto Hero : EffectTargets)
		{
			if(URTAbilitySystemComponent* AbilitySystemComponent = Hero->GetAbilitySystemComponent())
			{
				for(auto GameplayEffect : GameplayEffects)
				{
					FGameplayEffectContextHandle EffectContext = Avatar->GetAbilitySystemComponent()->MakeEffectContext();
					FGameplayEffectSpecHandle NewHandle = Avatar->GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
					if(NewHandle.IsValid())
					{
						Avatar->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
					}
				}
			}
		}
	}
}

void AAreaOfEffect::ApplyInstantEffects()
{
	TArray<UPrimitiveComponent*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	UKismetSystemLibrary::SphereOverlapComponents(this, HitBox->GetComponentLocation(), HitBox->GetScaledSphereRadius(), ObjectTypes, UCapsuleComponent::StaticClass(), TArray<AActor*>(), OverlappingActors);
	for(auto Component : OverlappingActors)
	{
		auto Actor = Component->GetAttachmentRootActor();
		if(ShouldHit(Actor))
		{
			if(AHero* Hero = Cast<AHero>(Actor))
			{
				EffectTargets.AddUnique(Hero);
			}
		}
	}

	if(Avatar)
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Location = GetActorLocation();		
		Avatar->GetAbilitySystemComponent()->ExecuteGameplayCue(CueTag, CueParameters);
	}
	
	ApplyGameplayEffects();
	Destroy();
}

// Called when the game starts or when spawned
void AAreaOfEffect::BeginPlay()
{
	Super::BeginPlay();

	if(TimerDisplay)
	{
		TimerDisplay->SetRelativeScale3D(FVector::ZeroVector);
	}
	
	if(LifeSpan == 0.f)
	{
		ApplyInstantEffects();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAreaOfEffect::ApplyInstantEffects, LifeSpan, false);
	}
}

// Called every frame
void AAreaOfEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(TimerDisplay)
	{
		TimerDisplay->SetRelativeScale3D(FVector(1.f, 1.f, 1.f) * (GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandle) / LifeSpan) * 1.3f);
	}
}

