// Copyright Radiant Studios


#include "Tower.h"

#include "Components/SphereComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Player/Avatar.h"
#include "Util/Util.h"

ATower::ATower()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	AttackRadius = CreateDefaultSubobject<USphereComponent>("AttackRadius");
	AttackRadius->SetupAttachment(RootComponent);
	AttackRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackRadius->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
}

void ATower::BeginPlay()
{
	Super::BeginPlay();
	AttackRadius->OnComponentBeginOverlap.AddDynamic(this, &ATower::BeingOverlap);
	AttackRadius->OnComponentEndOverlap.AddDynamic(this, &ATower::EndOverlap);

	GetAttributeSet()->SetAttackDamage(AttackDamage);
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		if (IsValid(Target))
		{
			FGameplayEventData EventData;
			EventData.Target = Target;
			EventData.Instigator = this;
			GetAbilitySystemComponent()->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("Trigger.Tower.Attack"),
															 &EventData);
		}
		else
		{
			TArray<AActor*> OverlappingActors;
			AttackRadius->GetOverlappingActors(OverlappingActors);
			if (OverlappingActors.Num() > 0)
			{
				for (AActor* Actor : OverlappingActors)
				{
					auto TeamMember = Cast<ITeamMember>(Actor);
					if (TeamMember && TeamMember->GetTeamId() != GetTeamId())
					{
						Target = Actor;
						break;
					}
				}
			}
			else
			{
				Target = nullptr;
			}
		}
	}
}

TObjectPtr<AActor> ATower::GetTarget() const
{
	return Target;
}

void ATower::BeingOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                          const FHitResult& SweepResult)
{
	ITeamMember* OtherTeamMember = Cast<ITeamMember>(OtherActor);
	ITeamMember* CurrentTarget = Cast<ITeamMember>(Target);
	
	if (!HasAuthority() ||
		!OtherTeamMember ||
		OtherTeamMember->GetTeamId() == GetTeamId())
	{
		return;
	}
	
	const ETeamId OppositeId = OtherTeamMember->GetTeamId() == ETeamId::Blue ? ETeamId::Red : ETeamId::Blue;
	if(IsValid(Target) && OppositeId == GetTeamId() && CurrentTarget->GetTeamId() == ETeamId::Neutral)
	{
		Target = OtherActor;
	}
	else if(!Target)
	{
		Target = OtherActor;
	}
}

void ATower::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		if (OtherActor == Target)
		{
			Target = nullptr;
		}
	}
}
