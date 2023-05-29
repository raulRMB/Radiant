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
	
	Owner->GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("States.Movement.Stopped"), 1);
	
	UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(this, FName("ConfirmTargeting"), EGameplayTargetingConfirmation::Instant, AMouseTargetActor::StaticClass());
	AGameplayAbilityTargetActor* TargetActor; 
	Task->BeginSpawningActor(this, AMouseTargetActor::StaticClass(), TargetActor);
	
	if(!HasAuthority(&ActivationInfo))
	{
		Instigator = Avatar;
		const UMouseVec* Dir = Cast<UMouseVec>(TriggerEventData->OptionalObject.Get());
		AMouseTargetActor* MouseTargetActor = GetWorld()->SpawnActor<AMouseTargetActor>(AMouseTargetActor::StaticClass(), Avatar->GetActorLocation(), FRotator::ZeroRotator);
		MouseTargetActor->MouseVec = Dir->MouseVec;
		TargetActor->SourceActor = MouseTargetActor;
	}
	Task->ValidData.AddDynamic(this, &UGALinearSkillshot::OnTargetValidData);
	Task->FinishSpawningActor(this, TargetActor);
}

void UGALinearSkillshot::OnTargetValidData(const FGameplayAbilityTargetDataHandle& Data)
{
	FVector Vec = UUtil::GetMouseVecFromTargetData(Data);
	UE_LOG(LogTemp, Warning, TEXT("Vec: %s"), *Vec.ToString());
}
