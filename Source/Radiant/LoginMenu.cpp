// Copyright Radiant Studios


#include "LoginMenu.h"
#include "Components/Button.h"

void ULoginMenu::NativeConstruct()
{
	Super::NativeConstruct();

	LoginButton->OnClicked.AddDynamic(this, &ULoginMenu::OnLoginButtonClicked);
	RegisterButton->OnClicked.AddDynamic(this, &ULoginMenu::OnRegisterButtonClicked);
}

void ULoginMenu::OnLoginButtonClicked()
{
	
}

void ULoginMenu::OnRegisterButtonClicked()
{
}

