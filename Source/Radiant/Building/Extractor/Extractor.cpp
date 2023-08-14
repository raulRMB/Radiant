#include "Extractor.h"
#include "GAS/AttributeSets/Buildings/ExtractorAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Objectives/Vein.h"

AExtractor::AExtractor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	AActor::SetReplicateMovement(false);

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

	ExtractionTimer += DeltaSeconds;
	
	if(Vein && ExtractionTimer >= ExtractionRate)
	{
		ExtractionTimer = 0.f;
		Extract();
	}
}

void AExtractor::Extract() const
{
	Vein->OnExtracted(ExtractionRate);
}

void AExtractor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExtractor, Radianite);
}
