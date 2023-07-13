// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Hooker.generated.h"

USTRUCT()
struct FHookSocketInfo
{
	GENERATED_BODY()

	UPROPERTY()
	class USceneComponent* Component;
	FName SocketName;

	FHookSocketInfo() : Component(nullptr), SocketName(NAME_None) {}
};

UINTERFACE(MinimalAPI)
class UHooker : public UInterface
{
	GENERATED_BODY()
};

class RADIANT_API IHooker
{
	GENERATED_BODY()

public:
	virtual FHookSocketInfo GetHookSocketInfo() const = 0;
};
