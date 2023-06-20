// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "GameFramework/SaveGame.h"
#include "UserSettings.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UUserSettings : public USaveGame
{
	GENERATED_BODY()
public:
	UUserSettings();
	UPROPERTY()
	TArray<FEnhancedActionKeyMapping> Mappings;
};
