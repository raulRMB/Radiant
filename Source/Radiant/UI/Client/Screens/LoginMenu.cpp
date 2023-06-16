// Copyright Radiant Studios


#include "LoginMenu.h"
#include "UI/Client/ClientSubsystem.h"
#include "../WidgetManager.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void ULoginMenu::NativeConstruct()
{
	Super::NativeConstruct();

	LoginButton->OnClicked.AddDynamic(this, &ULoginMenu::OnLoginButtonClicked);
	LoginButton->OnHovered.AddDynamic(this, &ULoginMenu::OnLoginButtonHovered);
	RegisterButton->OnClicked.AddDynamic(this, &ULoginMenu::OnRegisterButtonClicked);
	RegisterButton->OnHovered.AddDynamic(this, &ULoginMenu::OnRegisterButtonHovered);
	UserNameTextBox->OnTextCommitted.AddDynamic(this, &ULoginMenu::OnEnterPressed);
	PasswordTextBox->OnTextCommitted.AddDynamic(this, &ULoginMenu::OnEnterPressed);
	ExitButton->OnClicked.AddDynamic(this, &ULoginMenu::QuitGame);
	UserNameTextBox->SetFocus();
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->OnLoginErrorMessage.AddUObject(this, &ULoginMenu::HandleError);
	FWidgetSwitchPage* SwitchPageDelegate = GetGameInstance()->GetSubsystem<UClientSubsystem>()->GetPageChangeDelegate();
	if(ensureMsgf(SwitchPageDelegate, TEXT("No SwitchPageDelegate found in ClientSubsystem!")))
	{
		SwitchPageDelegate->AddUObject(this, &ULoginMenu::ResetPage);
	}
}

void ULoginMenu::ResetPage()
{
	ErrorMessage->SetText(FText::FromString(""));
}

void ULoginMenu::OnEnterPressed(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(CommitMethod == ETextCommit::OnEnter)
	{
		OnLoginButtonClicked();
	}
}

void ULoginMenu::OnLoginButtonClicked()
{
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->LoginUser(UserNameTextBox->GetText().ToString(), PasswordTextBox->GetText().ToString());
	OnLoginButtonClicked_BP();
}

void ULoginMenu::OnRegisterButtonClicked()
{
	if(auto WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass())))
	{
		WidgetManager->SwitchTo(FString("RegisterMenu"));
	}
}

void ULoginMenu::OnLoginButtonHovered()
{
	OnLoginButtonHovered_BP();
}

void ULoginMenu::OnRegisterButtonHovered()
{
	OnRegisterButtonHovered_BP();
}

void ULoginMenu::QuitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void ULoginMenu::HandleError(const PlayFab::FPlayFabCppError& Error)
{
	if(Error.ErrorCode == 1000)
	{
		ErrorMessage->SetText(FText::FromString("Login Failed. Please check your username and password."));	
	}
	else
	{
		ErrorMessage->SetText(FText::FromString(Error.ErrorMessage));	
	}
}

