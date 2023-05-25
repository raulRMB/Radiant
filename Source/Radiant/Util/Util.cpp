// Copyright Radiant Studios


#include "Util/Util.h"

#include "Modes/RTGameMode.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

struct FGameplayAbilityTargetDataHandle;

FVector UUtil::GetMousePosition(class UObject* WorldContext, TArray<AActor*> IgnoredActors)
{
	FVector2D MousePosition;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetMousePosition(MousePosition);
	}

	// Get the world position
	FVector WorldPosition;
	FVector WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(WorldContext->GetWorld()->GetFirstPlayerController(), MousePosition, WorldPosition, WorldDirection);
	
	// Get the hit result
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	for(auto Actor : IgnoredActors)
	{
		CollisionQueryParams.AddIgnoredActor(Actor);
	}
	WorldContext->GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel2, CollisionQueryParams);

	return HitResult.Location;
}

FVector UUtil::ProjectileDirection(FVector A, FVector B)
{
	return FVector(B.X - A.X, B.Y - A.Y, 0).GetSafeNormal();
}

FVector UUtil::GetMouseVecFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData)
{
		if(TargetData.Data.Num() > 0)
		{
			FGameplayAbilityTargetData* Data = TargetData.Data[0].Get();
			if (Data)
			{
				const FVector Mouse = Data->GetEndPointTransform().GetTranslation();
				return Mouse;
			}
		}
	
	return FVector::ZeroVector;
}

FVector UUtil::ClampVectorMaxDist(FVector A, FVector B, float MaxDist)
{
	FVector Vector = B - A;
	if(Vector.Size() > MaxDist)
	{
		Vector = Vector.GetSafeNormal() * MaxDist;
		Vector += A;
		return Vector;
	}

	return B;
}

AHero* UUtil::GetHeroFromPlayerID(UObject* WorldContext, int PlayerID)
{
	if(ARTGameMode* GM = Cast<ARTGameMode>(UGameplayStatics::GetGameMode(WorldContext)))
	{
		return GM->GetHeroFromPlayerID(PlayerID);
	}
	
	return nullptr;
}
