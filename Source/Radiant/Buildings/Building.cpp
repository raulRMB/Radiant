#include "Building.h"

#include "Components/CapsuleComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/TowerAttributeSet.h"

ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UTowerAttributeSet>("AttributeSet");
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	SetRootComponent(CapsuleComponent);
}


void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	GiveInitialAbilities();
	AttributeSet->InitMaxHealth(MaxHealth);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
}


void ABuilding::GiveInitialAbilities()
{
	for(auto Ability : Abilities)
	{
		AbilitySystemComponent->GiveAbility(Ability);
	}
}

UAbilitySystemComponent* ABuilding::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UTowerAttributeSet* ABuilding::GetAttributeSet() const
{
	return AttributeSet;
}


