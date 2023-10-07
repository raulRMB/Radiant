// Copyright Radiant Studios


#include "Event/EventBroker.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"


UEventBroker* UEventBroker::Get(const UObject* WorldContextObject)
{
	return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UEventBroker>();
}
