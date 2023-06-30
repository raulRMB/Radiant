// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "Base.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ABase : public ABuilding
{
	GENERATED_BODY()
	
	void OnBaseHealthChanged(const FOnAttributeChangeData& Data);
	virtual void Tick(float DeltaTime) override;
	class UStaticMeshComponent* Gem;
protected:
	virtual void BeginPlay() override;
};
