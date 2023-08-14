#pragma once

#include "CoreMinimal.h"
#include "../Building.h"
#include "Extractor.generated.h"

UCLASS()
class AExtractor : public ABuilding
{
	GENERATED_BODY()
	
	UPROPERTY(Replicated)
	float Radianite = 0;

	UPROPERTY(EditAnywhere)
	float ExtractionRate = 1.f;
	float ExtractionTimer = 0.f;

	UPROPERTY()
	class AVein* Vein;

	UPROPERTY()
	class UExtractorAttributeSet* ExtractorAttributeSet;
	
public:
	AExtractor();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	void Extract() const;

	void SetVein(class AVein* NewVein) { Vein = NewVein; }
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
