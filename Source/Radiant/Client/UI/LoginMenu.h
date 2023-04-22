// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginMenu.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ULoginMenu : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* UserNameTextBox;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* PasswordTextBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RegisterButton;

private:
	UFUNCTION()
	void OnLoginButtonClicked();

	UFUNCTION()
	void OnRegisterButtonClicked();

	virtual void NativeConstruct() override;
};
