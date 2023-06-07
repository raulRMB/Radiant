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

	URTInfoPanel* InfoPanel;

public:
	void ShowEndScreen(bool won);
	void HideLoadScreen();
	void SetFPS(float FPS);
};
