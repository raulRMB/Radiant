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
	Cells.Init(EEnvironmentType::EEnvironmentType_Empty, GridSize.X * GridSize.Y);

	if(HasAuthority())
	{
		for(int y = 0; y < GridSize.Y; y++)
		{
			for(int x = 0; x < GridSize.X; x++)
			{
				float Perlin = FMath::PerlinNoise2D(FVector2D(x + 0.1, y + 0.1) * PerlinScale);
				
				EEnvironmentType Type;
				if(Perlin > .4f && Perlin < .5f)
				{
					Type = EEnvironmentType::EEnvironmentType_Tree;
				}
				else if(Perlin > .3f)
				{
					Type = EEnvironmentType::EEnvironmentType_TreeStump;
				}
				else if(Perlin > .2f)
				{
					Type = EEnvironmentType::EEnvironmentType_Grass;
				}
				else if(Perlin < -.4f)
				{
					Type = EEnvironmentType::EEnvironmentType_Rock;
				}
				else if(Perlin < -.3f)
				{
					Type = EEnvironmentType::EEnvironmentType_Pebbles;
				}
				else
				{
					Cells[x + y * GridSize.Y] = EEnvironmentType::EEnvironmentType_Empty;
					continue;
				}
				FTransform Transform = FTransform(FVector(x, y, 0.f) * CellSize);
				if(BuildingTypes.Contains(Type))
				{
					Cells[x + y * GridSize.Y] = Type;
					GetWorld()->SpawnActor<AActor>(BuildingTypes[Type], Transform);
				}
			}
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
		int32 Pos = Piece.Position.X + Piece.Position.Y * GridSize.Y;
		if(Cells.Num() < Pos)
		{
			return;
		}
		if(Piece.Type == EEnvironmentType::EEnvironmentType_Extractor)
		{
			EEnvironmentType Type = Cells[Pos];
			if(Type != EEnvironmentType::EEnvironmentType_Vein)
			{
				return;
			}
		}
		else if(Cells[Pos] != EEnvironmentType::EEnvironmentType_Empty)
		{
			return;
		}
		FTransform Transform = FTransform(FVector(Piece.Position.X, Piece.Position.Y, 0.f) * CellSize);
		ABuilding* Building = GetWorld()->SpawnActorDeferred<ABuilding>(BuildingTypes[Piece.Type], Transform);
		Building->S_SetTeamId(Piece.TeamId);
		Building->FinishSpawning(Transform);
		Cells[Piece.Position.X + Piece.Position.Y * GridSize.Y] = Piece.Type;
	}
}

bool AGridManager::CheckCanPlace(const FGridPiece Piece)
{
	FIntVector2 Position = Piece.Position;
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridSize.X || Position.Y >= GridSize.Y)
	{
		return false;
	}
	int32 Pos = Position.X + Position.Y * GridSize.Y;
	
	if(Cells.Num() < Pos)
	{
		return false;
	}
	if(Piece.Type == EEnvironmentType::EEnvironmentType_Extractor)
	{
		EEnvironmentType Type = Cells[Pos];
		return Type == EEnvironmentType::EEnvironmentType_Vein;
	}
	
	return Cells[Position.X + Position.Y * GridSize.Y] == EEnvironmentType::EEnvironmentType_Empty;
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
