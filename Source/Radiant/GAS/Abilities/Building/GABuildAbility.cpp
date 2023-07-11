// Copyright Radiant Studios


#include "GAS/Abilities/Building/GABuildAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Data/ItemData.h"
#include "GAS/Tasks/AbilityTask_BuildMode.h"
#include "Player/Avatar.h"
#include "Player/InventoryComponent.h"
#include "Player/RTPlayerController.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

UGABuildAbility::UGABuildAbility()
{
	bIsCancelable = false;
}

void UGABuildAbility::OnOrderAccepted()
{
	GetAvatar()->CastAbility(BuildAcceptAbility.GetDefaultObject()->GetTriggerTag());

	if(ARTPlayerState* PS = Cast<ARTPlayerState>(GetOwningActorFromActorInfo()))
	{
		if(ARTPlayerController* PC = Cast<ARTPlayerController>(PS->GetPlayerController()))
		{
			PC->OrderAccepted.Remove(OrderAcceptedHandle);
		}
	}

	GetAvatar()->GetInventory()->S_ItemUsed(ItemName);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGABuildAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	ChildSpecHandle = ActorInfo->AbilitySystemComponent->GiveAbility(BuildAcceptAbility.GetDefaultObject());
}

void UGABuildAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	ActorInfo->AbilitySystemComponent->ClearAbility(ChildSpecHandle);
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UGABuildAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{	
	if(!HasAuthority(&CurrentActivationInfo))
	{
		UAbilityTask_BuildMode* Task = UAbilityTask_BuildMode::BuildModeTask(this, FName("BuildModeTask"), EnvironmentType);
		Task->ReadyForActivation();
	}
	
	if(ARTPlayerState* PS = Cast<ARTPlayerState>(GetOwningActorFromActorInfo()))
	{
		if(ARTPlayerController* PC = Cast<ARTPlayerController>(PS->GetPlayerController()))
		{
			OrderAcceptedHandle = PC->OrderAccepted.AddUObject(this, &UGABuildAbility::OnOrderAccepted);
		}
	}
}
