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

void AGridManager::GenerateMap()
{
	Cells.Init(EEnvironmentType::EEnvironmentType_Empty, MapTexture->GetSizeX() * MapTexture->GetSizeY());
	GridDimensions = MapTexture->GetSizeX();
	
	TextureCompressionSettings OldCompressionSettings = MapTexture->CompressionSettings;
	ETextureSourceEncoding OldSourceEncoding = MapTexture->SourceColorSettings.EncodingOverride;
	TextureMipGenSettings OldMipGenSettings = MapTexture->MipGenSettings;
	bool OldSRGB = MapTexture->SRGB;

	MapTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	MapTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	MapTexture->SourceColorSettings.EncodingOverride = ETextureSourceEncoding::TSE_Linear;
	MapTexture->SRGB = false;
	MapTexture->UpdateResource();
	
	const FColor* FormatedImageData = static_cast<const FColor*>(MapTexture->GetPlatformData()->Mips[0].BulkData.LockReadOnly());
	
	for(int32 X = 0; X < MapTexture->GetSizeX(); X++)
	{
		for (int32 Y = 0; Y < MapTexture->GetSizeY(); Y++)
		{
			FColor PixelColor = FormatedImageData[Y * MapTexture->GetSizeX() + X];

			if(PixelColor.ToPackedRGBA() == 0Xffffffff)
			{
				Cells[(MapTexture->GetSizeY() - Y - 1) * MapTexture->GetSizeX() + X] = EEnvironmentType::EEnvironmentType_Wall;
			}
			else if(PixelColor.ToPackedRGBA() == 0Xff6f00ff)
			{
				Cells[(MapTexture->GetSizeY() - Y - 1) * MapTexture->GetSizeX() + X] = EEnvironmentType::EEnvironmentType_Rock;
			}
			else if(PixelColor.ToPackedRGBA() == 0X3bff00ff)
			{
				Cells[(MapTexture->GetSizeY() - Y - 1) * MapTexture->GetSizeX() + X] = EEnvironmentType::EEnvironmentType_Tree;
			}
			else if(PixelColor.ToPackedRGBA() == 0x0300aaff)
			{
				Cells[(MapTexture->GetSizeY() - Y - 1) * MapTexture->GetSizeX() + X] = EEnvironmentType::EEnvironmentType_Vein;
			}
			else if(PixelColor.ToPackedRGBA() == 0xfbff00ff)
			{
				Cells[(MapTexture->GetSizeY() - Y - 1) * MapTexture->GetSizeX() + X] = EEnvironmentType::EEnvironmentType_Grass;
			}
		}
	}

	MapTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	MapTexture->CompressionSettings = OldCompressionSettings;
	MapTexture->MipGenSettings = OldMipGenSettings;
	MapTexture->SourceColorSettings.EncodingOverride = OldSourceEncoding;
	MapTexture->SRGB = OldSRGB;
	MapTexture->UpdateResource();
}

void AGridManager::InitGrid()
{
	if(!bSpawnMap)
	{
		return;
	}
	
	if(HasAuthority())
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
				FTransform SpawnTransform = FTransform(FVector(Position.X, Position.Y, 0.f) * CellSize);
				AActor* Actor = GetWorld()->SpawnActorDeferred<AActor>(BuildingTypes[Cells[y * GridDimensions + x]], SpawnTransform);
				
				if(ABuilding* Building = Cast<ABuilding>(Actor))
				{
					FGridPiece Piece;
					Piece.Position = Position;
					Piece.Level = 1;
					Piece.Type = Cells[y * GridDimensions + x];
					Piece.TeamId = ETeamId::Neutral;
					Building->InitGridValues(this, Piece);
					Building->S_SetTeamId(ETeamId::Neutral);
				}
				
				Actor->FinishSpawning(SpawnTransform);
			}
		}
	}
}

void AGridManager::PlacePieceAtMouse(const FGridPiece Piece)
{
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
		AActor* Actor = GetWorld()->SpawnActorDeferred<AActor>(BuildingTypes[Piece.Type], Transform);
		if(ABuilding* Building = Cast<ABuilding>(Actor))
		{
			Building->S_SetTeamId(Piece.TeamId);
			Building->M_ShowInfoBar(true, 1.f);
			Building->InitGridValues(this, Piece);
		}
		Actor->FinishSpawning(Transform);
		Cells[Piece.Position.X + Piece.Position.Y * GridDimensions] = Piece.Type;
	}
}

void AGridManager::ClearPiece(const FGridPiece Piece)
{
	FIntVector2 Position = Piece.Position;
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridDimensions || Position.Y >= GridDimensions)
	{
		return;
	}

	Cells[Position.X + Position.Y * GridDimensions] = EEnvironmentType::EEnvironmentType_Empty;
}

bool AGridManager::CheckCanPlace(const FGridPiece Piece)
{
	FIntVector2 Position = Piece.Position;
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridDimensions || Position.Y >= GridDimensions)
	{
		return false;
	}
	int32 Pos = Position.X + Position.Y * GridDimensions;
	
	if(Cells.Num() < Pos)
	{
		return false;
	}
	if(Piece.Type == EEnvironmentType::EEnvironmentType_Extractor)
	{
		EEnvironmentType Type = Cells[Pos];
		return Type == EEnvironmentType::EEnvironmentType_Vein;
	}
	
	return Cells[Position.X + Position.Y * GridDimensions] == EEnvironmentType::EEnvironmentType_Empty;
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
