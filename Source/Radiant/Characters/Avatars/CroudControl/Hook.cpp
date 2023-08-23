// Copyright Radiant Studios


#include "Characters/Avatars/CroudControl/Hook.h"
#include "CableComponent.h"
#include "GAHook.h"
#include "Net/UnrealNetwork.h"

AHook::AHook()
{
	PrimaryActorTick.bCanEverTick = true;
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));
	CableComponent->SetupAttachment(RootComponent);
	CableComponent->bAttachStart = true;
}


void AHook::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHook, Hooker);
}

void AHook::AttachToHooker(AActor* Actor)
{
	Hooker = Actor;
}

void AHook::OverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OverlapStart(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(HookAbility)
	{
		HookAbility->OnTargetHit(OtherActor);
	}
}

void AHook::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	Super::OverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AHook::LifeSpanExpired()
{
	if(HookAbility)
	{
		HookAbility->OnFinished();
	}
	Super::LifeSpanExpired();
}

void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(CableComponent)
	{
		FVector Dir = GetActorLocation() - Hooker->GetActorLocation();
		Dir = Dir.RotateAngleAxis(-GetActorRotation().Yaw + 180.f, FVector::UpVector);
		CableComponent->EndLocation = Dir;
		CableComponent->TileMaterial = FVector::Dist(Hooker->GetActorLocation(), GetActorLocation());
	}
}
