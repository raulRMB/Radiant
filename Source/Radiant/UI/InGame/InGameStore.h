// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameStore.generated.h"


UCLASS()
class RADIANT_API UInGameStore : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UGridPanel* GridPanel;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RadianiteText;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UStoreItem> StoreItemClass;
	
	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTable;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateRadianite(float RadianiteAmount);
public:
	void Init(class ARTPlayerState* PS);
};
