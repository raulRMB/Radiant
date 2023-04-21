// Copyright Radiant Studios


#include "LobbyMenu.h"

#include "ClientGameMode.h"
#include "Components/Button.h"

void ULobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	FindMatchButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnFindMatchButtonClicked);
	CancelMatchmakingButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnCancelMatchmakingButtonClicked);
	if(auto Gamemode = Cast<AClientGameMode>(GetWorld()->GetAuthGameMode()))
	{
		Gamemode->OnToggleQueueButtons.BindUObject(this, &ULobbyMenu::OnButtonToggle);
	}
}

void ULobbyMenu::OnFindMatchButtonClicked()
{
	if(auto Gamemode = Cast<AClientGameMode>(GetWorld()->GetAuthGameMode()))
	{
		Gamemode->StartMatchmaking();
	}
}

void ULobbyMenu::OnCancelMatchmakingButtonClicked()
{
	if(auto Gamemode = Cast<AClientGameMode>(GetWorld()->GetAuthGameMode()))
	{
		Gamemode->CancelMatchmaking();
	}
}

void ULobbyMenu::OnButtonToggle(bool bIsMatchmaking)
{
	if(bIsMatchmaking)
	{
		FindMatchButton->SetVisibility(ESlateVisibility::Hidden);
		CancelMatchmakingButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FindMatchButton->SetVisibility(ESlateVisibility::Visible);
		CancelMatchmakingButton->SetVisibility(ESlateVisibility::Hidden);
	}
}