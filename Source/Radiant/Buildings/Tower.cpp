// Copyright Radiant Studios


#include "Buildings/Tower.h"

#include "Components/SphereComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/TowerAttributeSet.h"
#include "Player/Avatar.h"
#include "Util/Util.h"

// Sets default values
ATower::ATower()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Gem = CreateDefaultSubobject<UStaticMeshComponent>("Gem");
	Gem->SetupAttachment(Mesh);

	AttackRadius = CreateDefaultSubobject<USphereComponent>("AttackRadius");
	AttackRadius->SetupAttachment(Mesh);
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
	AttributeSet->InitAttackDamage(AttackDamage);
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Gem->AddLocalRotation(FRotator(0.f, 90.f * DeltaTime, 0.f));
	if(Target)
	{
		FGameplayEventData EventData;
		EventData.Target = Target;
		EventData.Instigator = this;
		GetAbilitySystemComponent()->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("Event.Tower.Attack"), &EventData);
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
		if(AAvatar* Avatar = Cast<AAvatar>(OtherActor))
		{
			Target = Avatar;
		}
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
