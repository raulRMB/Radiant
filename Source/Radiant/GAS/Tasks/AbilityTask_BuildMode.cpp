// Copyright Radiant Studios


#include "GAS/Tasks/AbilityTask_BuildMode.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

UAbilityTask_BuildMode::UAbilityTask_BuildMode(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_BuildMode* UAbilityTask_BuildMode::BuildModeTask(UGameplayAbility* OwningAbility, FName TaskInstanceName,
                                                              TSubclassOf<class AActor> NewBuildingType)
{
	UAbilityTask_BuildMode* Task = NewAbilityTask<UAbilityTask_BuildMode>(OwningAbility, TaskInstanceName);
	Task->BuildingType = NewBuildingType;
	return Task;
}

void UAbilityTask_BuildMode::Activate()
{
	Super::Activate();

	if(IsValid(Ability))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Actor = GetWorld()->SpawnActor<AActor>(BuildingType, FTransform(FVector(0.f, 0.f, 0.f)), SpawnParams);
	}
}

void UAbilityTask_BuildMode::OnDestroy(bool AbilityEnded)
{
	if(IsValid(Actor))
	{
		Actor->Destroy();
	}
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_BuildMode::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if(IsValid(Ability) && Ability->IsActive() && IsValid(Actor))
	{
		FVector Mouse = UUtil::GetMousePosition(this, {});
		Mouse.X = FMath::RoundToInt(Mouse.X / 200.f);
		Mouse.Y = FMath::RoundToInt(Mouse.Y / 200.f);
		Mouse.Z = 0.f;
		Actor->SetActorLocation(Mouse * 200.f);
	}
}

