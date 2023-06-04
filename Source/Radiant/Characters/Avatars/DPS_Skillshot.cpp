// Copyright Radiant Studios


#include "Characters/Avatars/DPS_Skillshot.h"

#include "AbilitySystemComponent.h"
#include "Character/Hero.h"

void UDPS_Skillshot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);	
}

void UDPS_Skillshot::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimEventReceived(EventTag, EventData);

	FGameplayCueParameters CueParameters;
	CueParameters.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	if(AHero* Hero = Cast<AHero>(GetAvatarActorFromActorInfo()))
	{
		CueParameters.TargetAttachComponent = Hero->GetMesh();
		CueParameters.Instigator = Hero;
	}
	GetAbilitySystemComponentFromActorInfo_Checked()->ExecuteGameplayCue(CastStartCue, CueParameters);
}