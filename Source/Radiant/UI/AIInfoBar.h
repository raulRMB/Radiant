// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AIInfoBar.generated.h"


UCLASS()
class RADIANT_API UAIInfoBar : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Level;
	UPROPERTY(meta = (BindWidget))
	class UImage* LevelBg;
public:
	void ShowLevel(bool Value);
	void SetHealthPercent(float Percent);
	void SetLevel(int32 Value);
	void SetColor(FColor Color);
};
