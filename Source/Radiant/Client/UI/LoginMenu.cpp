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
	LoginButton->OnHovered.AddDynamic(this, &ULoginMenu::OnLoginButtonHovered);
	RegisterButton->OnClicked.AddDynamic(this, &ULoginMenu::OnRegisterButtonClicked);
	RegisterButton->OnHovered.AddDynamic(this, &ULoginMenu::OnRegisterButtonHovered);
	UserNameTextBox->OnTextCommitted.AddDynamic(this, &ULoginMenu::OnEnterPressed);
	PasswordTextBox->OnTextCommitted.AddDynamic(this, &ULoginMenu::OnEnterPressed);
	UserNameTextBox->SetFocus();
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

