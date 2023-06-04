// Copyright Radiant Studios


#include "Characters/Avatars/DPS_Skillshot.h"

#include "AbilitySystemComponent.h"
#include "Character/Hero.h"

void UDPS_Skillshot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	FGameplayCueParameters CueParameters;
	CueParameters.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	if(AHero* Hero = Cast<AHero>(GetAvatarActorFromActorInfo()))
		CueParameters.TargetAttachComponent = Hero->GetMesh();	
	
	GetAbilitySystemComponentFromActorInfo_Checked()->ExecuteGameplayCue(CastStartCue, CueParameters);
}
