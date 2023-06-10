// Copyright Radiant Studios


#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "Character/Avatar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Modes/RTGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void URTHeroAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if(GetHealth() <= 0)
		{
			AAvatar* Hero = Cast<AAvatar>(GetActorInfo()->AvatarActor);
			Hero->M_SetInfoBarVisibility(false);

			if(Hero->HasAuthority())
			{
				Cast<ARTGameState>(GetWorld()->GetGameState())->OnHeroDeath(Hero);
			}
			
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.State.Death")));
			GetOwningAbilitySystemComponentChecked()->TryActivateAbilitiesByTag(TagContainer);
		}
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
	else if(Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
	{
		UpdateMovementSpeed();
	}
	else if(Data.EvaluatedData.Attribute == GetCurrentRespawnTimeAttribute())
	{
		if(GetCurrentRespawnTime() == 0.0f)
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Respawn")));
			GetOwningAbilitySystemComponentChecked()->TryActivateAbilitiesByTag(TagContainer);
			SetCurrentRespawnTime(10.f);
		}
	}
}

void URTHeroAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, MaxMovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, CurrentRespawnTime, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTHeroAttributeSetBase, MaxRespawnTime, COND_None, REPNOTIFY_Always);
}

void URTHeroAttributeSetBase::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if(Attribute == GetMovementSpeedAttribute())
	{
		UpdateMovementSpeed();
	}
}

void URTHeroAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, Health, OldHealth);
}

void URTHeroAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, MaxHealth, OldMaxHealth);
}

void URTHeroAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, Mana, OldMana);
}

void URTHeroAttributeSetBase::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, MaxMana, OldMaxMana);
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

void URTHeroAttributeSetBase::OnRep_AttackDamage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, Damage, OldDamage);
}

void URTHeroAttributeSetBase::OnRep_CurrentRespawnTime(const FGameplayAttributeData& OldCurrentRespawnTime)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, CurrentRespawnTime, OldCurrentRespawnTime);
}

void URTHeroAttributeSetBase::OnRep_MaxRespawnTime(const FGameplayAttributeData& OldMaxRespawnTime)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTHeroAttributeSetBase, MaxRespawnTime, OldMaxRespawnTime);
}

void URTHeroAttributeSetBase::UpdateMovementSpeed()
{
	if(auto PS = Cast<ARTPlayerState>(GetOwningActor()))
	{
		if(auto Hero = Cast<AAvatar>(PS->GetPawn()))
		{
			Hero->GetCharacterMovement()->MaxWalkSpeed = GetMovementSpeed();
		}
	}
}
