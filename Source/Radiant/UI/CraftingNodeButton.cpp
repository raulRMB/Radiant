#include "CraftingNodeButton.h"

FReply SCraftingNodeButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply =  SButton::OnMouseButtonDown(MyGeometry, MouseEvent);

	if(MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnRightClick.ExecuteIfBound();
	}
	
	return Reply;
}
