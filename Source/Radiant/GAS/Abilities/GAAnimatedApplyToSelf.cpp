#include "GAAnimatedApplyToSelf.h"

#include "AbilitySystemComponent.h"

void UGAAnimatedApplyToSelf::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);
	for(auto Effect : Effects)
	{
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectToSelf(Effect.GetDefaultObject(), 1, GetAbilitySystemComponentFromActorInfo()->MakeEffectContext());
	}
}
