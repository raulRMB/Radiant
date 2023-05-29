// Copyright Radiant Studios


#include "LobbyMenu.h"

#include "../ClientSubsystem.h"
#include "Components/Button.h"



void ULobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	FindMatchButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnFindMatchButtonClicked);
	FindMatchButton->OnHovered.AddDynamic(this, &ULobbyMenu::OnFindMatchButtonHovered);
	CancelMatchmakingButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnCancelMatchmakingButtonClicked);
	CancelMatchmakingButton->OnHovered.AddDynamic(this, &ULobbyMenu::OnCancelMatchmakingButtonHovered);
	
	GetGameInstance()->GetSubsystem<UClientSubsystem>()->OnToggleQueueButtons.BindUObject(this, &ULobbyMenu::OnButtonToggle);
	
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

void ULobbyMenu::OnCancelMatchmakingButtonClicked()
{
	OnCancelMatchmakingButtonClicked_BP();
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