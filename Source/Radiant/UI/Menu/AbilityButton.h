// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityButton.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UAbilityButton : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
	virtual bool Initialize() override;
	void RefreshButton();
	virtual void SynchronizeProperties() override;
	UPROPERTY(EditAnywhere)
	class UAbilityDataAsset* AbilityData;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* Icon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Tooltip;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;

public:
	void SetAbilityData(class UAbilityDataAsset* Data);
private:
	UFUNCTION()
	void OnButtonClicked();
};
