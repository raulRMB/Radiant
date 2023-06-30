// Copyright Radiant Studios


#include "Characters/Avatars/CroudControl/Jook.h"

#include "GAJook.h"

void AJook::OverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OverlapStart(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(JookAbility)
	{
		JookAbility->OnTargetHit(OtherActor);
	}
}

void AJook::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	Super::OverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
