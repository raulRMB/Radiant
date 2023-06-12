// Copyright Radiant Studios


#include "RegisterMenu.h"
#include "../ClientSubsystem.h"
#include "../../Util/WidgetManager.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void URegisterMenu::HandleError(const PlayFab::FPlayFabCppError& Error)
{
	if(Error.ErrorCode == 1000)
	{
		ErrorMessage->SetText(FText::FromString("Registration Failed. Check you have entered a valid email, your username is 3 or more characters, and password is at least 6 characters."));	
	}
	else
	{
		ErrorMessage->SetText(FText::FromString(Error.ErrorMessage));	
	}
}

void URegisterMenu::NativeConstruct()
{
	Super::NativeConstruct();

	RegisterButton->OnClicked.AddDynamic(this, &URegisterMenu::OnRegisterButtonClicked);
	BackButton->OnClicked.AddDynamic(this, &URegisterMenu::OnBackButtonClicked);
	OnRegisterSuccess.BindUObject(this, &URegisterMenu::OnRegisterSuccessCallback);
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->OnRegisterErrorMessage.AddUObject(this, &URegisterMenu::HandleError);
}

void URegisterMenu::OnRegisterButtonClicked()
{
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->RegisterUser(EmailTextBox->GetText().ToString(), UserNameTextBox->GetText().ToString(), PasswordTextBox->GetText().ToString());
}

void URegisterMenu::OnBackButtonClicked()
{
	if(auto WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass())))
	{
		WidgetManager->SwitchTo(FString("LoginMenu"));
	}
}

void URegisterMenu::OnRegisterSuccessCallback()
{
	RemoveFromParent();
}
