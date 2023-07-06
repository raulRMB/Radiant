// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SheepAnimInstance.generated.h"


UCLASS()
class RADIANT_API USheepAnimInstance : public UAnimInstance
{
GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementSpeed;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
