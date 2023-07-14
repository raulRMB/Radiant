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

	for(int x = 3; x < 6; x++)
	{
		for(int y = 3; y < 6; y++)
		{
			Cells[x + y * GridSize.Y] = true;
		}
	}

	for(int x = Cells.Num() - 4; x < Cells.Num() - 7; x--)
	{
		for(int y = Cells.Num() - 4; y < Cells.Num() - 7; y--)
		{
			Cells[x + y * GridSize.Y] = true;
		}
	}
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

bool AGridManager::CheckCanPlace(const FGridPiece Piece)
{
	return Cells[Piece.Position.X + Piece.Position.Y * GridSize.Y];
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
