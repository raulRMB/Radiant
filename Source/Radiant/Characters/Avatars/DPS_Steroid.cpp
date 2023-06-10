// Copyright Radiant Studios


#include "Characters/Avatars/DPS_Steroid.h"

#include "Player/Avatar.h"

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
	GetAbilitySystemComponentFromActorInfo_Checked()->ExecuteGameplayCue(EffectCueTag, CueParameters);
}
