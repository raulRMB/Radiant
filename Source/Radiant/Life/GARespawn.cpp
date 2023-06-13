// Copyright Radiant Studios


#include "Life/GARespawn.h"

#include "AbilitySystemComponent.h"
#include "Player/RadiantPlayerController.h"
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
		ARadiantPlayerController* PC = Cast<ARadiantPlayerController>(ActorInfo->PlayerController.Get());
		GetWorld()->GetAuthGameMode<ARTGameMode>()->Respawn(PC);
		PC->GetPlayerState<ARTPlayerState>()->SetPlayerStats();
	}
	FGameplayTagContainer TagContainer;
	GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(TagContainer);
	for(auto Tag : TagContainer)
	{
		GetAbilitySystemComponentFromActorInfo()->UpdateTagMap(Tag, -INT_MAX);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
