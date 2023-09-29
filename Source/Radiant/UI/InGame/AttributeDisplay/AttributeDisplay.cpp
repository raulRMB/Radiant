// Copyright Radiant Studios


#include "AttributeDisplay.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/TextBlock.h"
#include "GAS/Cues/AttributeChangeDisplay.h"

void UAttributeDisplay::Init(const FText& Text, const FLinearColor& Color, AAttributeChangeDisplay* Object)
{
	TextBlock->SetText(Text);
	TextBlock->SetColorAndOpacity(Color);
	if(UUMGSequencePlayer* SP = PlayAnimation(FloatAnim))
	{
		SP->OnSequenceFinishedPlaying().AddUObject(Object, &AAttributeChangeDisplay::OnAnimFinished);
	}
}