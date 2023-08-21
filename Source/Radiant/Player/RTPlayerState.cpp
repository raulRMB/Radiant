// Copyright Radiant Studios


#include "Player/RTPlayerState.h"
#include "InventoryComponent.h"
#include "RTPlayerController.h"
#include "Data/AbilityDataAsset.h"
#include "Data/GearData.h"
#include "Data/ItemData.h"
#include "Data/WeaponDataAsset.h"
#include "Event/EventBroker.h"
#include "Modes/Base/RTGameState.h"
#include "Modes/Base/RTGameMode.h"
#include "Player/Avatar.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/RTHUD.h"
#include "UI/Client/ClientSubsystem.h"
#include "Util/Util.h"

void ARTPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTPlayerState, Target);
	DOREPLIFETIME(ARTPlayerState, TeamId);
	DOREPLIFETIME(ARTPlayerState, Username);
	DOREPLIFETIME(ARTPlayerState, InnateAbilities);
	DOREPLIFETIME(ARTPlayerState, bIsDead);
	DOREPLIFETIME(ARTPlayerState, CurrentClass);
}

FVector ARTPlayerState::GetCarrierLocation() const
{
	if (GetRTController())
	{
		return GetRTController()->GetCarrierLocation();
	}
	return FVector::ZeroVector;
}

UInventoryComponent* ARTPlayerState::GetInventory() const
{
	if (GetRTController())
	{
		return GetRTController()->GetInventory();
	}
	return nullptr;
}

void ARTPlayerState::RemoveAbility(FGameplayAbilitySpecHandle Handle)
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->SetRemoveAbilityOnEnd(Handle);
	}
}

ARTPlayerController* ARTPlayerState::GetRTController() const
{
	return Cast<ARTPlayerController>(GetPlayerController());
}

void ARTPlayerState::BeginPlay()
{
	Super::BeginPlay();

	AttributeSet->SetArmor(100.f);
}

void ARTPlayerState::OnRadianiteChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	OnUpdateRadianite.Broadcast(OnAttributeChangeData.NewValue);
}

ARTPlayerState::ARTPlayerState()
{
	bReplicates = true;
	bReplicateUsingRegisteredSubObjectList = true;

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<URTAvatarAttributeSet>(TEXT("AttributeSet"));

	NetUpdateFrequency = 100.f;

	AttributeSet->InitMaxHealth(500.f);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	AttributeSet->InitMaxMana(150.f);
	AttributeSet->InitMana(AttributeSet->GetMaxMana());
	AttributeSet->InitMovementSpeed(500.f);
	AttributeSet->InitAttackDamage(40.f);
	AttributeSet->InitArmor(0.f);
	AttributeSet->InitMaxRespawnTime(10.f);
	AttributeSet->InitCurrentRespawnTime(AttributeSet->GetMaxRespawnTime());
	AttributeSet->InitXP(0.f);
	AttributeSet->InitMaxXP(100.f);
	AttributeSet->InitLevel(1.f);
	AttributeSet->InitRadianite(500.f);
	AttributeSet->InitAttackSpeed(1.f);
	AttributeSet->InitAttackRange(0.f);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetRadianiteAttribute()).AddUObject(
		this, &ARTPlayerState::OnRadianiteChanged);
}

void ARTPlayerState::SetPlayerStats()
{
	AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	AttributeSet->SetMana(AttributeSet->GetMaxMana());
	AttributeSet->SetMovementSpeed(600.f);
	AttributeSet->SetCurrentRespawnTime(AttributeSet->GetMaxRespawnTime());
}

void ARTPlayerState::OnRep_UsernameChanged()
{
	GetPawn<AAvatar>()->SetOverheadBarText(Username);
}

void ARTPlayerState::OnRep_CurrentClass()
{
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		UEventBroker::Get(this)->CurrentClassChanged.Broadcast(CurrentClass);
	}
}

void ARTPlayerState::S_SetTarget_Implementation(AActor* NewTargetId)
{
	Target = NewTargetId;
}

UAbilitySystemComponent* ARTPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

URTAvatarAttributeSet* ARTPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

FString ARTPlayerState::GetUsername()
{
	return Username;
}

#pragma region Killable

void ARTPlayerState::S_SetIsDead_Implementation(bool bNewIsDead)
{
	bIsDead = bNewIsDead;
	FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag("States.Dead");
	GetAbilitySystemComponent()->SetLooseGameplayTagCount(DeadTag, bNewIsDead ? 1 : 0);
	GetAbilitySystemComponent()->SetReplicatedLooseGameplayTagCount(DeadTag, bNewIsDead ? 1 : 0);
	if(bIsDead && HasAuthority())
	{
		StartRespawnTimer();
	}
}

void ARTPlayerState::SetIsDead(const bool NewIsDead)
{
	IKillable::SetIsDead(NewIsDead);
	S_SetIsDead(NewIsDead);
}


void ARTPlayerState::StartRespawnTimer()
{
	if(GetWorld())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ARTPlayerState::Respawn, AttributeSet->GetCurrentRespawnTime(), false);
	}
}

void ARTPlayerState::Respawn()
{
	GetRTGM()->Respawn(GetRTPC());
	AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	AttributeSet->SetMana(AttributeSet->GetMaxMana());
}

#pragma endregion Killable

#pragma region ConvenienceGetters


ARTPlayerController* ARTPlayerState::GetRTPC() const
{
	return Cast<ARTPlayerController>(GetPlayerController());
}


AAvatar* ARTPlayerState::GetAvatar() const
{
	return GetPawn<AAvatar>();
}

ARTGameMode* ARTPlayerState::GetRTGM() const
{
	if(GetWorld())
	{
		return GetWorld()->GetAuthGameMode<ARTGameMode>();
	}
	return nullptr;
}

ARTGameState* ARTPlayerState::GetRTGS() const
{
	if(GetWorld())
	{
		return GetWorld()->GetGameState<ARTGameState>();
	}
	return nullptr;
}

#pragma endregion ConvenienceGetters


void ARTPlayerState::GameReady_Implementation()
{
	auto SS = GetGameInstance()->GetSubsystem<UClientSubsystem>();
	if (SS)
	{
		FString CUsername = GetGameInstance()->GetSubsystem<UClientSubsystem>()->Username;
		if (!CUsername.IsEmpty())
		{
			SetUsername(CUsername);
		}
	}
	ARTPlayerController* PC = Cast<ARTPlayerController>(GetPlayerController());
	if(PC)
	{
		PC->GetHUD<ARTHUD>()->HideLoadScreen();
		PC->GetHUD<ARTHUD>()->BindUIItems();
	}
	UEventBroker::Get(this)->GameIsReady.Broadcast();
}

void ARTPlayerState::S_BuyAbility_Implementation(const FName& AbilityName, int32 Amount)
{
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(AbilityName, FString("BuyAbility"));
	// if(ItemData->AbilityData->Price * Amount > AttributeSet->GetRadianite())
	// {
	// 	Amount = AttributeSet->GetRadianite() / ItemData->AbilityData->Price;
	// }
	if (!ItemData || Amount <= 0 || InnateAbilities.Contains(ItemData->AbilityData))
	{
		return;
	}
	GetInventory()->AddItem(AbilityName, ItemData, Amount);
	// AttributeSet->SetRadianite(AttributeSet->GetRadianite() - ItemData->AbilityData->Price * Amount);
}

void ARTPlayerState::S_EquipGear_Implementation(const FName& WeaponName)
{
	const FItemData* GearItemData = UUtil::GetItemDataFromName(WeaponName);
	const UWeaponDataAsset* WeaponDataAsset = Cast<UWeaponDataAsset>(GearItemData->AbilityData);
	const UGearData* GearData = Cast<UGearData>(GearItemData->GearData);
	
	if(!GearItemData)
	{
		return;
	}
	if(WeaponDataAsset)
	{	
		if(WeaponAbilityHandle.IsValid())
		{
			AbilitySystemComponent->SetRemoveAbilityOnEnd(WeaponAbilityHandle);
		}
		WeaponAbilityHandle = AbilitySystemComponent->GiveAbility(WeaponDataAsset->Ability.GetDefaultObject());
		CurrentClass = GearItemData->ClassType;
	}
	
	if(GearData)
	{
		for(const TSubclassOf<UGameplayEffect>& EffectClass : GearData->GameplayEffects)
		{
			UGameplayEffect* Effect = EffectClass.GetDefaultObject();
			AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect, 1.f, AbilitySystemComponent->MakeEffectContext());
		}
	}
}

void ARTPlayerState::S_UnequipWeapon_Implementation()
{
	if(WeaponAbilityHandle.IsValid())
	{
		AttributeSet->SetAttackRange(200.f);
		AbilitySystemComponent->SetRemoveAbilityOnEnd(WeaponAbilityHandle);
	}
}

void ARTPlayerState::S_UnequipGear_Implementation(const FName& GearName)
{
	if(FItemData* ItemData = UUtil::GetItemDataFromName(GearName))
	{
		if(const UGearData* GearData = Cast<UGearData>(ItemData->GearData))
		{
			if(ItemData->ItemType == EItemType::Weapon)
			{
				CurrentClass = EClassType::General;
			}
			for(const TSubclassOf<UGameplayEffect>& EffectClass : GearData->GameplayEffects)
			{
				GetAbilitySystemComponent()->RemoveActiveGameplayEffectBySourceEffect(EffectClass, GetAbilitySystemComponent());
			}
		}
	}
}

FGameplayAbilitySpecHandle ARTPlayerState::GiveAbility(const FItemData* ItemData) const
{
	if(ItemData->bIsGear || !ItemData->AbilityData)
		return FGameplayAbilitySpecHandle();
	FGameplayAbilitySpec AbilitySpec = ItemData->AbilityData->Ability.GetDefaultObject();
	return AbilitySystemComponent->GiveAbility(AbilitySpec);
}

void ARTPlayerState::S_GiveItem_Implementation(const FName& ItemName, int32 Amount)
{
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemName, FString("BuyAbility"));
	if (!ItemData || Amount <= 0)
	{
		return;
	}
	GetInventory()->AddItem(ItemName, ItemData, Amount);
}

TArray<class UAbilityDataAsset*> ARTPlayerState::GetInnateAbilities() const
{
	return InnateAbilities;
}

void ARTPlayerState::SetUsername_Implementation(const FString& String)
{
	Username = String;
}
