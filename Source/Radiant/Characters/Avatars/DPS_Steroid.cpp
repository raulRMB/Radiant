// Copyright Radiant Studios


#include "Characters/Avatars/DPS_Steroid.h"

#include "Character/Hero.h"

void UDPS_Steroid::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayCueParameters CueParameters;
	if(AHero* Hero = Cast<AHero>(GetAvatarActorFromActorInfo()))
	{
		CueParameters.TargetAttachComponent = Hero->GetMesh();
		CueParameters.Instigator = Hero;
	}
	if(HasAuthority(&CurrentActivationInfo))
	{
		GetAbilitySystemComponentFromActorInfo_Checked()->ExecuteGameplayCue(EffectCueTag, CueParameters);
	}
}
