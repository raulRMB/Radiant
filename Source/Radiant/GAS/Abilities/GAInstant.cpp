// Copyright Radiant Studios


#include "GAS/Abilities/GAInstant.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Util/Util.h"

void UGAInstant::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(HasAuthority(&CurrentActivationInfo))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TriggerEventData->TargetData,0);
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
	
		for(TSubclassOf<UGameplayEffect> Effect : GameplayEffects)
		{
			UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			if(AbilityTarget == EInstantAbilityTarget::Self || AbilityTarget == EInstantAbilityTarget::SelfAndTarget)
			{
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1.0f, ContextHandle);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Duration")), EffectDuration);
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				
				CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
			}
			else if (AbilityTarget == EInstantAbilityTarget::Target || AbilityTarget == EInstantAbilityTarget::SelfAndTarget)
			{
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1.0f, ContextHandle);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Duration")), EffectDuration);
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
			}
		}
	}
}
