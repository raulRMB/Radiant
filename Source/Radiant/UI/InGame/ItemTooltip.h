#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

	UPROPERTY()
	TArray<FTooltipStatInfo> StatInfos;

public:
	void AddStatInfo(FTooltipStatInfo StatInfo);

	static class UTextBlock* CreateTextBlock(UObject* Outer, FTooltipStatInfo& StatInfo); 
	
	void Init();
};
