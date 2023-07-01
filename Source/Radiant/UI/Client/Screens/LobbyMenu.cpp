// Copyright Radiant Studios


#include "LobbyMenu.h"

#include "../ClientSubsystem.h"
#include "Components/Button.h"
#include "Components/ComboBox.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Util/Util.h"


void ULobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	FindMatchButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnFindMatchButtonClicked);
	FindMatchButton->OnHovered.AddDynamic(this, &ULobbyMenu::OnFindMatchButtonHovered);
	CancelMatchmakingButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnCancelMatchmakingButtonClicked);
	CancelMatchmakingButton->OnHovered.AddDynamic(this, &ULobbyMenu::OnCancelMatchmakingButtonHovered);
	LogoutButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnLogoutButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &ULobbyMenu::QuitGame);
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->OnLobbyErrorMessage.AddUObject(this, &ULobbyMenu::HandleError);
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->OnWidgetSwitchPage.AddUObject(this, &ULobbyMenu::ResetPage);
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->OnMatchmakingStatusChanged.BindUObject(this, &ULobbyMenu::OnButtonToggle);
	ResetPage();
	QueueSelector->SetIsEnabled(true);
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->SetQueueName("1v1");
}

void ULobbyMenu::ResetPage()
{
	Welcome->SetText(FText::FromString(GetGameInstance()->GetSubsystem<UClientSubsystem>()->Username));
	FindMatchButton->SetVisibility(ESlateVisibility::Visible);
	CancelMatchmakingButton->SetVisibility(ESlateVisibility::Hidden);
	ErrorMessage->SetText(FText::FromString(""));
}

void ULobbyMenu::OnFindMatchButtonClicked()
{
	OnFindMatchButtonClicked_BP();
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->StartMatchmaking();
}

void ULobbyMenu::OnFindMatchButtonHovered()
{
	OnFindMatchButtonHovered_BP();
}

void ULobbyMenu::OnCancelMatchmakingButtonHovered()
{
	OnCancelMatchmakingButtonHovered_BP();
}

void ULobbyMenu::QuitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void ULobbyMenu::HandleError(const PlayFab::FPlayFabCppError& PlayFabCppError)
{
	ErrorMessage->SetText(FText::FromString(PlayFabCppError.ErrorMessage));
}

void ULobbyMenu::OnCancelMatchmakingButtonClicked()
{
	OnCancelMatchmakingButtonClicked_BP();
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->CancelMatchmaking();
}

void ULobbyMenu::OnButtonToggle(bool bIsMatchmaking, FString Message)
{
	if(bIsMatchmaking)
	{
		QueueSelector->SetIsEnabled(false);
		FindMatchButton->SetVisibility(ESlateVisibility::Hidden);
		CancelMatchmakingButton->SetVisibility(ESlateVisibility::Visible);
		CancelButtonText->SetText(RTPRINTF("Cancel Queue: %s", *Message));
		ErrorMessage->SetText(FText::FromString(""));
	}
	else
	{
		QueueSelector->SetIsEnabled(true);
		FindMatchButton->SetVisibility(ESlateVisibility::Visible);
		CancelMatchmakingButton->SetVisibility(ESlateVisibility::Hidden);
		ErrorMessage->SetText(FText::FromString(Message));
	}
}

void ULobbyMenu::OnLogoutButtonClicked()
{
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->Logout();
}
