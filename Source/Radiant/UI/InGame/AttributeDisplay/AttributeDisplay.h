// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeDisplay.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UAttributeDisplay : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	class UWidgetAnimation* FloatAnim;	
public:
	void Init(const FText& Text, const FLinearColor& Color, class AAttributeChangeDisplay* Object);
};
