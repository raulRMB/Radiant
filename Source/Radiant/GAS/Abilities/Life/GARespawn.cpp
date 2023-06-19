// Copyright Radiant Studios


#include "GAS/Abilities/Life/GARespawn.h"

#include "AbilitySystemComponent.h"
#include "Player/RTPlayerController.h"
#include "Modes/Base/RTGameMode.h"
#include "Player/RTPlayerState.h"

UGARespawn::UGARespawn()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag("Event.Respawn");
	AbilityTriggers.Add(TriggerData);
}

void UGARespawn::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if(HasAuthority(&CurrentActivationInfo))
	{
		ARTPlayerController* PC = Cast<ARTPlayerController>(ActorInfo->PlayerController.Get());
		GetWorld()->GetAuthGameMode<ARTGameMode>()->Respawn(PC);
		PC->GetPlayerState<ARTPlayerState>()->SetPlayerStats();
	}
	FGameplayTagContainer TagContainer;
	GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(TagContainer);
	for(auto Tag : TagContainer)
	{
		GetAbilitySystemComponentFromActorInfo()->SetLooseGameplayTagCount(Tag, 0);
		GetAbilitySystemComponentFromActorInfo()->SetReplicatedLooseGameplayTagCount(Tag, 0);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
