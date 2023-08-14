#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vein.generated.h"

UCLASS()
class AVein : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(Replicated)
	float RemainingRadianite = 0;
	UPROPERTY(Replicated)
	bool bEmpowered = false;
	
public:
	bool OnExtracted(float Amount);
	
	void SetEmpowered(const bool Empowered);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
