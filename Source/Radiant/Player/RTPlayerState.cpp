// Copyright Radiant Studios


#include "Player/RTPlayerState.h"
#include "InventoryComponent.h"
#include "RTPlayerController.h"
#include "Data/AbilityDataAsset.h"
#include "Data/ItemData.h"
#include "Modes/Base/RTGameState.h"
#include "Modes/Base/RTGameMode.h"
#include "Player/Avatar.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

void ARTPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTPlayerState, Target);
	DOREPLIFETIME(ARTPlayerState, TeamId);
	DOREPLIFETIME(ARTPlayerState, Username);
	DOREPLIFETIME(ARTPlayerState, InnateAbilities);
	DOREPLIFETIME(ARTPlayerState, bIsDead);
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

	AttributeSet->InitMaxHealth(100.f);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	AttributeSet->InitMaxMana(150.f);
	AttributeSet->InitMana(AttributeSet->GetMaxMana());
	AttributeSet->InitMovementSpeed(600.f);
	AttributeSet->InitDamage(15.f);
	AttributeSet->InitMaxRespawnTime(10.f);
	AttributeSet->InitCurrentRespawnTime(AttributeSet->GetMaxRespawnTime());
	AttributeSet->InitXP(0.f);
	AttributeSet->InitMaxXP(100.f);
	AttributeSet->InitLevel(1.f);
	AttributeSet->InitRadianite(0.f);
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

void ARTPlayerState::S_SetTarget_Implementation(AActor* NewTargetId)
{
	Target = NewTargetId;
}

UAbilitySystemComponent* ARTPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

URTAvatarAttributeSet* ARTPlayerState::GetAttributeSetBase() const
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


void ARTPlayerState::S_BuyAbility_Implementation(const FName& AbilityName)
{
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(AbilityName, FString("BuyAbility"));
	if (AttributeSet->GetRadianite() < ItemData->AbilityData->Price || InnateAbilities.Contains(ItemData->AbilityData))
	{
		return;
	}
	GetInventory()->AddItem(AbilityName, ItemData);

	AttributeSet->SetRadianite(AttributeSet->GetRadianite() - ItemData->AbilityData->Price);
}

FGameplayAbilitySpecHandle ARTPlayerState::GiveAbility(FItemData* ItemData)
{
	FGameplayAbilitySpec AbilitySpec = ItemData->AbilityData->Ability.GetDefaultObject();
	return AbilitySystemComponent->GiveAbility(AbilitySpec);
}

TArray<class UAbilityDataAsset*> ARTPlayerState::GetInnateAbilities() const
{
	return InnateAbilities;
}

void ARTPlayerState::SetUsername_Implementation(const FString& String)
{
	Username = String;
}
