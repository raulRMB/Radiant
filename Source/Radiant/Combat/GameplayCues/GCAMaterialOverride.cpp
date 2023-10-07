// Copyright Radiant Studios


#include "Combat/GameplayCues/GCAMaterialOverride.h"
#include "Characters/RTCharacter.h"
#include "Components/SkeletalMeshComponent.h"

bool AGCAOverlayMaterial::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if(ARTCharacter* Character = Cast<ARTCharacter>(MyTarget))
	{
		Character->GetMesh()->SetOverlayMaterial(OverlayMaterial);
	}
	
	return Super::OnActive_Implementation(MyTarget, Parameters);
}

bool AGCAOverlayMaterial::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if(ARTCharacter* Character = Cast<ARTCharacter>(MyTarget))
	{
		if(Character->GetMesh()->GetOverlayMaterial() == OverlayMaterial)
		{
			Character->GetMesh()->SetOverlayMaterial(nullptr);
		}
	}
	
	return Super::OnRemove_Implementation(MyTarget, Parameters);
}
