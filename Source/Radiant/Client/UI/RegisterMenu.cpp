// Copyright Radiant Studios


#include "RegisterMenu.h"
#include "../ClientSubsystem.h"
#include "../../Util/WidgetManager.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

void URegisterMenu::NativeConstruct()
{
	Super::NativeConstruct();

	RegisterButton->OnClicked.AddDynamic(this, &URegisterMenu::OnRegisterButtonClicked);
	BackButton->OnClicked.AddDynamic(this, &URegisterMenu::OnBackButtonClicked);
	OnRegisterSuccess.BindUObject(this, &URegisterMenu::OnRegisterSuccessCallback);
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
