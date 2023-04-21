// Copyright Radiant Studios


#include "RegisterMenu.h"
#include "Components/Button.h"

void URegisterMenu::NativeConstruct()
{
	Super::NativeConstruct();

	RegisterButton->OnClicked.AddDynamic(this, &URegisterMenu::OnRegisterButtonClicked);
	BackButton->OnClicked.AddDynamic(this, &URegisterMenu::OnBackButtonClicked);
}

void URegisterMenu::OnRegisterButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Register Button Clicked!"));
}

void URegisterMenu::OnBackButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Back Button Clicked!"));
}
