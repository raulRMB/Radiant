// Copyright Radiant Studios


#include "Characters/RTCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerController.h"
#include "Util/Util.h"
#include "Util/Managers/Grid/TeamGridManager.h"

void ARTCharacter::RemoveActorInVision(class AActor* Actor)
{
	ActorsInVision.Remove(Actor);
	TeamGridManager->RemoveActorFromActorsInVision(Actor);
}

void ARTCharacter::OnVisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ActorsInVisionRadius.AddUnique(OtherActor);
	if(ABuilding* Building = Cast<ABuilding>(OtherActor))
	{
		Building->OnDeathNotify.AddUObject(this, &ARTCharacter::RemoveActorInVision);
	}
}

void ARTCharacter::OnVisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ActorsInVisionRadius.Remove(OtherActor);
	TeamGridManager->DecrementActorFromActorsInVision(OtherActor);
	ActorsInVision.Remove(OtherActor);
}

void ARTCharacter::SetupVisionRadius() const
{
	VisionRadius->SetupAttachment(RootComponent);
}

ARTCharacter::ARTCharacter()
{
	bReplicates = true;

	VisionRadius = CreateDefaultSubobject<USphereComponent>(FName("VisionRadius"));

	PrimaryActorTick.bHighPriority = true;
}

void ARTCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTCharacter, TeamId);
	DOREPLIFETIME(ARTCharacter, bIsDead);
	DOREPLIFETIME(ARTCharacter, TeamGridManager);
}

void ARTCharacter::BeginPlay()
{
	Super::BeginPlay();

	ActorManager = GetGameInstance()->GetSubsystem<UActorManager>();
	
	
	if(HasAuthority())
	{
		VisionRadius->OnComponentBeginOverlap.AddDynamic(this, &ARTCharacter::OnVisionBeginOverlap);
		VisionRadius->OnComponentEndOverlap.AddDynamic(this, &ARTCharacter::OnVisionEndOverlap);
	}
	
	OnUnitDied.AddUObject(this, &ARTCharacter::OnDeath);
}

void ARTCharacter::OnDeath()
{
	if(DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
}

bool ARTCharacter::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	if(ViewTarget == this)
	{
		return true;
	}
	if(const ITeamMember* TeamMember = Cast<ITeamMember>(ViewTarget))
	{
		if(TeamMember->GetTeamId() == GetTeamId())
		{
			return true;
		}
	}
	if(const ARTPlayerController* TeamMember = Cast<ARTPlayerController>(RealViewer))
	{
		if(ATeamGridManager* NewTeamGridManager = ActorManager->GetTeamGridManager(TeamMember->GetTeamId()))
		{
			return NewTeamGridManager->IsTargetInVision(this);
		}
	}
	
	return false;
}

void ARTCharacter::CheckCanSeeTarget(AActor* Target)
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceMultiByChannel(HitResults, GetActorLocation(), Target->GetActorLocation(), ECC_Vision, CollisionQueryParams);
	if(HitResults.Num() <= 3)
	{
		for(FHitResult& HitResult : HitResults)
		{
			if(HitResult.GetActor() == Target && !ActorsInVision.Contains(Target))
			{
				ActorsInVision.Add(Target);
				TeamGridManager->AddActorToActorsInVision(Target);
			}
		}
	}
	else
	{
		if(ActorsInVision.Contains(Target))
		{
			ActorsInVision.Remove(Target);
			TeamGridManager->DecrementActorFromActorsInVision(Target);
		}
	}
}

void ARTCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority())
	{
		for(AActor* Actor : ActorsInVisionRadius)
		{
			CheckCanSeeTarget(Actor);
		}
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
