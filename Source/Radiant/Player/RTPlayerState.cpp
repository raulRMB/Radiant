// Copyright Radiant Studios


#include "Player/RTPlayerState.h"

#include "Data/AbilityDataAsset.h"
#include "Player/Avatar.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

void ARTPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTPlayerState, Target);
	DOREPLIFETIME(ARTPlayerState, TeamId);
	DOREPLIFETIME(ARTPlayerState, Username);
	DOREPLIFETIME(ARTPlayerState, OwnedAbilities);
	DOREPLIFETIME(ARTPlayerState, AbilityTriggers);
}

ARTPlayerState::ARTPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);	

	AttributeSet = CreateDefaultSubobject<URTAvatarAttributeSet>(TEXT("AttributeSet"));

	NetUpdateFrequency = 100.f;

	AttributeSet->InitMaxHealth(100.f);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	AttributeSet->InitMaxMana(150.f);
	AttributeSet->InitMana(AttributeSet->GetMaxMana());
	AttributeSet->InitMovementSpeed(600.f);
	AttributeSet->InitDamage(15.f);
	AttributeSet->InitMaxRespawnTime(10.f);
	AttributeSet->InitCurrentRespawnTime(AttributeSet->GetMaxRespawnTime());
	AttributeSet->InitXP(0.f);
	AttributeSet->InitMaxXP(100.f);
	AttributeSet->InitLevel(1.f);
	AttributeSet->InitRadianite(0.f);
}

void ARTPlayerState::SetPlayerStats()
{
	AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	AttributeSet->SetMana(AttributeSet->GetMaxMana());
	AttributeSet->SetMovementSpeed(600.f);
	AttributeSet->SetCurrentRespawnTime(AttributeSet->GetMaxRespawnTime());
}

void ARTPlayerState::OnRep_UsernameChanged()
{
	GetPawn<AAvatar>()->SetOverheadBarText(Username);
}

void ARTPlayerState::S_SetTarget_Implementation(AActor* NewTargetId)
{
	Target = NewTargetId;
}

UAbilitySystemComponent* ARTPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;	
}

URTAvatarAttributeSet* ARTPlayerState::GetAttributeSetBase() const
{
	return AttributeSet;
}

FString ARTPlayerState::GetUsername()
{
	return Username;
}

void ARTPlayerState::S_BuyAbility_Implementation(UAbilityDataAsset* AbilityDataAsset)
{
	if(AttributeSet->GetRadianite() < AbilityDataAsset->Price)
	{
		return;
	}
	if(OwnedAbilities.Contains(AbilityDataAsset))
	{
		return;
	}
	OwnedAbilities.Add(AbilityDataAsset);
	AbilityTriggers.AddTag(AbilityDataAsset->Ability.GetDefaultObject()->GetTriggerTag());
	AbilitySystemComponent->GiveAbility(AbilityDataAsset->Ability.GetDefaultObject());
	AttributeSet->SetRadianite(AttributeSet->GetRadianite() - AbilityDataAsset->Price);
}

TArray<class UAbilityDataAsset*> ARTPlayerState::GetOwnedAbilities() const
{
	return OwnedAbilities;
}

FGameplayTagContainer ARTPlayerState::GetAbilityTriggers() const
{
	return AbilityTriggers;
}

void ARTPlayerState::OnRepAbilityTriggers()
{
	GetPawn<AAvatar>()->SetHUDIcons();
}

void ARTPlayerState::SetUsername_Implementation(const FString& String)
{
	Username = String;
}
