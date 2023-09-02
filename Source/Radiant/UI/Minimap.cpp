// Copyright Radiant Studios


#include "UI/Minimap.h"

#include "Components/Image.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Modes/Base/RTGameState.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerState.h"
#include "Util/Managers/ActorManager.h"

void UMinimap::DrawDynamic()
{
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RenderTarget, FLinearColor::Transparent);
	
	UCanvas* Canvas;
	FDrawToRenderTargetContext Context;
	FVector2D Size;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
		this,
		RenderTarget,
		Canvas,
		Size,
		Context
		);

	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(this, AAvatar::StaticClass(), Players);
	if(ARTPlayerState* PS = UGameplayStatics::GetPlayerController(this, 0)->GetPlayerState<ARTPlayerState>())
	{
		float MapSize = 12600.f;
		for(AActor* Player : Players)
		{
			if(Player)
			{
				float X = Player->GetActorLocation().Y;
				float Y = Player->GetActorLocation().X;

				X = X / MapSize * Size.X;
				Y = Size.Y - Y / MapSize * Size.Y;

				if(AAvatar* Avatar = Cast<AAvatar>(Player))
				{
					Canvas->DrawTile(Avatar->GetTeamId() == PS->GetTeamId() ? AllyTexture : EnemyTexture, X - HalfUnitSize, Y - HalfUnitSize, UnitSize, UnitSize, 0, 0, Size.X, Size.Y, BLEND_Opaque);
				}
			}
		}
	}
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
	MinimapImage->SetBrushFromMaterial(Material);
}

void UMinimap::BindGridUpdate()
{
	if(AGridManager* GM = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(this, AGridManager::StaticClass())))
	{
		GM->OnGridUpdate.AddUObject(this, &UMinimap::OnGridUpdate);
	}
	//FTimerHandle Handle;
	//GetWorld()->GetTimerManager().SetTimer(Handle, this, &UMinimap::Request, 5.f, false);
}

void UMinimap::OnGridUpdate(const FIntVector2& Position, EEnvironmentType EnvironmentType)
{
	if(EnvironmentTextures.Contains(EnvironmentType))
	{
		UCanvas* Canvas;
		FDrawToRenderTargetContext Context;
		FVector2D Size;
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
			this,
			MapRenderTarget,
			Canvas,
			Size,
			Context
			);

		Canvas->DrawTile(EnvironmentTextures[EnvironmentType], Position.X, Position.Y, 1, 1, 0, 0, 1, 1, BLEND_Opaque);
		
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
		MinimapImage->SetBrushFromMaterial(Material);
	}
}

void UMinimap::Request()
{
	if(ARTPlayerState* PS = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->GetPlayerState<ARTPlayerState>())
	{
		PS->S_RequestInitGrid();
	}
}

void UMinimap::InitGrid(const TArray<EEnvironmentType>& Environments)
{
	for(int x = 0; x < 128; x++)
	{
		for(int y = 0; y < 128; y++)
		{
			if(EnvironmentTextures.Contains(Environments[y * 128 + x]))
			{
				UCanvas* Canvas;
				FDrawToRenderTargetContext Context;
				FVector2D Size;
				UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
					this,
					MapRenderTarget,
					Canvas,
					Size,
					Context
					);

				Canvas->DrawTile(EnvironmentTextures[Environments[y * 128 + x]], x, y, 1, 1, 0, 0, 1, 1, BLEND_Opaque);
				UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
			}
		}
	}
	MinimapImage->SetBrushFromMaterial(Material);
}
