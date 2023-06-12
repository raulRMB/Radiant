// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Util.generated.h"

#define RTPRINTP(f, x) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(f), x));
#define RTPRINT(f) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(f)));
#define RTLOG(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define RTLOGP(x, a) UE_LOG(LogTemp, Warning, TEXT(x), a);

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
	static AAvatar* GetHeroFromPlayerID(class UObject* WorldContext, int PlayerID);

	UFUNCTION(BlueprintCallable)
	static void ChangeQualitySetting(FString Name); 
};
