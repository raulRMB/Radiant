// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTHUD.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARTHUD : public AHUD
{
	GENERATED_BODY()
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class URTInfoPanel> InfoPanelClass;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> SettingsPanelClass;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> CaptureAreaBarClass;

	URTInfoPanel* InfoPanel;
	UUserWidget* SettingsPanel;
public:
	TObjectPtr<class UCaptureAreaBar> CaptureAreaBar;
public:
	void ShowEndScreen(bool won);
	UFUNCTION(BlueprintCallable, Exec)
	void ToggleSettings(bool on);
	void HideLoadScreen();
	void SetFPS(float FPS);
};
