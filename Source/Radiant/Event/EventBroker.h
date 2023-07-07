// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EventBroker.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FItemChangedSignature, const struct FInventoryItem&);

UCLASS()
class RADIANT_API UEventBroker : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FItemChangedSignature ItemChanged;

	static UEventBroker* Get(const UObject* WorldContextObject);
};
