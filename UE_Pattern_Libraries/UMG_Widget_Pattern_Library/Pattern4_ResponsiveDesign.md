# Responsive Design

This document outlines patterns and techniques for creating responsive UMG widgets that adapt to different screen sizes and resolutions in C++.

## Overview

Responsive design in UMG ensures that your UI elements maintain their usability and visual appeal across different screen sizes, aspect ratios, and resolutions. This pattern focuses on creating flexible layouts that automatically adjust to the viewing environment.

## Implementation

### 1. Responsive Widget Base Class

A base class that provides common functionality for responsive widgets.

```cpp
// ResponsiveWidget.h
#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "ResponsiveWidget.generated.h"

/**
 * Base class for responsive widgets.
 * Provides functionality for adapting to different screen sizes.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UResponsiveWidget : public UBaseWidget
{
    GENERATED_BODY()

public:
    UResponsiveWidget(const FObjectInitializer& ObjectInitializer);

    // Update layout based on current viewport size
    UFUNCTION(BlueprintCallable, Category = "Responsive Design")
    virtual void UpdateLayout();

    // Get the current viewport size
    UFUNCTION(BlueprintCallable, Category = "Responsive Design")
    void GetViewportSize(FVector2D& ViewportSize) const;

    // Get the current screen resolution scale
    UFUNCTION(BlueprintCallable, Category = "Responsive Design")
    float GetScreenResolutionScale() const;

    // Get the current DPI scale
    UFUNCTION(BlueprintCallable, Category = "Responsive Design")
    float GetDPIScale() const;

protected:
    // Override NativeConstruct for initialization
    virtual void NativeConstruct() override;

    // Called when the viewport size changes
    virtual void OnViewportSizeChanged(const FVector2D& NewViewportSize);

    // Reference resolution for scaling calculations
    UPROPERTY(EditAnywhere, Category = "Responsive Design")
    FVector2D ReferenceResolution;

    // Minimum scale factor
    UPROPERTY(EditAnywhere, Category = "Responsive Design", meta = (ClampMin = "0.1", ClampMax = "1.0"))
    float MinScaleFactor;

    // Maximum scale factor
    UPROPERTY(EditAnywhere, Category = "Responsive Design", meta = (ClampMin = "1.0", ClampMax = "5.0"))
    float MaxScaleFactor;

private:
    // Current viewport size
    FVector2D CurrentViewportSize;

    // Delegate handle for viewport size changes
    FDelegateHandle ViewportResizedHandle;

    // Calculate scale factor based on current viewport size
    float CalculateScaleFactor() const;
};
```

```cpp
// ResponsiveWidget.cpp
#include "UI/ResponsiveWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/GameViewportClient.h"

UResponsiveWidget::UResponsiveWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ReferenceResolution(FVector2D(1920.0f, 1080.0f))
    , MinScaleFactor(0.5f)
    , MaxScaleFactor(2.0f)
{
}

void UResponsiveWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Get initial viewport size
    GetViewportSize(CurrentViewportSize);

    // Bind to viewport resize event
    if (GEngine && GEngine->GameViewport)
    {
        ViewportResizedHandle = GEngine->GameViewport->ViewportResizedEvent.AddUObject(
            this,
            &UResponsiveWidget::OnViewportSizeChanged
        );
    }

    // Initial layout update
    UpdateLayout();
}

void UResponsiveWidget::UpdateLayout()
{
    // Get current viewport size
    FVector2D NewViewportSize;
    GetViewportSize(NewViewportSize);

    // Only update if the size has changed
    if (!CurrentViewportSize.Equals(NewViewportSize))
    {
        CurrentViewportSize = NewViewportSize;
        OnViewportSizeChanged(CurrentViewportSize);
    }
}

void UResponsiveWidget::GetViewportSize(FVector2D& ViewportSize) const
{
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }
    else
    {
        ViewportSize = FVector2D::ZeroVector;
    }
}

float UResponsiveWidget::GetScreenResolutionScale() const
{
    return CalculateScaleFactor();
}

float UResponsiveWidget::GetDPIScale() const
{
    return UWidgetLayoutLibrary::GetViewportScale(GetWorld());
}

void UResponsiveWidget::OnViewportSizeChanged(const FVector2D& NewViewportSize)
{
    // Calculate new scale factor
    float ScaleFactor = CalculateScaleFactor();

    // Apply scale to root widget
    SetRenderScale(FVector2D(ScaleFactor, ScaleFactor));
}

float UResponsiveWidget::CalculateScaleFactor() const
{
    if (ReferenceResolution.X <= 0.0f || ReferenceResolution.Y <= 0.0f)
    {
        return 1.0f;
    }

    // Calculate scale based on width and height ratios
    float WidthRatio = CurrentViewportSize.X / ReferenceResolution.X;
    float HeightRatio = CurrentViewportSize.Y / ReferenceResolution.Y;

    // Use the smaller ratio to ensure the UI fits within the viewport
    float ScaleFactor = FMath::Min(WidthRatio, HeightRatio);

    // Clamp to min/max scale factors
    return FMath::Clamp(ScaleFactor, MinScaleFactor, MaxScaleFactor);
}
```

### 2. Responsive Layout Container

A container widget that automatically arranges its children based on available space.

```cpp
// ResponsiveLayoutContainer.h
#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "ResponsiveLayoutContainer.generated.h"

UENUM(BlueprintType)
enum class EResponsiveLayout : uint8
{
    Flow,       // Children flow horizontally then wrap
    Grid,       // Children arranged in a grid
    Adaptive    // Layout changes based on screen size
};

/**
 * Container widget that arranges children responsively.
 */
UCLASS()
class YOURGAME_API UResponsiveLayoutContainer : public UPanelWidget
{
    GENERATED_BODY()

public:
    UResponsiveLayoutContainer(const FObjectInitializer& ObjectInitializer);

    // Add child to container
    UFUNCTION(BlueprintCallable, Category = "Responsive Layout")
    UWidget* AddChild(UWidget* Widget);

    // Remove child from container
    UFUNCTION(BlueprintCallable, Category = "Responsive Layout")
    bool RemoveChild(UWidget* Widget);

    // Set the layout type
    UFUNCTION(BlueprintCallable, Category = "Responsive Layout")
    void SetLayoutType(EResponsiveLayout NewLayout);

    // Set the spacing between items
    UFUNCTION(BlueprintCallable, Category = "Responsive Layout")
    void SetSpacing(FVector2D NewSpacing);

    // Set the minimum width for children
    UFUNCTION(BlueprintCallable, Category = "Responsive Layout")
    void SetMinChildWidth(float NewMinWidth);

protected:
    // Override layout calculation
    virtual void OnSlotAdded(UPanelSlot* InSlot) override;
    virtual void OnSlotRemoved(UPanelSlot* InSlot) override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;

    // Layout type
    UPROPERTY(EditAnywhere, Category = "Responsive Layout")
    EResponsiveLayout LayoutType;

    // Spacing between items
    UPROPERTY(EditAnywhere, Category = "Responsive Layout")
    FVector2D Spacing;

    // Minimum width for children
    UPROPERTY(EditAnywhere, Category = "Responsive Layout")
    float MinChildWidth;

private:
    // Update layout based on current settings
    void UpdateLayout();

    // Calculate flow layout
    void CalculateFlowLayout();

    // Calculate grid layout
    void CalculateGridLayout();

    // Calculate adaptive layout
    void CalculateAdaptiveLayout();

    // Get the number of columns based on available width
    int32 CalculateOptimalColumns() const;
};
```

### 3. Responsive HUD Layout

A specialized responsive layout for HUD elements.

```cpp
// ResponsiveHUDLayout.h
#pragma once

#include "CoreMinimal.h"
#include "UI/ResponsiveWidget.h"
#include "ResponsiveHUDLayout.generated.h"

UENUM(BlueprintType)
enum class EScreenRegion : uint8
{
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleCenter,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

/**
 * Responsive layout specifically for HUD elements.
 */
UCLASS()
class YOURGAME_API UResponsiveHUDLayout : public UResponsiveWidget
{
    GENERATED_BODY()

public:
    UResponsiveHUDLayout(const FObjectInitializer& ObjectInitializer);

    // Add a widget to a specific screen region
    UFUNCTION(BlueprintCallable, Category = "HUD Layout")
    void AddWidgetToRegion(UWidget* Widget, EScreenRegion Region);

    // Remove a widget from its region
    UFUNCTION(BlueprintCallable, Category = "HUD Layout")
    void RemoveWidgetFromRegion(UWidget* Widget);

    // Set the padding for a specific region
    UFUNCTION(BlueprintCallable, Category = "HUD Layout")
    void SetRegionPadding(EScreenRegion Region, FMargin Padding);

protected:
    // Override viewport size change handler
    virtual void OnViewportSizeChanged(const FVector2D& NewViewportSize) override;

private:
    // Map of regions to their container widgets
    UPROPERTY()
    TMap<EScreenRegion, class UCanvasPanel*> RegionContainers;

    // Map of regions to their padding
    TMap<EScreenRegion, FMargin> RegionPadding;

    // Initialize region containers
    void InitializeRegions();

    // Update region layouts
    void UpdateRegionLayouts();

    // Get the anchor points for a region
    void GetRegionAnchors(EScreenRegion Region, FVector2D& MinAnchor, FVector2D& MaxAnchor);
};
```

### 4. Safe Area Widget

A widget that respects device-specific safe areas (e.g., notches on mobile devices).

```cpp
// SafeAreaWidget.h
#pragma once

#include "CoreMinimal.h"
#include "UI/ResponsiveWidget.h"
#include "SafeAreaWidget.generated.h"

/**
 * Widget that automatically adjusts its content to respect safe areas.
 */
UCLASS()
class YOURGAME_API USafeAreaWidget : public UResponsiveWidget
{
    GENERATED_BODY()

public:
    USafeAreaWidget(const FObjectInitializer& ObjectInitializer);

    // Set whether to pad for the safe area
    UFUNCTION(BlueprintCallable, Category = "Safe Area")
    void SetApplySafeArea(bool bApply);

    // Set additional padding
    UFUNCTION(BlueprintCallable, Category = "Safe Area")
    void SetAdditionalPadding(const FMargin& NewPadding);

protected:
    // Override viewport size change handler
    virtual void OnViewportSizeChanged(const FVector2D& NewViewportSize) override;

    // Whether to apply safe area padding
    UPROPERTY(EditAnywhere, Category = "Safe Area")
    bool bApplySafeArea;

    // Additional padding beyond safe area
    UPROPERTY(EditAnywhere, Category = "Safe Area")
    FMargin AdditionalPadding;

private:
    // Get the current safe area
    FMargin GetSafeArea() const;

    // Update padding based on safe area
    void UpdateSafeAreaPadding();
};
```

## Usage Examples

### 1. Creating a Responsive Menu

```cpp
// ResponsiveMainMenu.h
#pragma once

#include "CoreMinimal.h"
#include "UI/ResponsiveWidget.h"
#include "ResponsiveMainMenu.generated.h"

/**
 * Example of a responsive main menu.
 */
UCLASS()
class YOURGAME_API UResponsiveMainMenu : public UResponsiveWidget
{
    GENERATED_BODY()

protected:
    // Menu container
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UResponsiveLayoutContainer* MenuContainer;

    // Button container
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UResponsiveLayoutContainer* ButtonContainer;

    // Override viewport size change handler
    virtual void OnViewportSizeChanged(const FVector2D& NewViewportSize) override;

private:
    // Update layout based on screen size
    void UpdateMenuLayout(const FVector2D& ViewportSize);
};
```

```cpp
// ResponsiveMainMenu.cpp
#include "UI/ResponsiveMainMenu.h"
#include "UI/ResponsiveLayoutContainer.h"

void UResponsiveMainMenu::OnViewportSizeChanged(const FVector2D& NewViewportSize)
{
    Super::OnViewportSizeChanged(NewViewportSize);
    UpdateMenuLayout(NewViewportSize);
}

void UResponsiveMainMenu::UpdateMenuLayout(const FVector2D& ViewportSize)
{
    // Adjust layout based on screen width
    if (ViewportSize.X < 1280.0f)
    {
        // Mobile/small screen layout
        if (MenuContainer)
        {
            MenuContainer->SetLayoutType(EResponsiveLayout::Flow);
        }
        if (ButtonContainer)
        {
            ButtonContainer->SetLayoutType(EResponsiveLayout::Grid);
            ButtonContainer->SetMinChildWidth(150.0f);
        }
    }
    else
    {
        // Desktop/large screen layout
        if (MenuContainer)
        {
            MenuContainer->SetLayoutType(EResponsiveLayout::Adaptive);
        }
        if (ButtonContainer)
        {
            ButtonContainer->SetLayoutType(EResponsiveLayout::Flow);
            ButtonContainer->SetMinChildWidth(200.0f);
        }
    }
}
```

### 2. Creating a Responsive HUD

```cpp
// GameHUD.h
#pragma once

#include "CoreMinimal.h"
#include "UI/ResponsiveHUDLayout.h"
#include "GameHUD.generated.h"

/**
 * Example of a responsive game HUD.
 */
UCLASS()
class YOURGAME_API UGameHUD : public UResponsiveHUDLayout
{
    GENERATED_BODY()

public:
    // Initialize the HUD
    virtual void InitializeHUD(class APlayerCharacter* PlayerCharacter);

protected:
    // Health bar widget
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UHealthBarWidget* HealthBar;

    // Minimap widget
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UMinimapWidget* Minimap;

    // Inventory widget
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UInventoryWidget* Inventory;

    // Override viewport size change handler
    virtual void OnViewportSizeChanged(const FVector2D& NewViewportSize) override;

private:
    // Update HUD layout based on screen size
    void UpdateHUDLayout(const FVector2D& ViewportSize);
};
```

```cpp
// GameHUD.cpp
#include "UI/GameHUD.h"
#include "Characters/PlayerCharacter.h"

void UGameHUD::InitializeHUD(APlayerCharacter* PlayerCharacter)
{
    if (!PlayerCharacter)
    {
        return;
    }

    // Add widgets to appropriate regions
    if (HealthBar)
    {
        AddWidgetToRegion(HealthBar, EScreenRegion::TopLeft);
    }

    if (Minimap)
    {
        AddWidgetToRegion(Minimap, EScreenRegion::TopRight);
    }

    if (Inventory)
    {
        AddWidgetToRegion(Inventory, EScreenRegion::BottomRight);
    }

    // Set initial layout
    FVector2D ViewportSize;
    GetViewportSize(ViewportSize);
    UpdateHUDLayout(ViewportSize);
}

void UGameHUD::OnViewportSizeChanged(const FVector2D& NewViewportSize)
{
    Super::OnViewportSizeChanged(NewViewportSize);
    UpdateHUDLayout(NewViewportSize);
}

void UGameHUD::UpdateHUDLayout(const FVector2D& ViewportSize)
{
    // Adjust HUD element sizes based on screen size
    if (ViewportSize.X < 1280.0f)
    {
        // Mobile/small screen layout
        SetRegionPadding(EScreenRegion::TopLeft, FMargin(10.0f));
        SetRegionPadding(EScreenRegion::TopRight, FMargin(10.0f));
        SetRegionPadding(EScreenRegion::BottomRight, FMargin(10.0f));

        if (Minimap)
        {
            Minimap->SetDesiredSize(FVector2D(150.0f, 150.0f));
        }
    }
    else
    {
        // Desktop/large screen layout
        SetRegionPadding(EScreenRegion::TopLeft, FMargin(20.0f));
        SetRegionPadding(EScreenRegion::TopRight, FMargin(20.0f));
        SetRegionPadding(EScreenRegion::BottomRight, FMargin(20.0f));

        if (Minimap)
        {
            Minimap->SetDesiredSize(FVector2D(200.0f, 200.0f));
        }
    }
}
```

## Best Practices

1. **Use Relative Sizing**: Use relative sizes (percentages) instead of fixed pixel values whenever possible.

2. **Design for Multiple Resolutions**: Test your UI at various resolutions and aspect ratios during development.

3. **Handle Safe Areas**: Always respect device-specific safe areas, especially on mobile devices.

4. **Implement Breakpoints**: Define clear breakpoints for layout changes based on screen size.

5. **Cache Viewport Queries**: Cache viewport size and scale information to avoid frequent queries.

6. **Use Anchors and Alignment**: Properly set up anchors and alignment for UI elements to maintain relative positioning.

7. **Consider DPI Scaling**: Account for different DPI settings when calculating sizes and positions.

## Common Pitfalls

1. **Fixed Sizes**: Using fixed pixel sizes that don't scale with the screen.

2. **Ignoring Safe Areas**: Not accounting for device-specific safe areas.

3. **Over-Scaling**: Allowing UI elements to scale too large or too small.

4. **Layout Thrashing**: Updating layouts too frequently during screen size changes.

5. **Inconsistent Anchoring**: Mixing different anchor points that cause layout issues.

## Related Patterns

- [Widget Base Classes](Pattern1_WidgetBaseClasses.md)
- [Widget Factory System](Pattern3_WidgetFactorySystem.md)
- [Performance Optimization](Pattern7_PerformanceOptimization.md) 