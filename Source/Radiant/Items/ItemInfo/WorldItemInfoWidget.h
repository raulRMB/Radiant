// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldItemInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UWorldItemInfoWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(meta=(BindWidget))
	class UImage* BackgroundImage;

public:
	void SetText(FString Name);
};
