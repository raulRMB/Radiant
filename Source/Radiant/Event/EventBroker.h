// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EventBroker.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FItemChangedSignature, const FName&, int32);

UCLASS()
class RADIANT_API UEventBroker : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FItemChangedSignature ItemAdded;
	FItemChangedSignature ItemRemoved;

	static UEventBroker* Get(const UObject* WorldContextObject);
};
