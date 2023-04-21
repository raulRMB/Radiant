// Copyright Radiant Studios


#include "LobbyMenu.h"
#include "Components/Button.h"

void ULobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	FindMatchButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnFindMatchButtonClicked);
}

void ULobbyMenu::OnFindMatchButtonClicked()
{
	
}
