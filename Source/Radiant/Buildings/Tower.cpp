// Copyright Radiant Studios


#include "Buildings/Tower.h"

#include "Components/SphereComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/BuildingAttributeSet.h"
#include "Player/Avatar.h"
#include "Util/Util.h"

// Sets default values
ATower::ATower()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	Gem = CreateDefaultSubobject<UStaticMeshComponent>("Gem");
	Gem->SetupAttachment(RootComponent);

	AttackRadius = CreateDefaultSubobject<USphereComponent>("AttackRadius");
	AttackRadius->SetupAttachment(RootComponent);
	AttackRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();
	AttackRadius->OnComponentBeginOverlap.AddDynamic(this,&ATower::BeingOverlap);
	AttackRadius->OnComponentEndOverlap.AddDynamic(this,&ATower::EndOverlap);
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<AActor*> OverlappingActors;
	Gem->AddLocalRotation(FRotator(0.f, 90.f * DeltaTime, 0.f));
	if(IsValid(Target))
	{
		FGameplayEventData EventData;
		EventData.Target = Target;
		EventData.Instigator = this;
		GetAbilitySystemComponent()->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("Trigger.Tower.Attack"), &EventData);
	}
	else
	{
		AttackRadius->GetOverlappingActors(OverlappingActors);
		if(OverlappingActors.Num() > 0)
		{
			for(AActor* Actor : OverlappingActors)
			{
				auto TeamMember = Cast<ITeamMember>(Actor);
				if(TeamMember && TeamMember->GetTeamId() != GetTeamId())
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

FTransform ATower::GetGemTransform() const
{
	return FTransform(Gem->GetComponentLocation() + GemOffset);
}

TObjectPtr<AActor> ATower::GetTarget() const
{
	return Target;
}

void ATower::BeingOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ITeamMember* OtherTeamMember = Cast<ITeamMember>(OtherActor);
	if(Target || !OtherTeamMember || OtherTeamMember->GetTeamId() == GetTeamId())
		return;
	
	if(HasAuthority())
	{
		Target = OtherActor;
	}
}

void ATower::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if(HasAuthority())
	{
		if(OtherActor == Target)
		{
			Target = nullptr;
		}
	}
}
