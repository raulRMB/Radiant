#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ItemData.h"
#include "Structs/TooltipStatInfo.h"
#include "ItemTooltip.generated.h"

UCLASS()
class UItemTooltip : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTooltipData;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ItemName;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Description;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ItemType;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ClassType;
	
	UPROPERTY()
	TArray<FTooltipStatInfo> StatInfos;

public:
	void ShowTooltip();
	void NativeConstruct() override;
	
	void AddStatInfo(FTooltipStatInfo StatInfo);

	static class UTextBlock* CreateTextBlock(UObject* Outer, FTooltipStatInfo& StatInfo); 
	
	void Init(FItemData* ItemData, FText ItemName);
};
