// Copyright Radiant Studios


#include "Characters/RTCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARTCharacter::ARTCharacter()
{
	bReplicates = true;
}

void ARTCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTCharacter, TeamId);
	DOREPLIFETIME(ARTCharacter, bIsDead);
}

void ARTCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnUnitDied.AddUObject(this, &ARTCharacter::OnDeath);
}

void ARTCharacter::OnDeath()
{
	if(DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
}

void ARTCharacter::SetIsDead(const bool NewIsDead)
{
	IKillable::SetIsDead(NewIsDead);
	bIsDead = NewIsDead;
	if(NewIsDead)
	{
		M_NotifyOnDeath();
	}
}

FHookSocketInfo ARTCharacter::GetHookSocketInfo() const
{
	FHookSocketInfo SocketInfo;
	SocketInfo.SocketName = TEXT("HookSocket");
	SocketInfo.Component = GetMesh();
	return SocketInfo;
}

void ARTCharacter::M_NotifyOnDeath_Implementation()
{
	OnUnitDied.Broadcast();
}

void ARTCharacter::M_SetIgnoreWalls_Implementation(const bool bIgnoreWalls)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, bIgnoreWalls ? ECR_Ignore : ECR_Block);
}
