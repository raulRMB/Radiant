#include "DynamicButton.h"

#include "Components/Image.h"
#include "Util/Util.h"

void UDynamicButton::ListenTooButton(EMouseButtonMask InMouseButtonMask, bool bListenTo)
{
	ButtonMask = bListenTo ? ButtonMask | static_cast<int32>(InMouseButtonMask) : ButtonMask & ~static_cast<int32>(InMouseButtonMask);
}

bool UDynamicButton::CheckButtonMask(const EMouseButtonMask Mask) const
{
	int32 Result = ButtonMask & static_cast<int32>(Mask);
	return Result == static_cast<int32>(Mask);
}

FReply UDynamicButton::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bool bShouldPress = bIsPressed;	
	if(CheckButtonMask(EMouseButtonMask::LeftClick) && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if(CheckButtonMask(EMouseButtonMask::Shift) && InMouseEvent.GetModifierKeys().IsShiftDown())
		{
			OnShiftLeftClick.Broadcast();
		}
		else if(CheckButtonMask(EMouseButtonMask::Control) && InMouseEvent.GetModifierKeys().IsControlDown())
		{
			OnControlLeftClick.Broadcast();
		}
		else if(CheckButtonMask(EMouseButtonMask::Alt) && InMouseEvent.GetModifierKeys().IsAltDown())
		{
			OnAltLeftClick.Broadcast();
		}
		else
		{
			OnLeftClick.Broadcast();
		}
		bShouldPress = true;
	}
	else if(CheckButtonMask(EMouseButtonMask::RightClick) && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if(CheckButtonMask(EMouseButtonMask::Shift) && InMouseEvent.GetModifierKeys().IsShiftDown())
		{
			OnShiftRightClick.Broadcast();
		}
		else if(CheckButtonMask(EMouseButtonMask::Control) && InMouseEvent.GetModifierKeys().IsControlDown())
		{
			OnControlRightClick.Broadcast();
		}
		else if(CheckButtonMask(EMouseButtonMask::Alt) && InMouseEvent.GetModifierKeys().IsAltDown())
		{
			OnControlRightClick.Broadcast();
		}
		else
		{
			OnRightClick.Broadcast();
		}
		bShouldPress = true;
	}
	else if(CheckButtonMask(EMouseButtonMask::MiddleClick) && InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
	{
		if(CheckButtonMask(EMouseButtonMask::Shift) && InMouseEvent.GetModifierKeys().IsShiftDown())
		{
			OnShiftMiddleClick.Broadcast();
		}
		else if(CheckButtonMask(EMouseButtonMask::Control) && InMouseEvent.GetModifierKeys().IsControlDown())
		{
			OnControlMiddleClick.Broadcast();
		}
		else if(CheckButtonMask(EMouseButtonMask::Alt) && InMouseEvent.GetModifierKeys().IsAltDown())
		{
			OnAltMiddleClick.Broadcast();
		}
		else
		{
			OnMiddleClick.Broadcast();
		}
		bShouldPress = true;
	}

	bIsPressed = bShouldPress;
	
	return FReply::Handled();
}

FReply UDynamicButton::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bool bShouldPress = bIsPressed;
	if(CheckButtonMask(EMouseButtonMask::LeftClick) && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bShouldPress = false;
	}
	else if(CheckButtonMask(EMouseButtonMask::RightClick) && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bShouldPress = false;
	}
	else if(CheckButtonMask(EMouseButtonMask::MiddleClick) && InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
	{
		bShouldPress = false;
	}
	bIsPressed = bShouldPress;
	
	return FReply::Handled();
}

FReply UDynamicButton::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(!bIsDoubleClickable)
	{
		NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}
	
	return FReply::Handled();
}

void UDynamicButton::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{	
	bIsHovered = true;
}

void UDynamicButton::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	bIsHovered = false;
	bIsPressed = false;
}

int32 UDynamicButton::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if(bIsPressed)
	{
		Image->SetColorAndOpacity(PressedColor);
	}
	else if(bIsHovered)
	{
		Image->SetColorAndOpacity(HoveredColor);
	}
	else
	{
		Image->SetColorAndOpacity(NormalColor);
	}
	
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);
}
