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

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelMatchmakingButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* LogoutButton;
	
	UFUNCTION()
	void OnFindMatchButtonHovered();
	UFUNCTION()
	void OnCancelMatchmakingButtonHovered();
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnFindMatchButtonClicked();

	UFUNCTION()
	void OnCancelMatchmakingButtonClicked();

	UFUNCTION()
	void OnButtonToggle(bool bIsMatchmaking);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnFindMatchButtonClicked_BP();
	UFUNCTION(BlueprintImplementableEvent)
	void OnFindMatchButtonHovered_BP();
	UFUNCTION(BlueprintImplementableEvent)
	void OnCancelMatchmakingButtonClicked_BP();
	UFUNCTION(BlueprintImplementableEvent)
	void OnCancelMatchmakingButtonHovered_BP();
	UFUNCTION()
	void OnLogoutButtonClicked();
};
