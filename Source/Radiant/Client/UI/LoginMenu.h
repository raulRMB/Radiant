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

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnLoginButtonClicked_BP();
	UFUNCTION(BlueprintImplementableEvent)
	void OnLoginButtonHovered_BP();
	UFUNCTION(BlueprintImplementableEvent)
	void OnRegisterButtonHovered_BP();
	UFUNCTION(BlueprintCallable)
	void OnLoginButtonClicked();

private:

	UFUNCTION()
	void OnRegisterButtonClicked();
	UFUNCTION()
	void OnLoginButtonHovered();
	UFUNCTION()
	void OnRegisterButtonHovered();
	UFUNCTION()
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnEnterPressed(const FText& Text, ETextCommit::Type CommitMethod);
};
