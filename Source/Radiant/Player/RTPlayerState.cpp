// Copyright Radiant Studios


#include "Player/RTPlayerState.h"
#include "InventoryComponent.h"
#include "RTPlayerController.h"
#include "TimerManager.h"
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
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTPlayerStart.h"
#include "Net/UnrealNetwork.h"
#include "UI/Minimap.h"
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

void ARTPlayerState::GameEnding_Implementation(ETeamId Won)
{
	GetPlayerController()->GetHUD<ARTHUD>()->ShowEndScreen(Won == TeamId);
	UGameplayStatics::PlaySound2D(GetWorld(), Won == TeamId ? WinSound : LoseSound);
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

void ARTPlayerState::OnRadiantiteChanged(double Radianite, unsigned Level)
{
	if(Level > TeamLevel)
	{
		TeamLevel = Level;
		UpdateGearActiveEffectLevel();
	}
}

void ARTPlayerState::BeginPlay()
{
	Super::BeginPlay();

	AttributeSet->SetArmor(100.f);

	ARTGameState* GS = GetWorld()->GetGameState<ARTGameState>();
	if(GetTeamId() == ETeamId::Blue)
	{
		GS->OnBlueRadianiteChangedDelegate.AddUObject(this, &ARTPlayerState::OnRadiantiteChanged);
	}
	else
	{
		GS->OnRedRadianiteChangedDelegate.AddUObject(this, &ARTPlayerState::OnRadiantiteChanged);
	}
}

void ARTPlayerState::UpdateGearActiveEffectLevel()
{
	uint32 Level = GetWorld()->GetGameState<ARTGameState>()->GetTeamLevel(GetTeamId());
	for(FActiveGameplayEffectHandle Handle : ActiveGearEffectHandles)
	{
		GetAbilitySystemComponent()->SetActiveGameplayEffectLevel(Handle, Level); 
	}
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
}

void ARTPlayerState::SetPlayerStats()
{
	AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	AttributeSet->SetMana(AttributeSet->GetMaxMana());
	AttributeSet->SetMovementSpeed(600.f);
	AttributeSet->SetCurrentRespawnTime(AttributeSet->GetMaxRespawnTime());
}

void ARTPlayerState::SetTarget(AActor* NewTarget)
{
	Target = NewTarget;
	S_SetTarget(NewTarget);
}

void ARTPlayerState::OnRep_UsernameChanged()
{
	GetPawn<AAvatar>()->SetOverheadBarText(Username);
}

void ARTPlayerState::OnRep_CurrentClass()
{
	if(GetPawn())
	{
		if(GetPawn()->GetLocalRole() == ROLE_AutonomousProxy)
		{
			UEventBroker::Get(this)->CurrentClassChanged.Broadcast(CurrentClass);
		}
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

void ARTPlayerState::AddHandleToName(FGameplayAbilitySpecHandle Handle, FName Name)
{
	HandleToItemName.Add(Handle, Name);
}

const FGameplayAbilitySpecHandle* ARTPlayerState::FindHandle(FName Name)
{
	return HandleToItemName.FindKey(Name);
}

FName ARTPlayerState::GetItemNameFormHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (HandleToItemName.Contains(Handle))
	{
		return HandleToItemName[Handle];
	}
	return NAME_None;
}

void ARTPlayerState::S_EquipAbility_Implementation(const FName& ItemName)
{
	FItemData* ItemData = UUtil::GetItemDataFromName(ItemName);
	if (ItemData->AbilityData && !FindHandle(ItemName))
	{
		FGameplayAbilitySpecHandle Handle = GiveAbility(ItemData);
		AddHandleToName(Handle, ItemName);
	}
}

void ARTPlayerState::S_UnequipAbility_Implementation(const FName& ItemName)
{
	if(const auto Handle = FindHandle(ItemName))
	{
		RemoveAbility(*Handle);
		HandleToItemName.Remove(*Handle);
	}
}

void ARTPlayerState::S_BuyAbility_Implementation(const FName& AbilityName, int32 Amount)
{
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(AbilityName, FString("BuyAbility"));
	if (!ItemData || Amount <= 0 || InnateAbilities.Contains(ItemData->AbilityData))
	{
		return;
	}
	GetInventory()->AddItem(AbilityName, ItemData, Amount);
}

void ARTPlayerState::S_EquipGear_Implementation(const FName& WeaponName)
{
	S_UnequipGear_Implementation(WeaponName);
	if(const FItemData* GearItemData = UUtil::GetItemDataFromName(WeaponName))
	{
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
			if(auto Avatar = GetAvatar())
			{
				Avatar->M_SwitchMesh(CurrentClass);
			}
		}
	
		if(GearData)
		{
			for(const TSubclassOf<UGameplayEffect>& EffectClass : GearData->GameplayEffects)
			{
				float Level = GetWorld()->GetGameState<ARTGameState>()->GetTeamLevel(GetTeamId());
				UGameplayEffect* Effect = EffectClass.GetDefaultObject();
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect, Level, AbilitySystemComponent->MakeEffectContext());
				if(ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					ActiveGearEffectHandles.AddUnique(ActiveGameplayEffectHandle);
				}
			}
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
				for(FActiveGameplayEffectHandle Handle : ActiveGearEffectHandles)
				{
					if(const FActiveGameplayEffect* Spec = AbilitySystemComponent->GetActiveGameplayEffect(Handle))
					{
						if(Spec->Spec.Def->GetClass() == EffectClass)
						{
							AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
							ActiveGearEffectHandles.Remove(Handle);
							break;
						}
					}
					else
					{
						ActiveGearEffectHandles.Remove(Handle);
						break;
					}
				}
			}
		}
	}
}

FGameplayAbilitySpecHandle ARTPlayerState::GiveAbility(const FItemData* ItemData) const
{
	if(ItemData->ItemType == EItemType::Gear || !ItemData->AbilityData)
		return FGameplayAbilitySpecHandle();
	FGameplayAbilitySpec AbilitySpec = ItemData->AbilityData->Ability.GetDefaultObject();
	return AbilitySystemComponent->GiveAbility(AbilitySpec);
}

void ARTPlayerState::S_GiveItemUnchecked_Implementation(const FName& ItemName, int32 Amount)
{
	GetInventory()->AddItemUnchecked(ItemName, Amount);
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
