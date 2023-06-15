// Copyright Radiant Studios

#include "Characters/Avatars/DPS/DPS_Blink.h"

#include "AbilitySystemComponent.h"

void UDPS_Blink::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);
	
	FGameplayCueParameters CueParameters;
	CueParameters.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	GetAbilitySystemComponentFromActorInfo_Checked()->ExecuteGameplayCue(BlinkStartCue, CueParameters);

	CueParameters.Location = GetRangeBasedMouseLocation();
	GetAbilitySystemComponentFromActorInfo_Checked()->ExecuteGameplayCue(BlinkEndCue, CueParameters);
}
