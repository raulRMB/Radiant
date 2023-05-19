// Copyright Radiant Studios


#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "Character/Hero.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/RTPlayerState.h"

void URTHeroAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if(Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
	{
		UpdateMovementSpeed();
	}
}

void URTHeroAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, MaxMovementSpeed, COND_None, REPNOTIFY_Always);
}

void URTHeroAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, Health, OldHealth);
}

void URTHeroAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, MaxHealth, OldMaxHealth);
}

void URTHeroAttributeSetBase::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, MovementSpeed, OldMovementSpeed);
	UpdateMovementSpeed();
}

void URTHeroAttributeSetBase::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, MaxMovementSpeed, OldMaxMovementSpeed);
}

void URTHeroAttributeSetBase::UpdateMovementSpeed()
{
	if(auto PS = Cast<ARTPlayerState>(GetOwningActor()))
	{
		if(auto Hero = Cast<AHero>(PS->GetPawn()))
		{
			Hero->GetCharacterMovement()->MaxWalkSpeed = GetMovementSpeed();
		}
	}
}
