// Copyright Radiant Studios


#include "UI/Minimap.h"

#include "CookerSettings.h"
#include "Components/Image.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
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
		float MapSize = 12000.f;
		for(AActor* Player : Players)
		{
			if(Player)
			{
				float X = Player->GetActorLocation().Y + MapSize / 2.f;
				float Y = Player->GetActorLocation().X + MapSize / 2.f;

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

void UMinimap::DrawStatic()
{
}
