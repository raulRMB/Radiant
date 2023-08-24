// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/ItemSlotID.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Util/Managers/GridManager.h"
#include "Util.generated.h"

#define RTPRINTP(f, x) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(f), x));
#define RTPRINT(f) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(f)));
#define RTPRINTT(f, t) GEngine->AddOnScreenDebugMessage(-1, t, FColor::Red, FString::Printf(TEXT(f)));
#define RTPRINTTP(t, f, x) GEngine->AddOnScreenDebugMessage(-1, t, FColor::Red, FString::Printf(TEXT(f), x));
#define RTLOG(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define RTLOGP(x, a) UE_LOG(LogTemp, Warning, TEXT(x), a);
#define RTLOGP2(x, a, b) UE_LOG(LogTemp, Warning, TEXT(x), a, b);
#define RTLOGP3(x, a, b, c) UE_LOG(LogTemp, Warning, TEXT(x), a, b, c);
#define RTLOGP4(x, a, b, c, e) UE_LOG(LogTemp, Warning, TEXT(x), a, b, c, e);
#define RTPRINTF(s, x) FText::FromString(FString::Printf(TEXT(s), x))

UCLASS()
class RADIANT_API UUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static UItemTooltip* InitTooltip(UUserWidget* Widget, TSubclassOf<UItemTooltip> TooltipClass, FName ItemName);

	UFUNCTION(BlueprintCallable)
	static FText ItemTypeToText(const EItemType Value);

	UFUNCTION(BlueprintCallable)
	static FName GetMappingNameFromSlot(EItemSlotID SlotID);

	UFUNCTION(BlueprintCallable)
	static FLinearColor ItemTypeToColor(EItemType Value);

	UFUNCTION(BlueprintCallable)
	static FLinearColor ClassTypeToColor(EClassType Value);
	
	UFUNCTION(BlueprintCallable)
	static FText ClassTypeToText(const EClassType Value);
	
	UFUNCTION(BlueprintCallable)
	static FVector GetMousePosition(class UObject* WorldContext, TArray<AActor*> IgnoredActors);

	UFUNCTION(BlueprintCallable)
	static FVector ProjectileDirection(FVector A, FVector B);

	UFUNCTION(BlueprintCallable)
	static FVector GetMouseVecFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintCallable)
	static FVector ClampVectorMaxDist(FVector A, FVector B, float MaxDist);

	UFUNCTION(BlueprintCallable)
	static AAvatar* GetHeroFromPlayerID(class UObject* WorldContext, int PlayerID);

	UFUNCTION(BlueprintCallable)
	static void ChangeQualitySetting(FString Name);

	static TSubclassOf<class AActor> GetBuildingType(EEnvironmentType Type);

	static void LogServerOrClient(class AActor* Actor);

	static struct FItemData* GetItemDataFromName(const FName& Name, const FString& ContextString = FString(""));

	static struct FTooltipData* GetTooltipDataFromName(const FName& Name, const FString& ContextString = FString(""));

	static class UDataTable* GetItemDataTable();
};

#define BindStatsPanelStat(StatName) \
PS->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PS->GetAttributeSet()->Get##StatName##Attribute()).AddUObject( \
	this, &UPlayerStatsPanel::Update##StatName); \
StatName##Text->SetText(RTPRINTF("%.0f", PS->GetAttributeSet()->Get##StatName())); \
if(FTooltipData* TooltipData = UUtil::GetTooltipDataFromName(FName(#StatName))) \
{ \
	StatName##Icon->SetBrushFromTexture(TooltipData->Icon); \
	StatName##Icon->SetColorAndOpacity(TooltipData->Color); \
}
