// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreItem.generated.h"


UCLASS()
class RADIANT_API UStoreItem : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UButton* Button;

	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Price;
	
	UPROPERTY()
	FString ItemName;
	
	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTable;
private:
	UFUNCTION()
	void OnClicked();
	
public:
	void Init(const FName& Name);
};
