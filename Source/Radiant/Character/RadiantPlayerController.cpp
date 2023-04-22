// Copyright Radiant Studios


#include "RadiantPlayerController.h"
#include "EnhancedInputComponent.h"

void ARadiantPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
	}
}
