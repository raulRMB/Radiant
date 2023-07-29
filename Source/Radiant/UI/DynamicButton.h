#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DynamicButton.generated.h"

DECLARE_MULTICAST_DELEGATE(FClickSignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FDynamicClickSignature, const FPointerEvent& /*InMouseEvent*/)

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EMouseButtonMask : uint8
{
	None		= 0,
	LeftClick	= 1 << 1,
	RightClick	= 1 << 2,
	MiddleClick = 1 << 3,
	Shift		= 1 << 4,
	Control		= 1 << 5,
	Alt			= 1 << 6
};
ENUM_CLASS_FLAGS(EMouseButtonMask)

UCLASS()
class UDynamicButton : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Bitmask, BitmaskEnum="/Script/Radiant.EMouseButtonMask", AllowPrivateAccess=true))
	int32 ButtonMask = 0;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* CanvasPanel;

	UPROPERTY(meta=(BindWidget))
	class UImage* Image;
	
	UPROPERTY(EditAnywhere)
	FLinearColor NormalColor = FLinearColor::White;
	
	UPROPERTY(EditAnywhere)
	FLinearColor HoveredColor = FLinearColor::White * 0.5f;

	UPROPERTY(EditAnywhere)
	FLinearColor PressedColor = FLinearColor::White * 0.2f;

	UPROPERTY(EditAnywhere)
	uint8 bIsDoubleClickable : 1;
private:
	uint8 bIsPressed : 1;
	uint8 bIsHovered : 1;
	
public:
	
	void ListenTooButton(EMouseButtonMask InMouseButtonMask, bool bListenTo);
	void ListenToAllButtons(bool bListenTo = true) { ButtonMask = bListenTo ? INT32_MAX : 0; }
	void SetButtonMask(int32 InListenTo) { ButtonMask = InListenTo; }
	int32 GetButtonMask() const { return ButtonMask; }
	void SetIsDoubleClickable(bool bInIsDoubleClickable) { bIsDoubleClickable = bInIsDoubleClickable; }
	
public:
	
	FClickSignature OnRightClick;
	FClickSignature OnLeftClick;
	FClickSignature OnMiddleClick;

	FClickSignature OnShiftRightClick;
	FClickSignature OnShiftLeftClick;
	FClickSignature OnShiftMiddleClick;
	
	FClickSignature OnControlRightClick;
	FClickSignature OnControlLeftClick;
	FClickSignature OnControlMiddleClick;

	FClickSignature OnAltRightClick;
	FClickSignature OnAltLeftClick;
	FClickSignature OnAltMiddleClick;

	FDynamicClickSignature OnDynamicClick;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual 
	
	bool CheckButtonMask(const EMouseButtonMask Mask) const;
};
