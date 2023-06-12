// Copyright Radiant Studios


#include "Buildings/Tower.h"

#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/TowerAttributeSet.h"

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

	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UTowerAttributeSet>("AttributeSet");

	AttributeSet->InitMaxHealth(MaxHealth);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	AttributeSet->InitAttackDamage(AttackDamage);
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
	
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

