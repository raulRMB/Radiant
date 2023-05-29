// Copyright Radiant Studios


#include "GAS/Abilities/GALinearSkillshot.h"

#include "Character/Hero.h"
#include "Player/RTPlayerState.h"
#include "Util/MouseTargetActor.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Util/MouseVec.h"
#include "Util/Util.h"

UGALinearSkillshot::UGALinearSkillshot()
{
	FAbilityTriggerData AbilityTrigger;
	AbilityTrigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTrigger.TriggerTag = FGameplayTag::RequestGameplayTag("Ability.Combat.Fireball");
	AbilityTriggers.Add(AbilityTrigger);
}

void UGALinearSkillshot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AHero* Avatar = Cast<AHero>(GetAvatarActorFromActorInfo());
	ARTPlayerState* Owner = Cast<ARTPlayerState>(GetOwningActorFromActorInfo());

}