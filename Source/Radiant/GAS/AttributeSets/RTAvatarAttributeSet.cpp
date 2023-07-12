// Copyright Radiant Studios


#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Player/Avatar.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Modes/Base/RTGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RTPlayerState.h"

void URTAvatarAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
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
			Hero->S_StopMovement();
			Hero->SetIsDead(true);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
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
	else if(Data.EvaluatedData.Attribute == GetXPAttribute())
	{
		if(GetXP() >= GetMaxXP())
		{
			auto currMana = GetMana() / GetMaxMana();
			SetMaxMana(GetMaxMana() * 1.20f);
			SetMana(GetMaxMana() * currMana);
			
			AAvatar* Hero = Cast<AAvatar>(GetActorInfo()->AvatarActor);
			Hero->LevelUp(GetLevel());
		}
	}
	
	Super::PostGameplayEffectExecute(Data);
}

void URTAvatarAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(URTAvatarAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTAvatarAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTAvatarAttributeSet, CurrentRespawnTime, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTAvatarAttributeSet, MaxRespawnTime, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URTAvatarAttributeSet, Radianite, COND_None, REPNOTIFY_Always);
}


void URTAvatarAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTAvatarAttributeSet, Mana, OldMana);
}

void URTAvatarAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTAvatarAttributeSet, MaxMana, OldMaxMana);
}

void URTAvatarAttributeSet::OnRep_CurrentRespawnTime(const FGameplayAttributeData& OldCurrentRespawnTime)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTAvatarAttributeSet, CurrentRespawnTime, OldCurrentRespawnTime);
}

void URTAvatarAttributeSet::OnRep_MaxRespawnTime(const FGameplayAttributeData& OldMaxRespawnTime)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTAvatarAttributeSet, MaxRespawnTime, OldMaxRespawnTime);
}

void URTAvatarAttributeSet::OnRep_Radianite(const FGameplayAttributeData& OldRadianite)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URTAvatarAttributeSet, Radianite, OldRadianite);
}
