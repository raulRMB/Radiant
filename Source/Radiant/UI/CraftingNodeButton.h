#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

DECLARE_DELEGATE(FRightClickSignature);

class SCraftingNodeButton : public SButton
{
protected:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
public:
	FRightClickSignature OnRightClick;
};
