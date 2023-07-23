﻿#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CraftingNodeDataAsset.generated.h"

UCLASS()
class RADIANT_API UCraftingNodeDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere)
	class UTexture2D* Icon;
	
	UPROPERTY(EditAnywhere)
	TMap<class UCraftingNodeDataAsset*, uint8> Materials;

	UPROPERTY()
	TArray<class UCraftingNodeDataAsset*> Aggregates;
	
	void AddAggregatesToMaterials();
};

inline void UCraftingNodeDataAsset::AddAggregatesToMaterials()
{
	for(TPair<UCraftingNodeDataAsset*, uint8> It : Materials)
	{
		It.Key->Aggregates.AddUnique(this);
	}
}
