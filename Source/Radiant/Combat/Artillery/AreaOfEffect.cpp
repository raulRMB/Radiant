// Copyright Radiant Studios


#include "Combat/Artillery/AreaOfEffect.h"

#include "Character/Hero.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"

// Sets default values
AAreaOfEffect::AAreaOfEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(Mesh);

	TimerDisplay = CreateDefaultSubobject<UStaticMeshComponent>(FName("TimerDisplay"));
	TimerDisplay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TimerDisplay->SetupAttachment(Mesh);
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
	TArray<AActor*> OverlappingActors;
	Mesh->GetOverlappingActors(OverlappingActors, AHero::StaticClass());
	for(auto Actor : OverlappingActors)
	{
		if(AHero* Hero = Cast<AHero>(Actor))
		{
			EffectTargets.AddUnique(Hero);
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
	
	if(Mesh)
	{
		Mesh->OnComponentBeginOverlap.AddDynamic(this, &AAreaOfEffect::OnOverlapBegin);
		Mesh->OnComponentEndOverlap.AddDynamic(this, &AAreaOfEffect::OnOverlapEnd);
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

void AAreaOfEffect::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if(AHero* Hero = Cast<AHero>(OtherActor))
	{
		EffectTargets.AddUnique(Hero);
	}
}

void AAreaOfEffect::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(AHero* Hero = Cast<AHero>(OtherActor))
	{
		EffectTargets.Remove(Hero);
	}
}

