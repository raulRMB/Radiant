// Copyright Radiant Studios


#include "Util/Util.h"
#include "EngineUtils.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Data/BuildingTypes.h"
#include "Data/GearData.h"
#include "Data/ItemData.h"
#include "Data/TooltipData.h"
#include "Player/Avatar.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameState.h"
#include "Player/RTPlayerState.h"
#include "UI/InGame/ItemTooltip.h"

struct FGameplayAbilityTargetDataHandle;

FText UUtil::ItemTypeToText(const EItemType Value)
{
	switch (Value)
	{
		case EItemType::Ability:
			return FText::FromString("Ability");
		case EItemType::Gear:
			return FText::FromString("Gear");
		case EItemType::Material:
			return FText::FromString("Material");
		case EItemType::Weapon:
			return FText::FromString("Weapon");
	}
	return FText::FromString("");
}

FName UUtil::GetMappingNameFromSlot(const EItemSlotID SlotID)
{
	switch(SlotID)
	{
		case EItemSlotID::HotBarFirst:
			return FName("Ability 1");
		case EItemSlotID::HotBarTwo:
			return FName("Ability 2");
		case EItemSlotID::HotBarThree:
			return FName("Ability 3");
		case EItemSlotID::HotBarFour:
			return FName("Ability 4");
		case EItemSlotID::HotBarFive:
			return FName("Ability 5");
		case EItemSlotID::HotBarLast:
			return FName("Ability 6");
		default:
			return FName("");
	}
}

FName UUtil::GetDefaultKeybindFromSlot(const EItemSlotID SlotID)
{
	switch(SlotID)
	{
		case EItemSlotID::HotBarFirst:
			return FName("Q");
		case EItemSlotID::HotBarTwo:
			return FName("W");
		case EItemSlotID::HotBarThree:
			return FName("E");
		case EItemSlotID::HotBarFour:
			return FName("R");
		case EItemSlotID::HotBarFive:
			return FName("D");
		case EItemSlotID::HotBarLast:
			return FName("F");
		default:
			return FName("");
	}
}

FLinearColor UUtil::ItemTypeToColor(const EItemType Value)
{
	switch(Value)
	{
		case EItemType::Ability:
			return FLinearColor(0.0f, 1.0f, 1.0f, 1.0f);
		case EItemType::Gear:
			return FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
		case EItemType::Material:
			return FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
		case EItemType::Weapon:
			return FLinearColor(0.72f, 0.15f, 0.52f, 1.0f);
	}
	return FLinearColor::White;
}

FLinearColor UUtil::ClassTypeToColor(const EClassType Value)
{
	switch(Value)
	{
	case EClassType::Support:
		return FLinearColor(0.58f, 0.24f, 0.6f, 1.0f);
	case EClassType::Tank:
		return FLinearColor(0.91f, 0.57f, 0.2f, 1.0f);
	case EClassType::Damage:
		return FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
	case EClassType::General:
		return FLinearColor::White;
	}
	return FLinearColor::White;
}

FText UUtil::ClassTypeToText(const EClassType Value)
{
	switch (Value)
	{
		case EClassType::Damage:
			return FText::FromString("Damage");
		case EClassType::General:
			return FText::FromString("General");
		case EClassType::Support:
			return FText::FromString("Support");
		case EClassType::Tank:
			return FText::FromString("Tank");
	}
	return FText::FromString("");
}

UItemTooltip* UUtil::InitTooltip(UUserWidget* Widget, TSubclassOf<UItemTooltip> TooltipClass, FName ItemName)
{
	UItemTooltip* Tooltip = CreateWidget<UItemTooltip>(Widget, TooltipClass);
	if(FItemData* ItemData = GetItemDataFromName(ItemName))
	{
		if(ItemData->GearData)
		{
			for(TSubclassOf<UGameplayEffect>& EffectClass : ItemData->GearData->GameplayEffects)
			{
				if(UGameplayEffect* Effect = EffectClass.GetDefaultObject())
				{
					FTooltipStatInfo StatInfo;
					for(FGameplayModifierInfo& Modifier : Effect->Modifiers)
					{
						FName Name = FName(Modifier.Attribute.AttributeName);
						StatInfo.CellType = Name;
						float Magnitude;						
						Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Magnitude);
						StatInfo.ModOp = Modifier.ModifierOp;
						Magnitude = FMath::Abs(Magnitude);
						StatInfo.Value = Magnitude;
						Tooltip->AddStatInfo(StatInfo);
					}
				}
			}
		}
		Tooltip->Init(ItemData, FText::FromString(ItemName.ToString()));
		return Tooltip;
	}
	return nullptr;
}

FVector UUtil::GetMousePosition(class UObject* WorldContext, TArray<AActor*> IgnoredActors)
{
	if(!IsValid(WorldContext) || !IsValid(WorldContext->GetWorld()))
	{
		return FVector::ZeroVector;
	}
	
	FVector2D MousePosition;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetMousePosition(MousePosition);
	}

	// Get the world position
	FVector WorldPosition;
	FVector WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(WorldContext->GetWorld()->GetFirstPlayerController(), MousePosition,
	                                         WorldPosition, WorldDirection);

	// Get the hit result
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	for (AActor* Actor : IgnoredActors)
	{
		CollisionQueryParams.AddIgnoredActor(Actor);
	}
	WorldContext->GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition,
	                                                   WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel2,
	                                                   CollisionQueryParams);

	return HitResult.Location;
}

FVector UUtil::ProjectileDirection(FVector A, FVector B)
{
	return FVector(B.X - A.X, B.Y - A.Y, 0).GetSafeNormal();
}

FVector UUtil::GetMouseVecFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData)
{
	if (TargetData.Data.Num() > 0)
	{
		FGameplayAbilityTargetData* Data = TargetData.Data[0].Get();
		if (Data)
		{
			const FVector Mouse = Data->GetEndPointTransform().GetTranslation();
			return Mouse;
		}
	}

	return FVector::ZeroVector;
}

FVector UUtil::ClampVectorMaxDist(FVector A, FVector B, float MaxDist)
{
	FVector Vector = B - A;
	if (Vector.Size() > MaxDist)
	{
		Vector = Vector.GetSafeNormal() * MaxDist;
		Vector += A;
		return Vector;
	}

	return B;
}

AAvatar* UUtil::GetHeroFromPlayerID(class UObject* WorldContext, int PlayerID)
{
	auto GameState = WorldContext->GetWorld()->GetGameState<ARTGameState>();

	for (auto PlayerState : GameState->PlayerArray)
	{
		auto RTPlayerState = Cast<ARTPlayerState>(PlayerState);
		if (RTPlayerState->GetPlayerId() == PlayerID)
		{
			return Cast<AAvatar>(RTPlayerState->GetPawn());
		}
	}

	return nullptr;
}

void UUtil::ChangeQualitySetting(FString Name)
{
	int32 Setting = 0;
	if (Name == "Low")
	{
		Setting = 0;
	}
	else if (Name == "Medium")
	{
		Setting = 1;
	}
	else if (Name == "High")
	{
		Setting = 2;
	}
	else if (Name == "Ultra")
	{
		Setting = 3;
	}
	else if (Name == "Max")
	{
		Setting = 4;
	}
	if (GEngine->GetGameUserSettings()->GetOverallScalabilityLevel() != Setting)
	{
		GEngine->GetGameUserSettings()->SetOverallScalabilityLevel(Setting);
		GEngine->GetGameUserSettings()->ApplySettings(false);
	}
}

TSubclassOf<AActor> UUtil::GetBuildingType(EEnvironmentType Type)
{
	TArray<UObject*> DataAssets;
	FindOrLoadAssetsByPath(TEXT("/Game/Data/EnvironmentData/"), DataAssets, EngineUtils::ATL_Regular);
	if (UBuildingTypes* BuildingTypes = Cast<UBuildingTypes>(DataAssets[0]))
	{
		return BuildingTypes->EnvironmentTypes[Type];
	}
	return nullptr;
}

void UUtil::LogServerOrClient(AActor* Actor)
{
	if (Actor->HasAuthority())
	{
		RTLOG("I am the server")
	}
	else
	{
		RTLOG("I am the client")
	}
}

FItemData* UUtil::GetItemDataFromName(const FName& Name, const FString& ContextString)
{
	TArray<UObject*> Objects;
	FindOrLoadAssetsByPath(TEXT("/Game/Data/ItemData/"), Objects, EngineUtils::ATL_Regular);
	if (Objects.Num() > 0)
	{
		if (UDataTable* ItemTable = Cast<UDataTable>(Objects[0]))
		{
			if (FItemData* ItemData = ItemTable->FindRow<FItemData>(Name, ContextString))
			{
				return ItemData;
			}
		}
	}
	return nullptr;
}

FTooltipData* UUtil::GetTooltipDataFromName(const FName& Name, const FString& ContextString)
{
	TArray<UObject*> Objects;
	FindOrLoadAssetsByPath(TEXT("/Game/Data/TooltipData/"), Objects, EngineUtils::ATL_Regular);
	if (Objects.Num() > 0)
	{
		if (UDataTable* ItemTable = Cast<UDataTable>(Objects[0]))
		{
			if (FTooltipData* ItemData = ItemTable->FindRow<FTooltipData>(Name, ContextString))
			{
				return ItemData;
			}
		}
	}
	return nullptr;
}

UDataTable* UUtil::GetItemDataTable()
{
	TArray<UObject*> Objects;
	FindOrLoadAssetsByPath(TEXT("/Game/Data/ItemData/"), Objects, EngineUtils::ATL_Regular);
	if (Objects.Num() > 0)
	{
		if (UDataTable* ItemTable = Cast<UDataTable>(Objects[0]))
		{
			return ItemTable;
		}
	}
	return nullptr;
}
