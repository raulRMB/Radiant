// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "RadiantPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARadiantPlayerController : public APlayerController
{
	GENERATED_BODY()	
public:
	virtual void SetupInputComponent() override;

};
