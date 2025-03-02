# Widget Base Classes

This document outlines the foundation classes for different UMG widget types in C++, providing a solid structure for your UI development.

## Overview

Creating well-structured base classes for your UMG widgets helps establish consistency, reusability, and maintainability across your project. This pattern focuses on creating a hierarchy of widget base classes that serve different purposes.

## Base Class Hierarchy

```
UUserWidget (Engine)
└── UBaseWidget
    ├── UDialogWidget
    ├── UHUDWidget
    ├── UMenuWidget
    └── UInteractiveWidget
```

## Implementation

### 1. Base Widget Class

This serves as the foundation for all custom widgets in your project.

```cpp
// BaseWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseWidget.generated.h"

/**
 * Base class for all widgets in the project.
 * Provides common functionality and interfaces.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UBaseWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UBaseWidget(const FObjectInitializer& ObjectInitializer);

    // Common initialization method
    UFUNCTION(BlueprintCallable, Category = "Widget")
    virtual void InitializeWidget();

    // Common cleanup method
    UFUNCTION(BlueprintCallable, Category = "Widget")
    virtual void CleanupWidget();

    // Show widget with optional animation
    UFUNCTION(BlueprintCallable, Category = "Widget")
    virtual void ShowWidget(bool bPlayAnimation = true);

    // Hide widget with optional animation
    UFUNCTION(BlueprintCallable, Category = "Widget")
    virtual void HideWidget(bool bPlayAnimation = true);

protected:
    // Override NativeConstruct for initialization
    virtual void NativeConstruct() override;

    // Override NativeDestruct for cleanup
    virtual void NativeDestruct() override;

    // Animation for showing the widget
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ShowAnimation;

    // Animation for hiding the widget
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* HideAnimation;

    // Flag to track if the widget has been initialized
    bool bIsInitialized;
};
```

```cpp
// BaseWidget.cpp
#include "UI/BaseWidget.h"
#include "Animation/WidgetAnimation.h"

UBaseWidget::UBaseWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsInitialized(false)
{
}

void UBaseWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (!bIsInitialized)
    {
        InitializeWidget();
    }
}

void UBaseWidget::NativeDestruct()
{
    CleanupWidget();
    Super::NativeDestruct();
}

void UBaseWidget::InitializeWidget()
{
    bIsInitialized = true;
}

void UBaseWidget::CleanupWidget()
{
    // Base implementation does nothing
}

void UBaseWidget::ShowWidget(bool bPlayAnimation)
{
    SetVisibility(ESlateVisibility::Visible);
    
    if (bPlayAnimation && ShowAnimation)
    {
        PlayAnimation(ShowAnimation);
    }
}

void UBaseWidget::HideWidget(bool bPlayAnimation)
{
    if (bPlayAnimation && HideAnimation)
    {
        // Play the hide animation and set visibility to hidden when complete
        PlayAnimation(HideAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
        
        // Set up a delegate to hide the widget when the animation completes
        FWidgetAnimationDynamicEvent AnimationFinishedEvent;
        AnimationFinishedEvent.BindDynamic(this, &UBaseWidget::OnHideAnimationFinished);
        BindToAnimationFinished(HideAnimation, AnimationFinishedEvent);
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void UBaseWidget::OnHideAnimationFinished()
{
    SetVisibility(ESlateVisibility::Hidden);
}
```

### 2. Dialog Widget Class

Specialized for dialog/popup windows.

```cpp
// DialogWidget.h
#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "DialogWidget.generated.h"

/**
 * Base class for dialog/popup widgets.
 * Provides functionality for modal dialogs with standard controls.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UDialogWidget : public UBaseWidget
{
    GENERATED_BODY()

public:
    UDialogWidget(const FObjectInitializer& ObjectInitializer);

    // Initialize with a title and message
    UFUNCTION(BlueprintCallable, Category = "Dialog")
    virtual void InitializeDialog(const FText& Title, const FText& Message);

    // Set dialog result and close
    UFUNCTION(BlueprintCallable, Category = "Dialog")
    virtual void CloseDialog(bool bWasConfirmed = false);

    // Delegate for dialog result
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogResultDelegate, bool, bWasConfirmed);
    
    // Event fired when dialog is closed
    UPROPERTY(BlueprintAssignable, Category = "Dialog")
    FOnDialogResultDelegate OnDialogResult;

protected:
    // Override to add backdrop dimming
    virtual void ShowWidget(bool bPlayAnimation = true) override;
    
    // Override to remove backdrop dimming
    virtual void HideWidget(bool bPlayAnimation = true) override;

    // Title text component
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* TitleText;

    // Message text component
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* MessageText;

    // Confirm button
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UButton* ConfirmButton;

    // Cancel button
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UButton* CancelButton;

    // Close button
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UButton* CloseButton;

private:
    // Button click handlers
    UFUNCTION()
    void OnConfirmButtonClicked();

    UFUNCTION()
    void OnCancelButtonClicked();

    UFUNCTION()
    void OnCloseButtonClicked();
};
```

### 3. HUD Widget Class

Specialized for in-game HUD elements.

```cpp
// HUDWidget.h
#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "HUDWidget.generated.h"

/**
 * Base class for HUD widgets.
 * Provides functionality for in-game UI elements.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UHUDWidget : public UBaseWidget
{
    GENERATED_BODY()

public:
    UHUDWidget(const FObjectInitializer& ObjectInitializer);

    // Update HUD with player data
    UFUNCTION(BlueprintCallable, Category = "HUD")
    virtual void UpdateHUD(class APlayerCharacter* PlayerCharacter);

    // Toggle HUD visibility
    UFUNCTION(BlueprintCallable, Category = "HUD")
    virtual void ToggleHUDVisibility();

    // Set HUD opacity
    UFUNCTION(BlueprintCallable, Category = "HUD")
    virtual void SetHUDOpacity(float Opacity);

protected:
    // Override for HUD-specific initialization
    virtual void InitializeWidget() override;

    // Reference to the player character
    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    TWeakObjectPtr<class APlayerCharacter> OwningPlayerCharacter;

    // Current HUD opacity
    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    float CurrentOpacity;
};
```

### 4. Menu Widget Class

Specialized for menu screens.

```cpp
// MenuWidget.h
#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "MenuWidget.generated.h"

/**
 * Base class for menu widgets.
 * Provides functionality for navigation and menu management.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UMenuWidget : public UBaseWidget
{
    GENERATED_BODY()

public:
    UMenuWidget(const FObjectInitializer& ObjectInitializer);

    // Navigate to a submenu
    UFUNCTION(BlueprintCallable, Category = "Menu")
    virtual void NavigateToSubmenu(TSubclassOf<UMenuWidget> SubmenuClass);

    // Return to previous menu
    UFUNCTION(BlueprintCallable, Category = "Menu")
    virtual void ReturnToPreviousMenu();

    // Set the parent menu
    UFUNCTION(BlueprintCallable, Category = "Menu")
    virtual void SetParentMenu(UMenuWidget* ParentMenuWidget);

protected:
    // Override for menu-specific initialization
    virtual void InitializeWidget() override;

    // Parent menu reference for navigation
    UPROPERTY(BlueprintReadOnly, Category = "Menu")
    TWeakObjectPtr<UMenuWidget> ParentMenu;

    // Current submenu reference
    UPROPERTY(BlueprintReadOnly, Category = "Menu")
    TWeakObjectPtr<UMenuWidget> CurrentSubmenu;
};
```

### 5. Interactive Widget Class

Specialized for interactive UI elements like inventory slots.

```cpp
// InteractiveWidget.h
#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InteractiveWidget.generated.h"

/**
 * Base class for interactive widgets like inventory slots.
 * Implements drag and drop functionality.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UInteractiveWidget : public UBaseWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    UInteractiveWidget(const FObjectInitializer& ObjectInitializer);

    // Set the data object for this widget
    UFUNCTION(BlueprintCallable, Category = "Interactive Widget")
    virtual void SetDataObject(UObject* InDataObject);

    // Get the data object for this widget
    UFUNCTION(BlueprintCallable, Category = "Interactive Widget")
    virtual UObject* GetDataObject() const;

    // IUserObjectListEntry interface
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
    // Override for interactive widget initialization
    virtual void InitializeWidget() override;

    // Data object for this widget
    UPROPERTY(BlueprintReadOnly, Category = "Interactive Widget")
    TWeakObjectPtr<UObject> DataObject;

    // Background image
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UImage* BackgroundImage;

    // Selection border
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UImage* SelectionBorder;

    // Mouse event handlers
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
```

## Usage Examples

### Creating a Custom Dialog

```cpp
// ConfirmPurchaseDialog.h
#pragma once

#include "CoreMinimal.h"
#include "UI/DialogWidget.h"
#include "ConfirmPurchaseDialog.generated.h"

/**
 * Dialog for confirming purchases in the game.
 */
UCLASS()
class YOURGAME_API UConfirmPurchaseDialog : public UDialogWidget
{
    GENERATED_BODY()

public:
    // Initialize with item information
    UFUNCTION(BlueprintCallable, Category = "Purchase")
    void InitializePurchase(const FText& ItemName, int32 ItemCost, int32 PlayerBalance);

protected:
    // Price text component
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* PriceText;

    // Balance text component
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* BalanceText;

    // Item image component
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UImage* ItemImage;

private:
    // Item cost
    int32 Cost;
    
    // Player's current balance
    int32 Balance;
};
```

### Creating a Custom HUD Element

```cpp
// PlayerHealthBar.h
#pragma once

#include "CoreMinimal.h"
#include "UI/HUDWidget.h"
#include "PlayerHealthBar.generated.h"

/**
 * Widget for displaying player health.
 */
UCLASS()
class YOURGAME_API UPlayerHealthBar : public UHUDWidget
{
    GENERATED_BODY()

public:
    // Update health display
    UFUNCTION(BlueprintCallable, Category = "Health")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

protected:
    // Health progress bar
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UProgressBar* HealthBar;

    // Health text display
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* HealthText;

    // Low health warning image
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UImage* LowHealthWarning;

    // Low health threshold (percentage)
    UPROPERTY(EditDefaultsOnly, Category = "Health", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float LowHealthThreshold = 0.3f;
};
```

## Best Practices

1. **Use the BindWidget Meta Tag**: Use `UPROPERTY(meta = (BindWidget))` to automatically bind C++ properties to UMG widget components with matching names.

2. **Use the BindWidgetAnim Meta Tag**: Use `UPROPERTY(Transient, meta = (BindWidgetAnim))` to automatically bind C++ properties to UMG animations with matching names.

3. **Override NativeConstruct and NativeDestruct**: Use these methods for initialization and cleanup instead of BeginPlay and EndPlay.

4. **Implement Proper Cleanup**: Always clean up any resources, event bindings, or timers in the NativeDestruct method.

5. **Use Weak Pointers**: Use TWeakObjectPtr for references to other objects to prevent memory leaks.

6. **Separate Logic from Presentation**: Keep widget logic separate from game logic to improve maintainability.

7. **Make Widgets Reusable**: Design widgets to be reusable across different contexts by parameterizing their behavior.

## Common Pitfalls

1. **Memory Leaks**: Failing to clean up event bindings or timers can cause memory leaks.

2. **Performance Issues**: Creating and destroying widgets frequently can cause performance issues. Consider using object pooling for frequently used widgets.

3. **Circular References**: Be careful of circular references between widgets and other objects, which can prevent garbage collection.

4. **Thread Safety**: UMG widgets should only be accessed from the game thread.

5. **Missing BindWidget Properties**: If a property marked with BindWidget doesn't match any widget in the UMG editor, it will be null at runtime.

## Related Patterns

- [Data Binding Patterns](Pattern2_DataBindingPatterns.md)
- [Widget Factory System](Pattern3_WidgetFactorySystem.md)
- [Responsive Design](Pattern4_ResponsiveDesign.md) 