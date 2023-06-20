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

	virtual void NativeConstruct() override;
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> KeySelectClass;
	void RefreshKeybindList();
};
