// Copyright Radiant Studios


#include "ARefinery.h"

#include "Components/SphereComponent.h"
#include "Items/WorldItem.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameState.h"
#include "Util/Util.h"

AARefinery::AARefinery()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
}


void AARefinery::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		GameState = Cast<ARTGameState>(UGameplayStatics::GetGameState(this));
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AARefinery::OnOverlapBegin);
	}
}

void AARefinery::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AWorldItem* WorldItem = Cast<AWorldItem>(OtherActor))
	{
		if(WorldItem->GetItemName() == "Radianite")
		{
			GameState->DepositRadianite(GetTeamId(), WorldItem->GetAmount());
			WorldItem->Destroy();
		}
	}
}
