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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Gem = CreateDefaultSubobject<UStaticMeshComponent>("Gem");
	Gem->SetupAttachment(Mesh);

	AttackRadius = CreateDefaultSubobject<USphereComponent>("AttackRadius");
	AttackRadius->SetupAttachment(Mesh);
	AttackRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
	
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UTowerAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ATower::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UTowerAttributeSet* ATower::GetAttributeSet() const
{
	return AttributeSet;
}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();
	GiveInitialAbilities();
	AttackRadius->OnComponentBeginOverlap.AddDynamic(this,&ATower::BeingOverlap);
	AttackRadius->OnComponentEndOverlap.AddDynamic(this,&ATower::EndOverlap);
	AttributeSet->InitMaxHealth(MaxHealth);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	AttributeSet->InitAttackDamage(AttackDamage);
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	if(Target || OtherTeamMember->GetTeamId() == GetTeamId())
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

void ATower::GiveInitialAbilities()
{
	for(auto Ability : Abilities)
	{
		AbilitySystemComponent->GiveAbility(Ability);
	}
}
