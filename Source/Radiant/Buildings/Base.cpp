// Copyright Radiant Studios


#include "Buildings/Base.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/BuildingAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameMode.h"

void ABase::OnBaseHealthChanged(const FOnAttributeChangeData& Data)
{
	if(HasAuthority())
	{
		if(Data.NewValue <= 0.f)
		{
			if(ARTGameMode* GM = Cast<ARTGameMode>(UGameplayStatics::GetGameMode(this)))
			{
				if(TeamId == ETeamId::Red)
				{
					GM->SetMatchOver(ETeamId::Blue);
				}
				else if(TeamId == ETeamId::Blue)
				{
					GM->SetMatchOver(ETeamId::Red);
				}
				else
				{
					GM->SetMatchOver(ETeamId::Neutral);
				}
			}
		}
	}
}

void ABase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Gem->AddLocalRotation(FRotator(0.f, 90.f * DeltaTime, 0.f));
}

void ABase::BeginPlay()
{
	Super::BeginPlay();
	Gem = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Diamond")));
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ABase::OnBaseHealthChanged);
}
