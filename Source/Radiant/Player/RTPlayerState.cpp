// Copyright Radiant Studios


#include "Player/RTPlayerState.h"

#include "InventoryComponent.h"
#include "RTPlayerController.h"
#include "Data/AbilityDataAsset.h"
#include "Data/ItemData.h"
#include "Engine/ActorChannel.h"
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
}

FVector ARTPlayerState::GetCarrierLocation() const
{
	if(GetRTController())
	{
		return GetRTController()->GetCarrierLocation();
	}
	return FVector::ZeroVector;
}

UInventoryComponent* ARTPlayerState::GetInventory() const
{
	if(GetRTController())
	{
		return GetRTController()->GetInventory();
	}
	return nullptr;
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
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetRadianiteAttribute()).AddUObject(this, &ARTPlayerState::OnRadianiteChanged);
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

void ARTPlayerState::S_BuyAbility_Implementation(const FName& AbilityName)
{
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(AbilityName, FString("BuyAbility"));
	if(AttributeSet->GetRadianite() < ItemData->AbilityData->Price || InnateAbilities.Contains(ItemData->AbilityData))
	{
		return;
	}
	int32 AbilityCount = GetInventory()->AddItem(AbilityName);

	if(AbilityCount == 1)
	{
		AbilitySystemComponent->GiveAbility(ItemData->AbilityData->Ability.GetDefaultObject());
	}
	
	AttributeSet->SetRadianite(AttributeSet->GetRadianite() - ItemData->AbilityData->Price);
}

TArray<class UAbilityDataAsset*> ARTPlayerState::GetInnateAbilities() const
{
	return InnateAbilities;
}

void ARTPlayerState::SetUsername_Implementation(const FString& String)
{
	Username = String;
}
