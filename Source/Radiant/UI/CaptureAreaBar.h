// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/TeamId.h"
#include "CaptureAreaBar.generated.h"



UCLASS()
class RADIANT_API UCaptureAreaBar : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UImage* Needle;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* RedBar;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* BlueBar;
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Panel;
	
	UPROPERTY(meta=(BindWidget))
	float Percent = 0.5f;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetTeamPercent(ETeamId Team, float NewPercent);
};
