// Copyright Radiant Studios


#include "Combat/Artillery/LinearSkillshot.h"

#include "GameplayEffect.h"
#include "Player/Avatar.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
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

	if(HasAuthority())
	{
		if(bUseRange)
			LifeSpan = Range / Speed;
		SetLifeSpan(LifeSpan);
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
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority() || !ShouldHit(OtherActor))
		return;
	
	if(auto Character = Cast<ARTCharacter>(OtherActor))
	{
		for(auto Effect : GameplayEffects)
		{
			if(!AffectedActors.Contains(OtherActor))
			{
				UGameplayEffect* NewEffect = NewObject<UGameplayEffect>(GetTransientPackage(), Effect);
				if(ARTCharacter* Projectile_Instigator = Cast<ARTCharacter>(GetInstigator()))
				{
					Projectile_Instigator->GetAbilitySystemComponent()->ApplyGameplayEffectToTarget(NewEffect, Character->GetAbilitySystemComponent(),1.f);
				}
				AffectedActors.AddUnique(OtherActor);
			}
		}
		FGameplayCueParameters CueParameters;
		CueParameters.Location = Character->GetActorLocation();
		CueParameters.TargetAttachComponent = Character->GetMesh();
		CueParameters.Instigator = this;
		Character->GetAbilitySystemComponent()->ExecuteGameplayCue(HitCueTag, CueParameters);
	}

	OverlapStart(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if(bConsumeOnHit)
		Destroy();
}

void ALinearSkillshot::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

