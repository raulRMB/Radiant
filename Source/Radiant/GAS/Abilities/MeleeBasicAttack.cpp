// Copyright Radiant Studios

#include "MeleeBasicAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Player/Avatar.h"
#include "GAS/Tasks/PlayMontageAndWaitForEvent.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void UMeleeBasicAttack::OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	bIsCancelable = true;
	Super::OnAnimCancelled(EventTag, EventData);
}

void UMeleeBasicAttack::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	bIsCancelable = true;
	Super::OnAnimCompleted(EventTag, EventData);
}

void UMeleeBasicAttack::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	bIsCancelable = true;
	Super::OnAnimInterrupted(EventTag, EventData);
}

void UMeleeBasicAttack::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	bIsCancelable = true;
	Super::OnAnimBlendOut(EventTag, EventData);
}

void UMeleeBasicAttack::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimEventReceived(EventTag, EventData);

	if(HasAuthority(&CurrentActivationInfo))
	{
		if(ARTPlayerState* PlayerState = Cast<ARTPlayerState>(GetOwningActorFromActorInfo()))
		{
			if(IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PlayerState->GetTarget()))
			{
				if(UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
				{
					for(TSubclassOf<UGameplayEffect>& EffectClass : Effects)
					{
						UGameplayEffect* Effect = EffectClass.GetDefaultObject();
						PlayerState->GetAbilitySystemComponent()->ApplyGameplayEffectToTarget(Effect, ASC, 1.f, PlayerState->GetAbilitySystemComponent()->MakeEffectContext());
					}
				}
			}
		}
	}
	
	ReturnToDefault();
}

void UMeleeBasicAttack::OnUncancellableEventRecieved(FGameplayEventData EventData)
{
	bIsCancelable = false;
}

void UMeleeBasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	URTAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AAvatar* Avatar = Cast<AAvatar>(GetAvatarActorFromActorInfo());
	
	AActor* Target = Avatar->GetPlayerState<ARTPlayerState>()->GetTarget();
	if(!Target)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	FVector Loc = Avatar->GetActorLocation();
	Loc.Z = 0;
	FVector Direction = (Target->GetActorLocation() - Loc).GetSafeNormal();
	Avatar->SetRotationLock(true, Direction);

	SetMouseWorldLocation(Target->GetActorLocation());

	UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Notify.Uncancellable"));
	WaitEvent->EventReceived.AddDynamic(this, &UMeleeBasicAttack::OnUncancellableEventRecieved);
	WaitEvent->ReadyForActivation();

	float AttackSpeed = Cast<ARTPlayerState>(GetOwningActorFromActorInfo())->GetAttributeSet()->GetAttackSpeed();
	BindAnimations(AttackSpeed);
}