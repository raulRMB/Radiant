// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Settings.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API USettings : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* KeyList;

	UPROPERTY(meta = (BindWidget))
	class USlider* CameraSpeedSlider;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CameraSpeedText;

	UFUNCTION()
	void OnCameraSpeedChanged(float Value);
	
	virtual void NativeConstruct() override;
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> KeySelectClass;
	void RefreshKeybindList();
};
