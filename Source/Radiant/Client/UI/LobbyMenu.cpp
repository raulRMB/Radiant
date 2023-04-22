// Copyright Radiant Studios


#include "LobbyMenu.h"

#include "../ClientSubsystem.h"
#include "Components/Button.h"

void ULobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	FindMatchButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnFindMatchButtonClicked);
	CancelMatchmakingButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnCancelMatchmakingButtonClicked);
	
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->OnToggleQueueButtons.BindUObject(this, &ULobbyMenu::OnButtonToggle);
	
}

void ULobbyMenu::OnFindMatchButtonClicked()
{
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->StartMatchmaking();
}

void ULobbyMenu::OnCancelMatchmakingButtonClicked()
{
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->CancelMatchmaking();
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