// Copyright Radiant Studios


#include "Combat/GameplayCues/GCAChangeSize.h"

#include "Characters/RTCharacter.h"

bool AGCAChangeSize::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if(ARTCharacter* Character = Cast<ARTCharacter>(MyTarget))
	{
		OriginalScale = Character->GetMesh()->GetRelativeScale3D();
		FVector NewScale = OriginalScale * SizeMultiplier;
		Character->GetMesh()->SetRelativeScale3D(NewScale);
	}
	
	return Super::OnActive_Implementation(MyTarget, Parameters);
}

bool AGCAChangeSize::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if(ARTCharacter* Character = Cast<ARTCharacter>(MyTarget))
	{
		Character->GetMesh()->SetRelativeScale3D(OriginalScale);
	}
	
	return Super::OnRemove_Implementation(MyTarget, Parameters);
}
