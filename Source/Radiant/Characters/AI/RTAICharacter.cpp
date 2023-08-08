// Copyright Radiant Studios


#include "Characters/AI/RTAICharacter.h"

#include "Combat/PickUp.h"
#include "Components/WidgetComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/NPCAttributeSet.h"
#include "Items/WorldItem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/AIInfoBar.h"

ARTAICharacter::ARTAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>(FName("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UNPCAttributeSet>(FName("AttributeSet"));
	
	AttributeSet->InitMaxHealth(40.f);
	AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	AttributeSet->InitMovementSpeed(200.f);
	AttributeSet->InitAttackDamage(15.f);
	AttributeSet->InitLevel(1.f);

	OverHeadInfoBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("InfoBar");
	OverHeadInfoBarWidgetComponent->SetupAttachment(RootComponent);
	OverHeadInfoBarWidgetComponent->SetVisibility(true);
}

UAbilitySystemComponent* ARTAICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UNPCAttributeSet* ARTAICharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ARTAICharacter::SetIsDead(const bool NewIsDead)
{
	OnUnitDied.Broadcast();
	if(NewIsDead)
	{
		GetWorld()->SpawnActor<AWorldItem>(DropClass, GetActorTransform());
		M_NotifyOnDeath();
		Destroy();
	}
}

void ARTAICharacter::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthPercent(OnAttributeChangeData.NewValue / AbilitySystemComponent->GetSet<UNPCAttributeSet>()->GetMaxHealth());
	}
}

void ARTAICharacter::BeginPlay()
{
	Super::BeginPlay();
	OverHeadInfoBar = Cast<UAIInfoBar>(OverHeadInfoBarWidgetComponent->GetWidget());
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthPercent(1.f);
		OverHeadInfoBar->SetLevel(1);
	}
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ARTAICharacter::OnHealthChanged);
}


