// Copyright Radiant Studios


#include "LobbyMenu.h"

#include "ClientGameMode.h"
#include "Components/Button.h"

void ULobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	FindMatchButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnFindMatchButtonClicked);
}

void ULobbyMenu::OnFindMatchButtonClicked()
{
	if(auto Gamemode = Cast<AClientGameMode>(GetWorld()->GetAuthGameMode()))
		Gamemode->StartMatchmaking();
}
