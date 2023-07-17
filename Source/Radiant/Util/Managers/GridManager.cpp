// Copyright Radiant Studios


#include "Util/Managers/GridManager.h"
#include "DrawDebugHelpers.h"
#include "Building/Building.h"
#include "Net/UnrealNetwork.h"
#include "Util/Util.h"

AGridManager::AGridManager()
{
	bReplicates = true;
}

void AGridManager::InitGrid()
{
	Cells.Init(false, GridSize.X * GridSize.Y);
}

void AGridManager::PlacePieceAtMouse(FGridPiece Piece)
{
	if(HasAuthority())
	{
		if(Piece.Position.X < 0 || Piece.Position.Y < 0 || Piece.Position.X >= GridSize.X || Piece.Position.Y >= GridSize.Y)
		{
			return;
		}
		auto Pos = Piece.Position.X + Piece.Position.Y * GridSize.Y;
		if(Cells.Num() < Pos || Cells[Pos])
		{
			return;
		}
		FTransform Transform = FTransform(FVector(Piece.Position.X, Piece.Position.Y, 0.f) * CellSize);
		ABuilding* Building = GetWorld()->SpawnActorDeferred<ABuilding>(BuildingTypes[Piece.Type], Transform);
		Building->S_SetTeamId(Piece.TeamId);
		Building->FinishSpawning(Transform);
		Cells[Piece.Position.X + Piece.Position.Y * GridSize.Y] = true;
	}
}

bool AGridManager::CheckCanPlace(const FIntVector2 Position)
{
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridSize.X || Position.Y >= GridSize.Y)
	{
		return false;
	}
	int32 Pos = Position.X + Position.Y * GridSize.Y;
	if(Cells.Num() < Pos || Cells[Pos])
	{
		return false;
	}
	
	return Cells[Position.X + Position.Y * GridSize.Y];
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	InitGrid();
}

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGridManager, Cells);
}
