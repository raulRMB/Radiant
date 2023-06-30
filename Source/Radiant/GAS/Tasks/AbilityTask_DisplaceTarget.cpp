// Copyright Radiant Studios


#include "GAS/Tasks/AbilityTask_DisplaceTarget.h"
#include "Characters/RTCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Util/Util.h"

void UAbilityTask_DisplaceTarget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityTask_DisplaceTarget, TargetToDisplace);
}

UAbilityTask_DisplaceTarget* UAbilityTask_DisplaceTarget::DisplaceTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, FDisplaceTargetData Data)
{
	UAbilityTask_DisplaceTarget* MyTask = NewAbilityTask<UAbilityTask_DisplaceTarget>(OwningAbility, TaskInstanceName);

	MyTask->TargetToDisplace = Data.Target;
	MyTask->ForceName = TaskInstanceName;
	MyTask->TargetLocation = Data.Location;
	MyTask->Duration = Data.Duration;
	MyTask->bSetNewMovementMode = true;
	MyTask->NewMovementMode = EMovementMode::MOVE_Walking;
	MyTask->PathOffsetCurve = nullptr;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.0f;

	if(Data.Target)
	{
		MyTask->StartLocation = Data.Target->GetActorLocation();
	}
	else
	{
		MyTask->StartLocation = Data.Location;
	}

	MyTask->SharedInitAndApply();	

	return MyTask;
}

void UAbilityTask_DisplaceTarget::SharedInitAndApply()
{
	ACharacter* CharacterToDisplace = Cast<ACharacter>(TargetToDisplace);
	
	if (CharacterToDisplace->GetMovementComponent())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(CharacterToDisplace->GetMovementComponent());
		StartTime = GetWorld()->GetTimeSeconds();
		EndTime = StartTime + Duration;

		if (MovementComponent)
		{
			if (bSetNewMovementMode)
			{
				PreviousMovementMode = MovementComponent->MovementMode;
				PreviousCustomMovementMode = MovementComponent->CustomMovementMode;
				MovementComponent->SetMovementMode(NewMovementMode);
			}

			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionMoveToForce") : ForceName;
			TSharedPtr<FRootMotionSource_MoveToForce> MoveToForce = MakeShared<FRootMotionSource_MoveToForce>();
			MoveToForce->InstanceName = ForceName;
			MoveToForce->AccumulateMode = ERootMotionAccumulateMode::Override;
			MoveToForce->Settings.SetFlag(ERootMotionSourceSettingsFlags::UseSensitiveLiftoffCheck);
			MoveToForce->Priority = 1000;
			MoveToForce->TargetLocation = TargetLocation;
			MoveToForce->StartLocation = StartLocation;
			MoveToForce->Duration = Duration;
			MoveToForce->bRestrictSpeedToExpected = bRestrictSpeedToExpected;
			MoveToForce->PathOffsetCurve = PathOffsetCurve;
			MoveToForce->FinishVelocityParams.Mode = FinishVelocityMode;
			MoveToForce->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			MoveToForce->FinishVelocityParams.ClampVelocity = FinishClampVelocity;
			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(MoveToForce);
		}
	}
	else
	{
		RTLOG("No Target");
	}
}
