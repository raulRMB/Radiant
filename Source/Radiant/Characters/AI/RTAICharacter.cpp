// Copyright Radiant Studios


#include "Characters/AI/RTAICharacter.h"

#include "Combat/PickUp.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/NPCAttributeSet.h"

ARTAICharacter::ARTAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>(FName("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UNPCAttributeSet>(FName("AttributeSet"));
	
	AttributeSet->InitMaxHealth(10.f);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	AttributeSet->InitMovementSpeed(200.f);
	AttributeSet->InitDamage(15.f);
	AttributeSet->InitLevel(1.f);
}

UAbilitySystemComponent* ARTAICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UNPCAttributeSet* ARTAICharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ARTAICharacter::Die()
{
	OnUnitDied.Broadcast();
	GetWorld()->SpawnActor<APickUp>(PickUpClass, GetActorTransform());
	Destroy();
}

// Called when the game starts or when spawned
void ARTAICharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}


