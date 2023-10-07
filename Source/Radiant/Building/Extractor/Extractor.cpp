#include "Extractor.h"
#include "GAS/AttributeSets/RTBaseAttributeSet.h"
#include "Items/WorldItem.h"
#include "Net/UnrealNetwork.h"
#include "Util/TempGridActor.h"

AExtractor::AExtractor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	AActor::SetReplicateMovement(false);
	
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	ExtractorAttributeSet = CreateDefaultSubobject<URTBaseAttributeSet>(TEXT("ExtractorAttributeSet"));
}

void AExtractor::BeginPlay()
{
	Super::BeginPlay();
}

void AExtractor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority())
	{
		ExtractionTimer += DeltaSeconds;
		
		if(ExtractionTimer >= ExtractionRate)
		{
			ExtractionTimer = 0.f;
			Extract();
		}
	}
}

void AExtractor::Extract()
{	
	if(IsValid(WorldItem))
	{
		WorldItem->UpdateAmount(WorldItem->GetAmount() + ExtractionRate);
	}
	else
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		WorldItem = GetWorld()->SpawnActorDeferred<AWorldItem>(WorldItemClass, FTransform(GetActorLocation() + FVector(100.f, 0.f, 0.f)), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		WorldItem->InitItem(FName("Radianite"), ExtractionRate);
		WorldItem->FinishSpawning(FTransform(GetActorLocation() + FVector(100.f, 0.f, 0.f)));
	}
}

void AExtractor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExtractor, Radianite);
}
