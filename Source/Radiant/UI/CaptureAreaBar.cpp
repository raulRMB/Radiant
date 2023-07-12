// Copyright Radiant Studios


#include "UI/CaptureAreaBar.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UCaptureAreaBar::SetTeamPercent(ETeamId TeamId, float NewPercent)
{
	UCanvasPanelSlot* NeedleSlot = Cast<UCanvasPanelSlot>(Needle->Slot);
	NeedleSlot->SetPosition(FVector2D(
		NewPercent * Cast<UCanvasPanelSlot>(Panel->Slot)->GetSize().X - NeedleSlot->GetSize().X * .5f,
		NeedleSlot->GetPosition().Y));

	switch (TeamId)
	{
	case ETeamId::Blue:
		BlueBar->SetPercent(NewPercent);
		RedBar->SetPercent(0.f);
		break;
	case ETeamId::Red:
		RedBar->SetPercent(1 - NewPercent);
		BlueBar->SetPercent(0.f);
		break;
	case ETeamId::Neutral:
		BlueBar->SetPercent(0.f);
		RedBar->SetPercent(0.f);
		break;
	}
}
