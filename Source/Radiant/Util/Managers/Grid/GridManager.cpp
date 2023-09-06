// Copyright Radiant Studios


#include "Util/Managers/Grid/GridManager.h"
#include "DrawDebugHelpers.h"
#include "TeamGridManager.h"
#include "Building/Building.h"
#include "Enums/VIsionRange.h"
#include "Structs/GridPiece.h"
#include "Util/Util.h"

AGridManager::AGridManager()
{
	bReplicates = false;
	GridDimensions = 0;
	bSpawnMap = 0;
	MapTexture = nullptr;
	PrimaryActorTick.bCanEverTick = true;
}

void AGridManager::GenerateMap()
{
#if !UE_BUILD_SHIPPING

	Cells.Empty();
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
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Wall;
			}
			else if(PixelColor.ToPackedRGBA() == 0Xff6f00ff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Rock;
			}
			else if(PixelColor.ToPackedRGBA() == 0X3bff00ff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Tree;
			}
			else if(PixelColor.ToPackedRGBA() == 0x0000ffff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Vein;
			}
			else if(PixelColor.ToPackedRGBA() == 0xfbff00ff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Grass;
			}
		}
	}

	MapTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	MapTexture->CompressionSettings = OldCompressionSettings;
	MapTexture->MipGenSettings = OldMipGenSettings;
	MapTexture->SourceColorSettings.EncodingOverride = OldSourceEncoding;
	MapTexture->SRGB = OldSRGB;
	MapTexture->UpdateResource();
	
#endif // WITH_EDITOR
	
}

void AGridManager::GenerateVisionRanges()
{
	TArray<FColor> Colors;
	UUtil::ReadTextureColors(VisionRangesTexture, Colors);

	ShortRange.Empty();
	MediumRange.Empty();
	LongRange.Empty();

	int SizeX = VisionRangesTexture->GetSizeX();
	int SizeY = VisionRangesTexture->GetSizeX();
	for(int x = 0; x < SizeX; x++)
	{
		for(int y = 0; y < SizeY; y++)
		{
			FColor PixelColor = Colors[y * SizeX + x];
			
			if(PixelColor.ToPackedRGBA() == 0X0000ffff)
			{
				ShortRange.Add({x - SizeX / 2, y - SizeY / 2});
			}
			else if(PixelColor.ToPackedRGBA() == 0X00ff00ff)
			{
				MediumRange.Add({x - SizeX / 2, y - SizeY / 2});
			}
			else if(PixelColor.ToPackedRGBA() == 0xff0000ff)
			{
				LongRange.Add({x - SizeX / 2, y - SizeY / 2});
			}
		}
	}
}

TArray<FIntVector2>& AGridManager::GetRange(EVisionRange Range)
{
	switch (Range)
	{
	case EVisionRange::Short:
		return ShortRange;
	case EVisionRange::Medium:
		return MediumRange;
	case EVisionRange::Long:
		return LongRange;
	default:
		return ShortRange;
	}
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
				FTransform SpawnTransform = FTransform(GetTransformedVector(Position));			
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

void AGridManager::AddVisibleActor(AActor* Actor)
{
	for(ATeamGridManager* TeamManager : TeamGridManagers)
	{
		if(ITeamMember* TeamMember = Cast<ITeamMember>(Actor))
		{
			if(TeamManager->GetTeamId() == TeamMember->GetTeamId())
			{
				TeamManager->AddVisibleActor(Actor);
			}
		}
	}
}

void AGridManager::PieceChanged(const FGridPiece Piece)
{
	for(ATeamGridManager* TeamManager : TeamGridManagers)
	{
		TeamManager->PieceChanged(Piece);
	}
}

void AGridManager::PlacePieceAtMouse(const FGridPiece Piece)
{
	if(HasAuthority())
	{
		FIntVector2 TransformedPos = GetTransformedPosition(Piece.Position);
		
		if(TransformedPos.X < 0 || TransformedPos.Y < 0 || TransformedPos.X >= GridDimensions || TransformedPos.Y >= GridDimensions)
		{
			return;
		}
		int32 Pos = TransformedPos.X + TransformedPos.Y * GridDimensions;
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
		FTransform Transform = FTransform(GetTransformedVector(Piece.Position));
		AActor* Actor = GetWorld()->SpawnActorDeferred<AActor>(BuildingTypes[Piece.Type], Transform);
		if(ABuilding* Building = Cast<ABuilding>(Actor))
		{
			Building->S_SetTeamId(Piece.TeamId);
			Building->M_ShowInfoBar(true, 1.f);
			Building->InitGridValues(this, Piece);
		}
		Actor->FinishSpawning(Transform);
		Cells[TransformedPos.X + TransformedPos.Y * GridDimensions] = Piece.Type;
		PieceChanged(Piece);
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
	PieceChanged(Piece);
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

	if(HasAuthority())
	{
		ATeamGridManager* TeamGridManager = GetWorld()->SpawnActorDeferred<ATeamGridManager>(TeamGridManagerClass, FTransform::Identity);
		TeamGridManager->SetTeamId(ETeamId::Red);
		TeamGridManager->SetGridManager(this);
		TeamGridManagers.Add(TeamGridManager);
		TeamGridManager->FinishSpawning(FTransform::Identity);
		TeamGridManager->Init(Cells, BuildingTypes);
		
		TeamGridManager = GetWorld()->SpawnActorDeferred<ATeamGridManager>(TeamGridManagerClass, FTransform::Identity);
		TeamGridManager->SetTeamId(ETeamId::Blue);
		TeamGridManager->SetGridManager(this);
		TeamGridManagers.Add(TeamGridManager);
		TeamGridManager->FinishSpawning(FTransform::Identity);
		TeamGridManager->Init(Cells, BuildingTypes);
	}
}

EEnvironmentType& AGridManager::GetCell(const FIntVector2& Position)
{
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridDimensions || Position.Y >= GridDimensions)
	{
		return Cells[0];
	}
	return Cells[static_cast<int32>(Position.X) + static_cast<int32>(Position.Y) * GridDimensions];
}

ABuilding* AGridManager::GetBuildingType(EEnvironmentType Type)
{
	if(BuildingTypes.Contains(Type))
	{
		return BuildingTypes[Type]->GetDefaultObject<ABuilding>();
	}
	return nullptr;
}

UStaticMeshComponent* AGridManager::GetMesh(EEnvironmentType Type)
{
	if(BuildingTypes.Contains(Type))
	{
		if(ABuilding* Building = Cast<ABuilding>(BuildingTypes[Type].GetDefaultObject()))
		{
			return Cast<UStaticMeshComponent>(Building->GetMesh());
		}
	}
	return nullptr;
}

FVector AGridManager::GetTransformedVector(const FIntVector2& Position)
{
	double X = 127 - Position.Y;
	double Y = Position.X;
	
	return FVector(X, Y, 0.f) * CellSize;
}

FIntVector2 AGridManager::GetTransformedPosition(const FIntVector2& Position)
{
	return Position;
	int32 X = 127 - Position.Y;
	int32 Y = Position.X;
	
	return FIntVector2(X, Y);
}

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AGridManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
