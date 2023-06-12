// Copyright Radiant Studios


#include "Characters/Avatars/DPS_Steroid.h"

#include "Player/Avatar.h"
#include "Util/Util.h"

void UDPS_Steroid::RemoveCue()
{
	GetAbilitySystemComponentFromActorInfo_Checked()->RemoveGameplayCue(EffectCueTag);
	RTLOGP("Cue removed: %s", *EffectCueTag.ToString());
	RTPRINTP("Cue removed: %s", *EffectCueTag.ToString());
}

void UDPS_Steroid::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	FGameplayCueParameters CueParameters;
	if(AAvatar* Hero = Cast<AAvatar>(GetAvatarActorFromActorInfo()))
	{
		CueParameters.TargetAttachComponent = Hero->GetMesh();
		CueParameters.Instigator = Hero;
	}
	GetAbilitySystemComponentFromActorInfo_Checked()->AddGameplayCue(EffectCueTag, CueParameters);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([=]
	{
		RemoveCue();
	}), EffectDuration, false);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
