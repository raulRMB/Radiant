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
	AttackRadius->OnComponentBeginOverlap.AddDynamic(this,&ATower::BeingOverlap);
	AttributeSet->InitMaxHealth(MaxHealth);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	AttributeSet->InitAttackDamage(AttackDamage);
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATower::BeingOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		if(AAvatar* Avatar = Cast<AAvatar>(OtherActor))
		{
			RTLOG("Tower Attack")			
			GetAbilitySystemComponent()->ApplyGameplayEffectToTarget(AttackEffect.GetDefaultObject(),Avatar->GetAbilitySystemComponent());
		}
	}
}