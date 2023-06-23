// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreItemDescription.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UStoreItemDescription : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UButton* Button;

	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Price;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Description;
};
