// Copyright Radiant Studios


#include "LobbyMenu.h"

#include "../ClientSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


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
	FWidgetSwitchPage* SwitchPageDelegate = GetGameInstance()->GetSubsystem<UClientSubsystem>()->GetPageChangeDelegate();
	if(ensureMsgf(SwitchPageDelegate, TEXT("No SwitchPageDelegate found in ClientSubsystem!")))
	{
		SwitchPageDelegate->AddUObject(this, &ULobbyMenu::ResetPage);
	}
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->OnToggleQueueButtons.BindUObject(this, &ULobbyMenu::OnButtonToggle);
}

void ULobbyMenu::ResetPage()
{
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
		FindMatchButton->SetVisibility(ESlateVisibility::Hidden);
		CancelMatchmakingButton->SetVisibility(ESlateVisibility::Visible);
		ErrorMessage->SetText(FText::FromString(""));
	}
	else
	{
		FindMatchButton->SetVisibility(ESlateVisibility::Visible);
		CancelMatchmakingButton->SetVisibility(ESlateVisibility::Hidden);
		ErrorMessage->SetText(FText::FromString(Message));
	}
}

void ULobbyMenu::OnLogoutButtonClicked()
{
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->Logout();
}