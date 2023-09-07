#include "Extractor.h"
#include "GAS/AttributeSets/Buildings/ExtractorAttributeSet.h"
#include "Items/WorldItem.h"
#include "Net/UnrealNetwork.h"
#include "Objectives/Vein.h"
#include "Util/Util.h"

AExtractor::AExtractor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	AActor::SetReplicateMovement(false);
	
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	ExtractorAttributeSet = CreateDefaultSubobject<UExtractorAttributeSet>(TEXT("ExtractorAttributeSet"));
}

void AExtractor::BeginPlay()
{
	Super::BeginPlay();

	ExtractorAttributeSet->InitCapacity(1000.f);
	ExtractorAttributeSet->InitExtractionRate(1.f);
	ExtractorAttributeSet->InitRadianite(0.f);
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
