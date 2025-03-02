# Widget Interaction

This document outlines patterns and techniques for handling user input, focus management, and interaction states in UMG widgets using C++.

## Overview

Proper widget interaction handling is crucial for creating responsive and user-friendly UIs. This pattern focuses on managing input events, focus navigation, drag and drop operations, and interaction feedback in UMG widgets.

## Implementation

### 1. Interactive Widget Base Class

A base class that provides common functionality for interactive widgets.

```cpp
// InteractiveWidgetBase.h
#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "InteractiveWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetFocusChanged, bool, bHasFocus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetHoverChanged, bool, bIsHovered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetEnabledChanged, bool, bIsEnabled);

/**
 * Base class for interactive widgets.
 * Provides common functionality for handling user input and interaction states.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UInteractiveWidgetBase : public UBaseWidget
{
    GENERATED_BODY()

public:
    UInteractiveWidgetBase(const FObjectInitializer& ObjectInitializer);

    // Set widget enabled state
    UFUNCTION(BlueprintCallable, Category = "Widget Interaction")
    virtual void SetWidgetEnabled(bool bInIsEnabled);

    // Set widget focused state
    UFUNCTION(BlueprintCallable, Category = "Widget Interaction")
    virtual void SetWidgetFocused(bool bInIsFocused);

    // Get current interaction state
    UFUNCTION(BlueprintCallable, Category = "Widget Interaction")
    bool IsWidgetEnabled() const { return bIsEnabled; }

    UFUNCTION(BlueprintCallable, Category = "Widget Interaction")
    bool IsWidgetFocused() const { return bIsFocused; }

    UFUNCTION(BlueprintCallable, Category = "Widget Interaction")
    bool IsWidgetHovered() const { return bIsHovered; }

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Widget Interaction")
    FOnWidgetFocusChanged OnFocusChanged;

    UPROPERTY(BlueprintAssignable, Category = "Widget Interaction")
    FOnWidgetHoverChanged OnHoverChanged;

    UPROPERTY(BlueprintAssignable, Category = "Widget Interaction")
    FOnWidgetEnabledChanged OnEnabledChanged;

protected:
    // Override input event handlers
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnFocusReceived(const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

    // State flags
    UPROPERTY(BlueprintReadOnly, Category = "Widget Interaction")
    bool bIsEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Widget Interaction")
    bool bIsFocused;

    UPROPERTY(BlueprintReadOnly, Category = "Widget Interaction")
    bool bIsHovered;

    UPROPERTY(BlueprintReadOnly, Category = "Widget Interaction")
    bool bIsPressed;

    // Update visual state
    virtual void UpdateVisualState();

    // Handle state changes
    virtual void OnStateChanged();
};
```

```cpp
// InteractiveWidgetBase.cpp
#include "UI/InteractiveWidgetBase.h"

UInteractiveWidgetBase::UInteractiveWidgetBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsEnabled(true)
    , bIsFocused(false)
    , bIsHovered(false)
    , bIsPressed(false)
{
}

void UInteractiveWidgetBase::SetWidgetEnabled(bool bInIsEnabled)
{
    if (bIsEnabled != bInIsEnabled)
    {
        bIsEnabled = bInIsEnabled;
        OnEnabledChanged.Broadcast(bIsEnabled);
        OnStateChanged();
    }
}

void UInteractiveWidgetBase::SetWidgetFocused(bool bInIsFocused)
{
    if (bIsFocused != bInIsFocused)
    {
        bIsFocused = bInIsFocused;
        OnFocusChanged.Broadcast(bIsFocused);
        OnStateChanged();
    }
}

FReply UInteractiveWidgetBase::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!bIsEnabled)
    {
        return FReply::Unhandled();
    }

    return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UInteractiveWidgetBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (bIsEnabled)
    {
        bIsHovered = true;
        OnHoverChanged.Broadcast(true);
        OnStateChanged();
    }
}

void UInteractiveWidgetBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (bIsEnabled)
    {
        bIsHovered = false;
        bIsPressed = false;
        OnHoverChanged.Broadcast(false);
        OnStateChanged();
    }
}

FReply UInteractiveWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!bIsEnabled)
    {
        return FReply::Unhandled();
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsPressed = true;
        OnStateChanged();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UInteractiveWidgetBase::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!bIsEnabled)
    {
        return FReply::Unhandled();
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsPressed = false;
        OnStateChanged();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInteractiveWidgetBase::NativeOnFocusReceived(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnFocusReceived(InFocusEvent);

    if (bIsEnabled)
    {
        SetWidgetFocused(true);
    }
}

void UInteractiveWidgetBase::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnFocusLost(InFocusEvent);

    if (bIsEnabled)
    {
        SetWidgetFocused(false);
    }
}

void UInteractiveWidgetBase::UpdateVisualState()
{
    // Override in derived classes to update visuals based on state
}

void UInteractiveWidgetBase::OnStateChanged()
{
    UpdateVisualState();
}
```

### 2. Focus Navigation System

A system for managing keyboard/gamepad focus navigation between widgets.

```cpp
// FocusNavigationSystem.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FocusNavigationSystem.generated.h"

UENUM(BlueprintType)
enum class EFocusNavigationDirection : uint8
{
    Up,
    Down,
    Left,
    Right,
    Next,
    Previous
};

/**
 * System for managing focus navigation between widgets.
 */
UCLASS()
class YOURGAME_API UFocusNavigationSystem : public UObject
{
    GENERATED_BODY()

public:
    // Set the root widget container
    UFUNCTION(BlueprintCallable, Category = "Focus Navigation")
    void SetRootWidget(class UWidget* InRootWidget);

    // Navigate focus in specified direction
    UFUNCTION(BlueprintCallable, Category = "Focus Navigation")
    bool NavigateFocus(EFocusNavigationDirection Direction);

    // Set custom navigation path
    UFUNCTION(BlueprintCallable, Category = "Focus Navigation")
    void SetCustomNavigation(class UWidget* FromWidget, class UWidget* ToWidget, EFocusNavigationDirection Direction);

    // Clear custom navigation paths
    UFUNCTION(BlueprintCallable, Category = "Focus Navigation")
    void ClearCustomNavigation(class UWidget* Widget);

private:
    // Root widget reference
    UPROPERTY()
    TWeakObjectPtr<class UWidget> RootWidget;

    // Custom navigation paths
    TMap<TWeakObjectPtr<UWidget>, TMap<EFocusNavigationDirection, TWeakObjectPtr<UWidget>>> CustomNavigationPaths;

    // Find next focusable widget
    class UWidget* FindNextFocusableWidget(class UWidget* CurrentWidget, EFocusNavigationDirection Direction);

    // Check if widget is focusable
    bool IsWidgetFocusable(class UWidget* Widget) const;
};
```

### 3. Drag and Drop System

A system for handling drag and drop operations between widgets.

```cpp
// DragDropOperation.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "CustomDragDropOperation.generated.h"

/**
 * Custom drag and drop operation.
 */
UCLASS()
class YOURGAME_API UCustomDragDropOperation : public UDragDropOperation
{
    GENERATED_BODY()

public:
    // The data being dragged
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag Drop")
    UObject* DraggedData;

    // The source widget
    UPROPERTY()
    TWeakObjectPtr<class UWidget> SourceWidget;

    // Custom payload data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag Drop")
    FString PayloadData;
};
```

```cpp
// DraggableWidget.h
#pragma once

#include "CoreMinimal.h"
#include "UI/InteractiveWidgetBase.h"
#include "DraggableWidget.generated.h"

/**
 * Base class for draggable widgets.
 */
UCLASS()
class YOURGAME_API UDraggableWidget : public UInteractiveWidgetBase
{
    GENERATED_BODY()

public:
    // Check if widget can be dragged
    UFUNCTION(BlueprintCallable, Category = "Drag Drop")
    virtual bool CanBeDragged() const;

    // Get drag visual widget
    UFUNCTION(BlueprintCallable, Category = "Drag Drop")
    virtual UUserWidget* GetDragVisual();

    // Get drag operation class
    UFUNCTION(BlueprintCallable, Category = "Drag Drop")
    virtual TSubclassOf<UDragDropOperation> GetDragOperationClass() const;

protected:
    // Override mouse event handlers
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

    // Drag operation class
    UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
    TSubclassOf<UCustomDragDropOperation> DragOperationClass;

    // Drag visual widget class
    UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
    TSubclassOf<UUserWidget> DragVisualClass;
};
```

```cpp
// DropTargetWidget.h
#pragma once

#include "CoreMinimal.h"
#include "UI/InteractiveWidgetBase.h"
#include "DropTargetWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDropEvent, UDragDropOperation*, Operation, bool, bWasDropped);

/**
 * Base class for drop target widgets.
 */
UCLASS()
class YOURGAME_API UDropTargetWidget : public UInteractiveWidgetBase
{
    GENERATED_BODY()

public:
    // Check if can accept dropped item
    UFUNCTION(BlueprintCallable, Category = "Drag Drop")
    virtual bool CanAcceptDrop(UDragDropOperation* Operation) const;

    // Handle drop event
    UFUNCTION(BlueprintCallable, Category = "Drag Drop")
    virtual bool HandleDrop(UDragDropOperation* Operation);

    // Drop event delegate
    UPROPERTY(BlueprintAssignable, Category = "Drag Drop")
    FOnDropEvent OnDrop;

protected:
    // Override drag and drop event handlers
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    // Visual feedback for drag operations
    virtual void UpdateDropTargetVisuals(bool bIsAcceptableTarget);
};
```

### 4. Input Binding System

A system for binding input actions to widget functions.

```cpp
// WidgetInputBinding.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetInputBinding.generated.h"

USTRUCT(BlueprintType)
struct FWidgetInputAction
{
    GENERATED_BODY()

    // Input action name
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    FName ActionName;

    // Input event type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TEnumAsByte<EInputEvent> EventType;

    // Whether the action requires the widget to be focused
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bRequiresFocus;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputActionTriggered, const FWidgetInputAction&, Action);

/**
 * System for binding input actions to widgets.
 */
UCLASS()
class YOURGAME_API UWidgetInputBinding : public UObject
{
    GENERATED_BODY()

public:
    // Bind input action
    UFUNCTION(BlueprintCallable, Category = "Widget Input")
    void BindInputAction(const FWidgetInputAction& Action);

    // Unbind input action
    UFUNCTION(BlueprintCallable, Category = "Widget Input")
    void UnbindInputAction(const FName& ActionName);

    // Check if input action is bound
    UFUNCTION(BlueprintCallable, Category = "Widget Input")
    bool IsInputActionBound(const FName& ActionName) const;

    // Input action triggered event
    UPROPERTY(BlueprintAssignable, Category = "Widget Input")
    FOnInputActionTriggered OnInputActionTriggered;

private:
    // Bound input actions
    TMap<FName, FWidgetInputAction> BoundActions;

    // Handle input action
    void HandleInputAction(const FWidgetInputAction& Action);
};
```

## Usage Examples

### 1. Creating an Interactive Button

```cpp
// CustomButton.h
#pragma once

#include "CoreMinimal.h"
#include "UI/InteractiveWidgetBase.h"
#include "CustomButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClicked);

/**
 * Example of a custom interactive button.
 */
UCLASS()
class YOURGAME_API UCustomButton : public UInteractiveWidgetBase
{
    GENERATED_BODY()

public:
    UCustomButton(const FObjectInitializer& ObjectInitializer);

    // Button click event
    UPROPERTY(BlueprintAssignable, Category = "Button")
    FOnButtonClicked OnClicked;

protected:
    // Override mouse event handlers
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // Override state change handler
    virtual void OnStateChanged() override;

    // Button background
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UImage* Background;

    // Button text
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* ButtonText;

private:
    // Update button appearance based on state
    void UpdateButtonAppearance();
};
```

### 2. Implementing Drag and Drop

```cpp
// InventorySlot.h
#pragma once

#include "CoreMinimal.h"
#include "UI/DraggableWidget.h"
#include "InventorySlot.generated.h"

/**
 * Example of a draggable inventory slot.
 */
UCLASS()
class YOURGAME_API UInventorySlot : public UDraggableWidget
{
    GENERATED_BODY()

public:
    // Set slot item
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetItem(class UInventoryItem* Item);

    // Get slot item
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    class UInventoryItem* GetItem() const;

protected:
    // Override drag and drop functionality
    virtual bool CanBeDragged() const override;
    virtual UUserWidget* GetDragVisual() override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

private:
    // Current item
    UPROPERTY()
    class UInventoryItem* CurrentItem;
};
```

### 3. Setting Up Focus Navigation

```cpp
// MenuWidget.cpp
void UMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Set up focus navigation
    UFocusNavigationSystem* NavSystem = NewObject<UFocusNavigationSystem>(this);
    NavSystem->SetRootWidget(this);

    // Set custom navigation paths
    NavSystem->SetCustomNavigation(ContinueButton, OptionsButton, EFocusNavigationDirection::Down);
    NavSystem->SetCustomNavigation(OptionsButton, QuitButton, EFocusNavigationDirection::Down);
    NavSystem->SetCustomNavigation(QuitButton, ContinueButton, EFocusNavigationDirection::Down);

    // Set initial focus
    if (ContinueButton)
    {
        ContinueButton->SetFocus();
    }
}
```

## Best Practices

1. **Handle All Input States**: Account for all possible input states and transitions.

2. **Provide Visual Feedback**: Give clear visual feedback for all interaction states.

3. **Support Multiple Input Methods**: Design interactions to work with mouse, keyboard, and gamepad.

4. **Implement Focus Indicators**: Clearly indicate which widget has focus.

5. **Use Input Prioritization**: Properly handle input priority between overlapping widgets.

6. **Maintain Accessibility**: Ensure widgets are accessible via keyboard navigation.

7. **Handle Edge Cases**: Account for interrupted interactions and invalid states.

## Common Pitfalls

1. **Input Blocking**: Not properly handling input event propagation.

2. **Focus Management**: Losing track of focus state during navigation.

3. **Memory Management**: Not cleaning up input bindings and event handlers.

4. **Performance Issues**: Over-updating visual states during interaction.

5. **Inconsistent Behavior**: Different behavior between input methods.

## Related Patterns

- [Widget Base Classes](Pattern1_WidgetBaseClasses.md)
- [Widget Animation in C++](Pattern5_WidgetAnimationCPP.md)
- [Performance Optimization](Pattern7_PerformanceOptimization.md) 