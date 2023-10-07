// Copyright Radiant Studios


#include "TeamGridManager.h"

#include "Building/Building.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Texture2D.h"
#include "Net/UnrealNetwork.h"
#include "Structs/GridPiece.h"
#include "Util/TempGridActor.h"
#include "Enums/EnvironmentType.h"
#include "RHI.h"
#include "Player/RTPlayerController.h"
#include "Util/NotificationActor.h"
#include "Util/Util.h"
#include "Util/Interfaces/TeamMember.h"

void ATeamGridManager::OnRep_Cells()
{	
	if(Initialized || HasAuthority())
	{
		return;
	}
	InitGrid();
	Initialized = true;
	
	SpawnInitialTempActors();
}

void ATeamGridManager::SpawnInitialTempActors()
{
	if(TempActorsEnabled)
	{
		for(int x = 0; x < GridDimensions; x++)
		{
			for (int y = 0; y < GridDimensions; y++)
			{
				if(Cells[y * GridDimensions + x] == EEnvironmentType::EEnvironmentType_Empty)
				{
					continue;
				}
				FIntVector2 Position = FIntVector2(x, y);
				FTransform SpawnTransform = FTransform(GetTransformedVector(Position));			
				UStaticMeshComponent* Mesh = GridManager->GetMesh(Cells[y * GridDimensions + x]);
				ATempGridActor* TempGridActor = GetWorld()->SpawnActorDeferred<ATempGridActor>(ATempGridActor::StaticClass(), SpawnTransform);
				if(Mesh)
				{
					if(ABuilding* Building = GridManager->GetBuildingType(Cells[y * GridDimensions + x]))
					{
						TempGridActor->SetActorScale3D(Mesh->GetRelativeScale3D());
						TempGridActor->SetActorRotation(Mesh->GetRelativeRotation());
						TempGridActor->SetActorLocation(TempGridActor->GetActorLocation() + Mesh->GetRelativeLocation());
						TempGridActor->SetMesh(Mesh);
						TempGridActor->SetMaterial(Building->GetDarkMaterial());
						TempGridActor->SetBox(Building->GetBox(), GetTransformedVector(Position));
					}
				}
				TempPieces[x + y * GridDimensions] = TempGridActor;
				TempGridActor->FinishSpawning(SpawnTransform);
			}
		}
	}
}

void ATeamGridManager::InitializeCanvas()
{
	FOWPixels = std::make_unique<uint8[]>(128 * 128 * 4);
	FOWUpdateTextureRegion = std::make_unique<FUpdateTextureRegion2D>(0, 0, 0, 0, 128, 128);
	MinimapPixels = std::make_unique<uint8[]>(128 * 128 * 4);
	MinimapPixelsUpdateTextureRegion = std::make_unique<FUpdateTextureRegion2D>(0, 0, 0, 0, 128, 128);
}

void ATeamGridManager::SetPixelColor(uint8*& Pointer, FColor Color)
{
	*Pointer = Color.B;
	*(Pointer + 1) = Color.G;
	*(Pointer + 2) = Color.R;
	*(Pointer + 3) = 255;
}

void ATeamGridManager::UpdateCanvas() const
{
	if(FOWUpdateTextureRegion)
	{
		FOWCanvas->UpdateTextureRegions(0, 1, FOWUpdateTextureRegion.get(), 4 * FOWCanvas->GetSizeX(), 4, FOWPixels.get());
	}
	if(MinimapPixelsUpdateTextureRegion)
	{
		MinimapCanvas->UpdateTextureRegions(0, 1, MinimapPixelsUpdateTextureRegion.get(), 4 * MinimapCanvas->GetSizeX(), 4, MinimapPixels.get());
	}
}

ATeamGridManager::ATeamGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

const TMap<AActor*, int32>& ATeamGridManager::GetActorsInVision()
{
	return ActorsInVision;
}

void ATeamGridManager::AddActorToActorsInVision(AActor* Actor)
{
	if(ANotificationActor* NA = Cast<ANotificationActor>(Actor))
	{
		if(NA->GetTeamId() == TeamId)
		{
			UpdateCellForClients(NA->GetGridPiece());
			NA->Destroy();
		}
	}
	if(int32* Count = ActorsInVision.Find(Actor))
	{
		(*Count)++;
	}
	else
	{
		ActorsInVision.Add(Actor, 1);
	}
}

void ATeamGridManager::DecrementActorFromActorsInVision(AActor* Actor)
{
	if(Actor)
	{
		if(int32* Count = ActorsInVision.Find(Actor))
		{
			(*Count)--;
			if(*Count <= 0)
			{
				ActorsInVision.Remove(Actor);
			}
		}
	}
}

void ATeamGridManager::RemoveActorFromActorsInVision(AActor* Actor)
{
	ActorsInVision.Remove(Actor);
}

bool ATeamGridManager::IsTargetInVision(const AActor* ArtCharacter) const
{
	return ActorsInVision.Contains(ArtCharacter);
}

void ATeamGridManager::BeginPlay()
{
	InitializeCanvas();
	
	InitGrid();
	Super::BeginPlay();
	NotificationActors.Init(nullptr, 128*128);
	UActorManager* ActorManager = GetGameInstance()->GetSubsystem<UActorManager>();
	ActorManager->AddTeamGridManager(TeamId, this);
}

void ATeamGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATeamGridManager, Cells);
	DOREPLIFETIME(ATeamGridManager, TeamId);
	DOREPLIFETIME(ATeamGridManager, VisibleActors);
	DOREPLIFETIME(ATeamGridManager, GridManager);
}

bool ATeamGridManager::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	if(const ARTPlayerController* TeamMember = Cast<ARTPlayerController>(RealViewer))
	{
		return TeamMember->GetTeamId() == TeamId;
	}
	return false;
}

void ATeamGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!HasAuthority())
	{
		if(TeamId == ETeamId::Neutral || VisibleCells.Num() == 0 || TeamId != UUtil::GetLocalPlayerTeamId(this))
		{
			return;
		}
		ClearAllVisible();
		DrawVisible();
		DrawMiniMap();
	}
}

void ATeamGridManager::DrawMiniMap()
{
	uint8* FOWPixel = FOWPixels.get();
	uint8* MinimapPixel = MinimapPixels.get();
	for(int y = 0; y < 128; y++)
	{
		for(int x = 0; x < 128; x++)
		{
			SetPixelColor(MinimapPixel, GetColorForType(Cells[x + y * 128]));
			MinimapPixel+=4;
			SetPixelColor(FOWPixel, VisibleCells[x + y * 128] ? FColor::White : FColor::Black);
			FOWPixel+=4;
		}
	}
	UpdateCanvas();
}

void ATeamGridManager::InitGrid()
{
	if(!Initialized)
	{
		VisibleCells.Init(false, 128*128);
		TempPieces.Init(nullptr, 128*128);
	}
}

void ATeamGridManager::AddVisibleActor(AActor* Actor)
{
	VisibleActors.AddUnique(Actor);
}

void ATeamGridManager::RemoveVisibleActor(AActor* Actor)
{
	VisibleActors.Remove(Actor);
}

bool ATeamGridManager::HasTempActor(FGridPiece& Piece)
{
	if(!TempActorsEnabled)
	{
		return false;
	}
	return TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions] != nullptr;
}

void ATeamGridManager::SpawnTempActor(const FGridPiece& Piece, ABuilding* Building, FTransform Transform)
{
	if(!TempActorsEnabled)
	{
		return;
	}
	if(TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions] != nullptr)
	{
		return;
	}
	if(Piece.Type != EEnvironmentType::EEnvironmentType_Empty)
	{
		return;
	}
	ATempGridActor* TempGridActor = GetWorld()->SpawnActorDeferred<ATempGridActor>(ATempGridActor::StaticClass(), Transform);
	TempGridActor->SetMesh(Building->GetMesh());
	TempGridActor->SetActorTransform(Building->GetMesh()->GetComponentTransform());
	TempGridActor->SetBox(Building->GetBox(), GetTransformedVector(Piece.Position));
	TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions] = TempGridActor;
	TempGridActor->FinishSpawning(Transform);
}

void ATeamGridManager::HideTempActor(FGridPiece& Piece, bool bHide)
{
	if(TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions] == nullptr || !TempActorsEnabled)
	{
		return;
	}
	auto Actor = TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions];
	Actor->SetActorHiddenInGame(bHide);
}

void ATeamGridManager::DestroyTempActor(const FGridPiece& Piece)
{
	if(TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions] == nullptr || !TempActorsEnabled)
	{
		return;
	}
	TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions]->Destroy();
}

FVector ATeamGridManager::GetTransformedVector(const FIntVector2& Position)
{
	double X = 127 - Position.Y;
	double Y = Position.X;
	
	return FVector(X, Y, 0.f) * CellSize;
}

void ATeamGridManager::Init(const TArray<EEnvironmentType> TrueCells, TMap<EEnvironmentType, TSubclassOf<class AActor>> BuildingTypes)
{
	Cells = TrueCells;
}

bool ATeamGridManager::GetNeighbor(const FIntVector2& Position, const FIntVector2& Direction, EEnvironmentType& OutCell)
{
	FIntVector2 NeighborPosition = FIntVector2(Position.X + Direction.X, Position.Y + Direction.Y);
	if(NeighborPosition.X < 0 || NeighborPosition.Y < 0 || NeighborPosition.X >= GridDimensions || NeighborPosition.Y >= GridDimensions)
	{
		return false;
	}
	OutCell = Cells[NeighborPosition.X + NeighborPosition.Y * GridDimensions];
	return true;
}

bool ATeamGridManager::IsBlockingVision(const FIntVector2& Position)
{	
	EEnvironmentType& Cell = GetCell(Position);
	if(Cell != EEnvironmentType::EEnvironmentType_Empty)
	{
		return true;
	}
	return false;
}

EEnvironmentType& ATeamGridManager::GetCell(const FIntVector2& Position)
{
	return Cells[Position.X + Position.Y * GridDimensions];
}

bool ATeamGridManager::CheckVisible(const FVector2D& From, const FVector2D& To)
{
	const double DX = To.X - From.X;
	const double DY = To.Y - From.Y;

	const double SideLength = FMath::Max(FMath::Abs(DX), FMath::Abs(DY));

	const double XIncrement = DX / SideLength;
	const double YIncrement = DY / SideLength;

	double CurrentX = From.X;
	double CurrentY = From.Y;

	FVector2D Direction = FVector2D(DX, DY);
	Direction.Normalize();

	int depth = 0;
	for(int i = 0; i <= SideLength; i++)
	{
		uint32 index = static_cast<uint32>(CurrentX) + static_cast<uint32>(CurrentY) * GridDimensions;
		VisibleCells[index] = true;
		if(IsBlockingVision(FIntVector2(CurrentX, CurrentY)))
		{
			depth++;
		}
		if(depth >= 2)
		{
			break;
		}
		CurrentX += XIncrement;
		CurrentY += YIncrement;
	}
	return false;
}

FColor ATeamGridManager::GetColorForType(const EEnvironmentType& Type)
{
	switch (Type)
	{
	case EEnvironmentType::EEnvironmentType_Rock:
		return FColor(102, 102, 102);
	case EEnvironmentType::EEnvironmentType_Tree:
		return FColor(71, 119, 42);
	case EEnvironmentType::EEnvironmentType_Grass:
		return FColor(95, 185, 38);
	case EEnvironmentType::EEnvironmentType_Wall:
		return FColor(255,25,10);
	default:
		return FColor(3,3, 6);

	}
}

bool ATeamGridManager::GetVisibleCell(const FIntVector2& Position)
{
	return VisibleCells[Position.X + Position.Y * GridDimensions];
}

void ATeamGridManager::ClearAllVisible()
{	
	for(int i = 0; i < VisibleCells.Num(); i++)
	{
		if(VisibleCells[i])
		{
			VisibleCells[i] = false;
		}
	}
}

void ATeamGridManager::DrawVisible()
{
	for(auto Actor : VisibleActors)
	{
		if(Actor)
		{
			if(ITeamMember * TeamMember = Cast<ITeamMember>(Actor))
			{
				if(TeamMember->GetTeamId() != TeamId)
				{
					continue;
				}
				if(!IsValid(GridManager))
				{
					return;
				}
				double X = (Actor->GetActorLocation().Y + 200.) / 200.;
				double Y = 127. - (Actor->GetActorLocation().X - 200.) / 200.;

				for(float Rad = 0.f; Rad < TWO_PI; Rad += 0.01f)
				{
					FVector2D From = FVector2D(X, Y);
					FVector2D To = FVector2D(X + FMath::Cos(Rad) * 20, Y + FMath::Sin(Rad) * 20);
					CheckVisible(From, To);
				}
				// for(FIntVector2& Vec : GridManager->GetRange(EVisionRange::Medium))
				// {
				// 	FVector2D From = FVector2D(X, Y);
				// 	FVector2D To = FVector2D(X + Vec.X, Y + Vec.Y);
				// 	CheckVisible(From, To);
				// }
			}
		}
	}
}

bool ATeamGridManager::IsTargetVisible(const AActor* Target)
{
	FIntVector2 TargetPosition = FIntVector2((Target->GetActorLocation().Y + 100) / 200, 127 - (Target->GetActorLocation().X + 100) / 200);
	return VisibleCells[TargetPosition.X + TargetPosition.Y * GridDimensions];
}

void ATeamGridManager::UpdateTempActor(const FGridPiece& Piece)
{
	DestroyTempActor(Piece);
	if(Piece.Type != EEnvironmentType::EEnvironmentType_Empty)
	{
		FTransform SpawnTransform = FTransform(GetTransformedVector(Piece.Position));
		SpawnTempActor(Piece, GridManager->GetBuildingType(Piece.Type), SpawnTransform);
	}
}

void ATeamGridManager::M_UpdateTempActor_Implementation(const FGridPiece& Piece)
{
	if(!HasAuthority())
	{
		UpdateTempActor(Piece);
	}
}

void ATeamGridManager::PieceChanged(const ABuilding* Building)
{
	FGridPiece Piece = Building->GetGridPiece();
	
	if(HasAuthority())
	{
		if(Piece.Position.X < 0 || Piece.Position.Y < 0 || Piece.Position.X >= GridDimensions || Piece.Position.Y >= GridDimensions)
		{
			return;
		}
		int32 Pos = Piece.Position.X + Piece.Position.Y * GridDimensions;
		if(Cells.Num() < Pos)
		{
			return;
		}
		if(ActorsInVision.Contains(Building))
		{
			UpdateCellForClients(Piece);
		} else
		{
			SpawnNotificationActor(Piece);
		}
	}
}

void ATeamGridManager::UpdateCellForClients(FGridPiece Piece)
{
	int32 Pos = Piece.Position.X + Piece.Position.Y * GridDimensions;
	Cells[Pos] = GridManager->GetCell(Piece.Position);
	Piece.Type = Cells[Pos];
	M_UpdateTempActor(Piece);
}

void ATeamGridManager::SpawnNotificationActor(FGridPiece Piece)
{
	FTransform SpawnTransform = FTransform(GetTransformedVector(Piece.Position));
	ANotificationActor* NA = GetWorld()->SpawnActorDeferred<ANotificationActor>(ANotificationActor::StaticClass(), SpawnTransform);
	NA->SetGridPiece(Piece);
	NA->SetTeamId(TeamId);
	NA->FinishSpawning(SpawnTransform);
	int32 Pos = Piece.Position.X + Piece.Position.Y * GridDimensions;
	if(NotificationActors[Pos] != nullptr)
	{
		NotificationActors[Pos]->Destroy();
	}
	NotificationActors[Pos] = NA;
}


