// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Util.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UUtil :  public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static FVector GetMousePosition(class UObject* WorldContext, TArray<AActor*> IgnoredActors);

	UFUNCTION(BlueprintCallable)
	static FVector ProjectileDirection(FVector A, FVector B);

	UFUNCTION(BlueprintCallable)
	static FVector GetMouseVecFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintCallable)
	static FVector ClampVectorMaxDist(FVector A, FVector B, float MaxDist);

	UFUNCTION(BlueprintCallable)
	static AHero* GetHeroFromPlayerID(class UObject* WorldContext, int PlayerID);
};
