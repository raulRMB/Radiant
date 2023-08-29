// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamInfoDisplay.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UTeamInfoDisplay : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* BlueLevel;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* BlueXPBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RedLevel;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* RedXPBar;

public:
	void Init();

private:
	
	UFUNCTION()
	void OnRedRadianiteChanged(double XP, uint32 Level);
	
	UFUNCTION()
	void OnBlueRadianiteChanged(double XP, uint32 Level);	
};
