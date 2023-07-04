// Copyright Radiant Studios


#include "GAS/AttributeSets/RTBaseAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Util/Util.h"
#include "Util/Interfaces/Killable.h"

void URTBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if(GetHealth() <= 0)
		{
			if(IKillable* Killable = Cast<IKillable>(GetOwningActor()))
			{
				Killable->Die();
			}
		}
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if(Data.EvaluatedData.Attribute == GetXPAttribute())
	{
		if(GetXP() >= GetMaxXP())
		{
			SetLevel(GetLevel() + 1);
			SetXP(GetXP() - GetMaxXP());
			SetMaxXP(GetMaxXP() * 1.3f);
			
			auto currHealth = GetHealth() / GetMaxHealth();
			SetMaxHealth(GetMaxHealth() * 1.20f);
			SetHealth(GetMaxHealth() * currHealth);
			
			SetDamage(GetDamage() * 1.20f);
		}
	}
}

void URTBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if(Attribute == GetMovementSpeedAttribute())
	{
		UpdateMovementSpeed();
	}
}

void URTBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, MaxMovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, XP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, MaxXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTBaseAttributeSet, MaxLevel, COND_None, REPNOTIFY_Always);
}

void URTBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, Health, OldHealth);
}

void URTBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, MaxHealth, OldMaxHealth);
}

void URTBaseAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, MovementSpeed, OldMovementSpeed);
	UpdateMovementSpeed();
}

void URTBaseAttributeSet::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, MaxMovementSpeed, OldMaxMovementSpeed);
}

void URTBaseAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, Damage, OldDamage);
}

void URTBaseAttributeSet::OnRep_XP(const FGameplayAttributeData& OldXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, XP, OldXP);
}

void URTBaseAttributeSet::OnRep_MaxXP(const FGameplayAttributeData& OldMaxXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, MaxXP, OldMaxXP);
}

void URTBaseAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, Level, OldLevel);
}

void URTBaseAttributeSet::OnRep_MaxLevel(const FGameplayAttributeData& OldMaxLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTBaseAttributeSet, MaxLevel, OldMaxLevel);
}

void URTBaseAttributeSet::UpdateMovementSpeed()
{
	if(ACharacter* Character = Cast<ACharacter>(GetActorInfo()->AvatarActor))
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = GetMovementSpeed();
	}
}