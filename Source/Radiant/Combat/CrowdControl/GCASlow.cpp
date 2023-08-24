// Copyright Radiant Studios


#include "Combat/CrowdControl/GCASlow.h"
#include "Characters/RTCharacter.h"


bool AGCASlow::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if(ARTCharacter* Character = Cast<ARTCharacter>(MyTarget))
	{
		Character->GetMesh()->SetOverlayMaterial(SlowMaterial);
	}
	
	return Super::OnActive_Implementation(MyTarget, Parameters);
}

bool AGCASlow::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if(ARTCharacter* Character = Cast<ARTCharacter>(MyTarget))
	{
		if(Character->GetMesh()->GetOverlayMaterial() == SlowMaterial)
		{
			Character->GetMesh()->SetOverlayMaterial(nullptr);
		}
	}
	
	return Super::OnRemove_Implementation(MyTarget, Parameters);
}
