// Copyright Radiant Studios


#include "LoginMenu.h"
#include "../ClientSubsystem.h"
#include "../../Util/WidgetManager.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

void ULoginMenu::NativeConstruct()
{
	Super::NativeConstruct();

	LoginButton->OnClicked.AddDynamic(this, &ULoginMenu::OnLoginButtonClicked);
	RegisterButton->OnClicked.AddDynamic(this, &ULoginMenu::OnRegisterButtonClicked);
}

void ULoginMenu::OnLoginButtonClicked()
{
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->LoginUser(UserNameTextBox->GetText().ToString(), PasswordTextBox->GetText().ToString());
}

void ULoginMenu::OnRegisterButtonClicked()
{
	if(auto WidgetManager = Cast<AWidgetManager>(UGameplayStatics::GetActorOfClass(this, AWidgetManager::StaticClass())))
	{
		WidgetManager->SwitchTo(FString("RegisterMenu"));
	}
}
