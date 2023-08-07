// Copyright Radiant Studios


#include "Items/WorldItem.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ItemInfo/WorldItemInfoWidget.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "Util/Util.h"
#include "Util/Interfaces/Carrier.h"

AWorldItem::AWorldItem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	PickUpRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpRadius"));
	PickUpRadius->SetupAttachment(Mesh);
	
	FX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FX"));
	FX->SetupAttachment(Mesh);

	NameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	NameWidget->SetupAttachment(Mesh);
}

void AWorldItem::InitItem(FName NewItemName, uint32 NewAmount)
{
	ItemName = NewItemName;
	Amount = NewAmount;
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


void AWorldItem::MoveToActorIfMagnetized(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AAvatar* CarrierActor = Cast<AAvatar>(OtherActor))
	{
		if(CarrierActor && CarrierActor->IsMagnetized)
		{
			Target = CarrierActor;
		}
	}
}

void AWorldItem::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		PickUpRadius->OnComponentBeginOverlap.AddDynamic(this, &AWorldItem::MoveToActorIfMagnetized);
	}
	if(UWorldItemInfoWidget* ItemInfoWidget = Cast<UWorldItemInfoWidget>(NameWidget->GetUserWidgetObject()))
	{
		FString ItemNameString = ItemName.ToString();
		ItemNameString.Append(" : ");
		ItemNameString.Append(FString::FromInt(Amount));
		ItemInfoWidget->SetText(FName(*ItemNameString));
	}

	if(!HasAuthority() && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SetBackroundTimerHandle, this, &AWorldItem::SetBackgroundSize, 0.01f, false);
	}
}

void AWorldItem::Tick(float DeltaTime)
{
	if(Target && HasAuthority())
	{
		const float Distance = FVector::Dist(Target->GetActorLocation(), GetActorLocation());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Role %d"), GetLocalRole()));
		if(HasAuthority() && Distance < 300)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Picked up %s"), *ItemName.ToString()));
			Target->PickUpItem(ItemName, Amount);
			Destroy();
		}
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		Direction.Normalize();
		FVector NewLocation = GetActorLocation() + Direction * 1000 * DeltaTime;
		SetActorLocation(NewLocation);
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

void AWorldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorldItem, ItemName);
	DOREPLIFETIME(AWorldItem, Amount);
}


