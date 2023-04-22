// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RegisterMenu.generated.h"

DECLARE_DELEGATE(FRegisterSuccessSignature)

/**
 * 
 */
UCLASS()
class RADIANT_API URegisterMenu : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* EmailTextBox;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* UserNameTextBox;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* PasswordTextBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* RegisterButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnRegisterButtonClicked();

	UFUNCTION()
	void OnBackButtonClicked();
public:
	FRegisterSuccessSignature OnRegisterSuccess;
private:
	UFUNCTION()
		void OnRegisterSuccessCallback();
};
