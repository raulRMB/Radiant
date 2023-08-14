#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ExtractorAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class UExtractorAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_Radianite)
	FGameplayAttributeData Radianite = 0;
	UPROPERTY(ReplicatedUsing=OnRep_ExtractionRate)
	FGameplayAttributeData ExtractionRate = 1.f;
	UPROPERTY(ReplicatedUsing=OnRep_Capacity)
	FGameplayAttributeData Capacity = 0.f;
	
public:
	ATTRIBUTE_ACCESSORS(UExtractorAttributeSet, Radianite)
	ATTRIBUTE_ACCESSORS(UExtractorAttributeSet, ExtractionRate)
	ATTRIBUTE_ACCESSORS(UExtractorAttributeSet, Capacity)

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UFUNCTION()
	void OnRep_Radianite(const FGameplayAttributeData& OldRadianite);

	UFUNCTION()
	void OnRep_ExtractionRate(const FGameplayAttributeData& OldExtractionRate);

	UFUNCTION()
	void OnRep_Capacity(const FGameplayAttributeData& OldCapacity);
};
