// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Blueprint/UserWidget.h"
#include "KeySelect.generated.h"


UCLASS()
class RADIANT_API UKeySelect : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KeyText;

	UPROPERTY(meta = (BindWidget))
	class UInputKeySelector* Keybind;

	FEnhancedActionKeyMapping Mapping;
public:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void RebindKey(FInputChord SelectedKey);
	void SetMapping(FEnhancedActionKeyMapping NewMapping);
};
