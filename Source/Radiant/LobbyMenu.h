// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyMenu.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ULobbyMenu : public UUserWidget
{
	GENERATED_BODY()

	// create a find match button and bind it to the function
	UPROPERTY(meta = (BindWidget))
	class UButton* FindMatchButton;
private:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnFindMatchButtonClicked();
};
