// Copyright Radiant Studios


#include "Player/RTPlayerState.h"

#include "Character/Hero.h"
#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"

ARTPlayerState::ARTPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);	

	AttributeSetBase = CreateDefaultSubobject<URTHeroAttributeSetBase>(TEXT("AttributeSetBase"));

	NetUpdateFrequency = 30.f;

	AttributeSetBase->InitMaxHealth(100.f);
	AttributeSetBase->InitHealth(AttributeSetBase->GetMaxHealth());
	AttributeSetBase->InitMaxMana(100.f);
	AttributeSetBase->InitMana(AttributeSetBase->GetMaxMana());
	AttributeSetBase->InitMovementSpeed(600.f);
	AttributeSetBase->InitDamage(15.f);
}

UAbilitySystemComponent* ARTPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;	
}

URTHeroAttributeSetBase* ARTPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}
