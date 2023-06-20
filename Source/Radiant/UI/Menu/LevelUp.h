// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUp.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ULevelUp : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

	virtual void SynchronizeProperties() override;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* AbilityList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilityButton> AbilityButtonClass;
public:
	void RefreshList();
	UPROPERTY(EditAnywhere)
	TArray<class UAbilityDataAsset*> Abilities;

	bool ShouldShow() const { return Abilities.Num() > 0; }
	void RemoveAbility(class UAbilityDataAsset* AbilityData);
};
