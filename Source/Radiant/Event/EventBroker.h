// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EventBroker.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FItemChangedSignature, const class FName&, const uint32 Amount);
DECLARE_MULTICAST_DELEGATE_OneParam(FDragStatus, bool);
UCLASS()
class RADIANT_API UEventBroker : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FItemChangedSignature ItemChanged;
	FDragStatus DragStatusChanged;
	static UEventBroker* Get(const UObject* WorldContextObject);
};
