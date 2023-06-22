// Copyright Radiant Studios


#include "Player/RTPlayerState.h"

#include "Player/Avatar.h"
#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

void ARTPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTPlayerState, TargetId);
	DOREPLIFETIME(ARTPlayerState, TeamId);
	DOREPLIFETIME(ARTPlayerState, Username);
}

ARTPlayerState::ARTPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);	

	AttributeSetBase = CreateDefaultSubobject<URTHeroAttributeSetBase>(TEXT("AttributeSetBase"));

	NetUpdateFrequency = 100.f;

	AttributeSetBase->InitMaxHealth(100.f);
	AttributeSetBase->InitHealth(AttributeSetBase->GetMaxHealth());
	AttributeSetBase->InitMaxMana(150.f);
	AttributeSetBase->InitMana(AttributeSetBase->GetMaxMana());
	AttributeSetBase->InitMovementSpeed(600.f);
	AttributeSetBase->InitDamage(15.f);
	AttributeSetBase->InitMaxRespawnTime(10.f);
	AttributeSetBase->InitCurrentRespawnTime(AttributeSetBase->GetMaxRespawnTime());
	AttributeSetBase->InitXP(0.f);
	AttributeSetBase->InitMaxXP(100.f);
	AttributeSetBase->InitLevel(1.f);
}

void ARTPlayerState::SetPlayerStats()
{
	AttributeSetBase->SetHealth(AttributeSetBase->GetMaxHealth());
	AttributeSetBase->SetMana(AttributeSetBase->GetMaxMana());
	AttributeSetBase->SetMovementSpeed(600.f);
	AttributeSetBase->SetCurrentRespawnTime(AttributeSetBase->GetMaxRespawnTime());
}

void ARTPlayerState::OnRep_UsernameChanged()
{
	GetPawn<AAvatar>()->SetOverheadBarText(Username);
}

void ARTPlayerState::S_SetTargetId_Implementation(const int NewTargetId)
{
	TargetId = NewTargetId;
}

UAbilitySystemComponent* ARTPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;	
}

URTHeroAttributeSetBase* ARTPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

FString ARTPlayerState::GetUsername()
{
	return Username;
}

void ARTPlayerState::SetUsername_Implementation(const FString& String)
{
	Username = String;
}
