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
	Super::OnAnimCancelled(EventTag, EventData);
	RemoveMovementStoppedEffect();
}

void UMeleeBasicAttack::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimCompleted(EventTag, EventData);
	RemoveMovementStoppedEffect();
}

void UMeleeBasicAttack::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimInterrupted(EventTag, EventData);
	RemoveMovementStoppedEffect();
}

void UMeleeBasicAttack::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimBlendOut(EventTag, EventData);
	RemoveMovementStoppedEffect();
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

void UMeleeBasicAttack::OnSetUncancelable(FGameplayEventData Payload)
{
	bIsCancelable = false;
	MovementStoppedEffectHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectToSelf(MovementStoppedEffect.GetDefaultObject(), 1.f, GetAbilitySystemComponentFromActorInfo()->MakeEffectContext());
}

void UMeleeBasicAttack::OnUnsetUncancelable(FGameplayEventData Payload)
{
	bIsCancelable = true;
	RemoveMovementStoppedEffect();
}

void UMeleeBasicAttack::RemoveMovementStoppedEffect()
{
	if(bHasRemovedMovementStoppedEffect)
	{
		return;
	}
	bHasRemovedMovementStoppedEffect = true;
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(MovementStoppedEffectHandle, 1);
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

	float AttackSpeed = Cast<ARTPlayerState>(GetOwningActorFromActorInfo())->GetAttributeSet()->GetAttackSpeed();
	BindAnimations(AttackSpeed);

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.State.Uncancelable.Set"));
	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag);
	Task->EventReceived.AddDynamic(this, &UMeleeBasicAttack::OnSetUncancelable);
	Task->Activate();

	EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.State.Uncancelable.Unset"));
	UAbilityTask_WaitGameplayEvent* Task2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag);
	Task2->EventReceived.AddDynamic(this, &UMeleeBasicAttack::OnUnsetUncancelable);
	Task2->Activate();
}
