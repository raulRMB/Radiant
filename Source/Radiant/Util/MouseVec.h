// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "UObject/Interface.h"
#include "MouseVec.generated.h"

UCLASS(BlueprintType)
class UMouseVec : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	mutable FVector MouseVec;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UMouseVec, MouseVec);
	}

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	
};

