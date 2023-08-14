#include "ExtractorAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UExtractorAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UExtractorAttributeSet, Radianite, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExtractorAttributeSet, ExtractionRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UExtractorAttributeSet, Capacity, COND_None, REPNOTIFY_Always);
}

void UExtractorAttributeSet::OnRep_Radianite(const FGameplayAttributeData& OldRadianite)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExtractorAttributeSet, Radianite, OldRadianite);
}

void UExtractorAttributeSet::OnRep_ExtractionRate(const FGameplayAttributeData& OldExtractionRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExtractorAttributeSet, ExtractionRate, OldExtractionRate);
}

void UExtractorAttributeSet::OnRep_Capacity(const FGameplayAttributeData& OldCapacity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UExtractorAttributeSet, Capacity, OldCapacity);
}
