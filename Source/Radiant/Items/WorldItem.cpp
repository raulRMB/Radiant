// Copyright Radiant Studios


#include "Items/WorldItem.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ItemInfo/WorldItemInfoWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerController.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"
#include "Util/Interfaces/Carrier.h"

AWorldItem::AWorldItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	PickUpRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpRadius"));
	PickUpRadius->SetupAttachment(Mesh);
	
	FX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FX"));
	FX->SetupAttachment(Mesh);

	NameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	NameWidget->SetupAttachment(Mesh);
}

bool AWorldItem::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	if(const ARTPlayerController* PC = Cast<ARTPlayerController>(RealViewer))
	{
		if(auto PS = PC->GetPlayerState<ARTPlayerState>())
		{
			return IsVisibleForTeam(PS->GetTeamId());
		}
	}
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

bool AWorldItem::IsVisibleForTeam(const ETeamId TargetTeamId) const
{
	if(AGridManager* GM = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(this, GridManager->StaticClass())))
	{
		return GM->IsTargetVisibleForTeam(this, TargetTeamId);
	}
	return false;
}

void AWorldItem::InitItem(FName NewItemName, uint32 NewAmount)
{
	ItemName = NewItemName;
	Amount = NewAmount;
}

void AWorldItem::UpdateAmount(uint32 NewAmount)
{
	Amount = NewAmount;
	UpdateInfo();
}

void AWorldItem::PickUp(ICarrier* Carrier)
{
	TArray<AActor*> OverlappingActors;
	PickUpRadius->GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	if(AActor* CarrierActor = Cast<AActor>(Carrier))
	{
		if(OverlappingActors.Contains(CarrierActor))
		{
			Carrier->PickUpItem(ItemName, Amount);
			Destroy();
		}
	}
}

void AWorldItem::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Target)
	{
		return;
	}
	if(AAvatar* CarrierActor = Cast<AAvatar>(OtherActor))
	{
		if(CarrierActor)
		{
			if(CarrierActor->IsMagnetized)
			{
				Target = CarrierActor;
			}
			PotentialTargets.Add(CarrierActor);
		}
	}
}

void AWorldItem::OnEndCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(AAvatar* CarrierActor = Cast<AAvatar>(OtherActor))
	{
		PotentialTargets.Remove(CarrierActor);
	}
}

void AWorldItem::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		PickUpRadius->OnComponentBeginOverlap.AddDynamic(this, &AWorldItem::OnCollision);
		PickUpRadius->OnComponentEndOverlap.AddDynamic(this, &AWorldItem::OnEndCollision);
	}
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(this, AGridManager::StaticClass()));
	UpdateInfo();
	if(!HasAuthority() && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SetBackroundTimerHandle, this, &AWorldItem::SetBackgroundSize, 0.01f, false);
	}
}

void AWorldItem::Tick(float DeltaTime)
{
	if(!HasAuthority())
	{
		SetActorHiddenInGame(!IsVisibleForTeam(UUtil::GetLocalPlayerTeamId(this)));
	}
	if(HasAuthority() && Target)
	{
		if(!Target->IsMagnetized)
		{
			Target = nullptr;
			return;
		}
		const float Distance = FVector::Dist(Target->GetActorLocation(), GetActorLocation());
		if(Distance < 100)
		{
			Target->PickUpItem(ItemName, Amount);
			Destroy();
		}
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		Direction.Normalize();
		Location = GetActorLocation() + Direction * Speed * DeltaTime;
		SetActorLocation(Location);
		Speed *= 1.02f;
	}
	else
	{
		for(AAvatar* PotentialTarget : PotentialTargets)
		{
			if(PotentialTarget->IsMagnetized)
			{
				Target = PotentialTarget;
				break;
			}
		}
	}
	if(!HasAuthority() && bLocationDirty)
	{
		SetActorLocation(FMath::Lerp(GetActorLocation(), Location, 0.1f));
	}
	Super::Tick(DeltaTime);
}


void AWorldItem::SetBackgroundSize()
{
	if(UWorldItemInfoWidget* ItemInfoWidget = Cast<UWorldItemInfoWidget>(NameWidget->GetUserWidgetObject()))
	{
		ItemInfoWidget->SetBackgroundSize();
	}
}

void AWorldItem::UpdateInfo()
{
	if(UWorldItemInfoWidget* ItemInfoWidget = Cast<UWorldItemInfoWidget>(NameWidget->GetUserWidgetObject()))
	{
		FItemData* ItemData = UUtil::GetItemDataFromName(ItemName);
		FString ItemNameString = ItemData ? ItemData->DisplayName.ToString() : ItemName.ToString();
		ItemNameString.Append(" : ");
		ItemNameString.Append(FString::FromInt(Amount));
		ItemInfoWidget->SetText(FName(*ItemNameString));
	}
}

void AWorldItem::OnRep_Location()
{
	bLocationDirty = true;
}

void AWorldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorldItem, ItemName);
	DOREPLIFETIME(AWorldItem, Amount);
	DOREPLIFETIME(AWorldItem, Location);
}

void AWorldItem::OnRep_Amount()
{
	UpdateInfo();
}


