// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/ItemSlotID.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Util/Managers/Grid/GridManager.h"
#include "Util.generated.h"

#define ECC_CursorTarget ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Ground ECollisionChannel::ECC_GameTraceChannel2
#define ECC_Artillery ECollisionChannel::ECC_GameTraceChannel3
#define ECC_Floor ECollisionChannel::ECC_GameTraceChannel4
#define ECC_Vision ECollisionChannel::ECC_GameTraceChannel5

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

DECLARE_MULTICAST_DELEGATE_OneParam(FDeathNotifySignature, class AActor* /*Unit*/);

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
	static ETeamId GetLocalPlayerTeamId(class UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable)
	static FName GetMappingNameFromSlot(EItemSlotID SlotID);

	UFUNCTION(BlueprintCallable)
	static FName GetDefaultKeybindFromSlot(EItemSlotID SlotID);

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

	static void ReadTextureColors(class UTexture2D* Texture, TArray<FColor>& Colors);

#pragma region Grid
	static void CheckVisible(const TArray<EEnvironmentType>& Grid, class UTexture2D* TargetTexture, const FIntVector2& Position);
private:
	static void SetValid(TArray<TArray<bool>>& MapState, TArray<TArray<bool>>& VisionState, int X, int Y, int OX, int oY);
	static bool CheckValid(TArray<TArray<bool>>& Grid, int X, int Y, int OX, int OY);
	static int GetDir(int O, int T); 
#pragma endregion Grid
public:
	
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
