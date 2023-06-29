// Copyright Radiant Studios


#include "Characters/AI/RTAICharacter.h"

#include "Combat/PickUp.h"
#include "Components/WidgetComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/NPCAttributeSet.h"
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
	AttributeSet->InitDamage(15.f);
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

void ARTAICharacter::Die()
{
	OnUnitDied.Broadcast();
	GetWorld()->SpawnActor<APickUp>(PickUpClass, GetActorTransform());
	Destroy();
}

void ARTAICharacter::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthPercent(OnAttributeChangeData.NewValue / AbilitySystemComponent->GetSet<UNPCAttributeSet>()->GetMaxHealth());
	}
}

// Called when the game starts or when spawned
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


