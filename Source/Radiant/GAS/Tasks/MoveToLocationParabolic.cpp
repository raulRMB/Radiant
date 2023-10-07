// Copyright Radiant Studios


#include "GAS/Tasks/MoveToLocationParabolic.h"

#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

void UMoveToLocationParabolic::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMoveToLocationParabolic, ParabolicCurve);
	DOREPLIFETIME(UMoveToLocationParabolic, Height);
}

void UMoveToLocationParabolic::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	AActor* MyActor = GetAvatarActor();
	if (MyActor)
	{
		ACharacter* MyCharacter = Cast<ACharacter>(MyActor);
		if (MyCharacter)
		{
			UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MyCharacter->GetMovementComponent());
			if (CharMoveComp)
			{
				CharMoveComp->SetMovementMode(MOVE_Custom, 0);
			}
		}


		float CurrentTime = GetWorld()->GetTimeSeconds();

		if (CurrentTime >= TimeMoveWillEnd)
		{
			bIsFinished = true;

			// Teleport in attempt to find a valid collision spot
			MyActor->TeleportTo(TargetLocation, MyActor->GetActorRotation());
			if (!bIsSimulating)
			{
				MyActor->ForceNetUpdate();
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnTargetLocationReached.Broadcast();
				}
				EndTask();
			}
		}
		else
		{
			FVector NewLocation;

			float MoveFraction = (CurrentTime - TimeMoveStarted) / DurationOfMovement;
			if (LerpCurveVector)
			{
				const FVector ComponentInterpolationFraction = LerpCurveVector->GetVectorValue(MoveFraction);
				NewLocation = FMath::Lerp<FVector, FVector>(StartLocation, TargetLocation, ComponentInterpolationFraction);
			}
			else
			{
				if (LerpCurve)
				{
					MoveFraction = LerpCurve->GetFloatValue(MoveFraction);
				}

				NewLocation = FMath::Lerp<FVector, float>(StartLocation, TargetLocation, MoveFraction);
				
			}
			if(ParabolicCurve)
				NewLocation += ParabolicCurve->GetVectorValue(MoveFraction) * Height;
			MyActor->SetActorLocation(NewLocation);
			MyActor->SetActorRotation((TargetLocation - StartLocation).Rotation());
		}
	}
	else
	{
		bIsFinished = true;
		EndTask();
	}
}

UMoveToLocationParabolic* UMoveToLocationParabolic::MoveToLocationParabolic(UGameplayAbility* OwningAbility,
	FName TaskInstanceName, FVector Location, float Duration, UCurveFloat* OptionalInterpolationCurve,
	UCurveVector* OptionalVectorInterpolationCurve, UCurveVector* ParabolicCurve, float Height)
{
	UMoveToLocationParabolic* MyObj = NewAbilityTask<UMoveToLocationParabolic>(OwningAbility, TaskInstanceName);

	if (MyObj->GetAvatarActor() != nullptr)
	{
		MyObj->StartLocation = MyObj->GetAvatarActor()->GetActorLocation();
	}

	MyObj->TargetLocation = Location;
	MyObj->DurationOfMovement = FMath::Max(Duration, 0.001f);		// Avoid negative or divide-by-zero cases
	MyObj->TimeMoveStarted = MyObj->GetWorld()->GetTimeSeconds();
	MyObj->TimeMoveWillEnd = MyObj->TimeMoveStarted + MyObj->DurationOfMovement;
	MyObj->LerpCurve = OptionalInterpolationCurve;
	MyObj->LerpCurveVector = OptionalVectorInterpolationCurve;
	MyObj->ParabolicCurve = ParabolicCurve;
	MyObj->Height = Height;

	return MyObj;
}
