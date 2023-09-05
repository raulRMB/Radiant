// Copyright Radiant Studios


#include "Combat/Artillery/LinearSkillshot.h"

#include "AreaOfEffect.h"
#include "GameplayEffect.h"
#include "Player/Avatar.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RTPlayerState.h"

// Sets default values
ALinearSkillshot::ALinearSkillshot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;


	HitBox = CreateDefaultSubobject<USphereComponent>(FName("HitBox"));
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox->SetSphereRadius(50.f);
	HitBox->SetHiddenInGame(false);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ALinearSkillshot::OnOverlapBegin);
	SetRootComponent(HitBox);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ALinearSkillshot::SetGameplayEffects(TArray<TSubclassOf<UGameplayEffect>> NewGameplayEffects)
{
	GameplayEffects = NewGameplayEffects;
}


void ALinearSkillshot::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Velocity = GetActorForwardVector() * Speed;

	if (HasAuthority())
	{
		if (bUseRange)
		{
			LifeSpan = Range / Speed;
		}
		SetLifeSpan(LifeSpan);
	}

	if(AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(this, AGridManager::StaticClass())))
	{
		GridManager->AddVisibleActor(this);
	}
}

void ALinearSkillshot::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetReplicateMovement(true);
}

// Called every frame
void ALinearSkillshot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALinearSkillshot::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	if (!HasAuthority() || !ShouldHit(OtherActor))
	{
		return;
	}

	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		for (TSubclassOf<UGameplayEffect>& Effect : GameplayEffects)
		{
			if (!AffectedActors.Contains(OtherActor))
			{
				if (ARTCharacter* Projectile_Instigator = Cast<ARTCharacter>(GetInstigator()))
				{
					Projectile_Instigator->GetAbilitySystemComponent()->ApplyGameplayEffectToTarget(
						Effect.GetDefaultObject(), ASCInterface->GetAbilitySystemComponent(), 1.f, ASCInterface->GetAbilitySystemComponent()->MakeEffectContext());
				}
			}
		}
		AffectedActors.AddUnique(OtherActor);
		FGameplayCueParameters CueParameters;
		CueParameters.Location = OtherActor->GetActorLocation();
		if(ARTCharacter* OtherCharacter = Cast<ARTCharacter>(GetInstigator()))
		{
			CueParameters.TargetAttachComponent = OtherCharacter->GetMesh();
		}
		CueParameters.Instigator = this;
		ASCInterface->GetAbilitySystemComponent()->ExecuteGameplayCue(HitCueTag, CueParameters);
	}

	OverlapStart(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	for(TSubclassOf<AArtillery> Artillery : ArtilleryChildren)
	{
		AArtillery* ArtilleryChild = GetWorld()->SpawnActorDeferred<AArtillery>(
			Artillery,
			FTransform(GetActorLocation()),
			GetOwner(),
			GetInstigator(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		if(AAreaOfEffect* AreaOfEffect = Cast<AAreaOfEffect>(ArtilleryChild))
		{
			if(ARTCharacter* Character = Cast<ARTCharacter>(GetInstigator()))
			{
				AreaOfEffect->SetSourceCharacter(Character);
			}
		}

		ArtilleryChild->FinishSpawning(FTransform(GetActorLocation()));
	}

	if (bConsumeOnHit)
	{
		Destroy();
	}
}

void ALinearSkillshot::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
