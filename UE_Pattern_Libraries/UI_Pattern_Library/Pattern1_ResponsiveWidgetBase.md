# Responsive Widget Base Pattern

## Problem
Creating UMG widgets that automatically adjust to different screen sizes, aspect ratios, and platforms while maintaining visual consistency.

## Solution Pattern
Implement a base widget class that handles responsive layout calculations and provides utilities for widgets to adapt their layouts dynamically.

## Code Example

```cpp
// UResponsiveWidgetBase.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResponsiveWidgetBase.generated.h"

// Device profile types for different screen configurations
UENUM(BlueprintType)
enum class EScreenSizeType : uint8
{
    Mobile_Small UMETA(DisplayName = "Mobile Small"),
    Mobile_Medium UMETA(DisplayName = "Mobile Medium"),
    Mobile_Large UMETA(DisplayName = "Mobile Large"),
    Tablet UMETA(DisplayName = "Tablet"),
    Desktop UMETA(DisplayName = "Desktop"),
    TV UMETA(DisplayName = "TV/Console")
};

// Screen orientation types
UENUM(BlueprintType)
enum class EScreenOrientation : uint8
{
    Landscape UMETA(DisplayName = "Landscape"),
    Portrait UMETA(DisplayName = "Portrait")
};

/**
 * Base widget class for responsive UI layouts
 */
UCLASS()
class YOURGAME_API UResponsiveWidgetBase : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UResponsiveWidgetBase(const FObjectInitializer& ObjectInitializer);
    
    // Begin UUserWidget interface
    virtual void NativeConstruct() override;
    virtual void NativePreConstruct() override;
    virtual void NativeOnScreenSizeChanged(const FIntPoint& OldSize, const FIntPoint& NewSize) override;
    // End UUserWidget interface
    
    // Call this to update the layout based on the current screen size
    UFUNCTION(BlueprintCallable, Category = "UI|Responsive")
    void UpdateResponsiveLayout();
    
    // Get current screen size type
    UFUNCTION(BlueprintPure, Category = "UI|Responsive")
    EScreenSizeType GetCurrentScreenSizeType() const;
    
    // Get current screen orientation
    UFUNCTION(BlueprintPure, Category = "UI|Responsive")
    EScreenOrientation GetCurrentScreenOrientation() const;
    
    // Calculate scale factor for current screen size compared to base size
    UFUNCTION(BlueprintPure, Category = "UI|Responsive")
    float GetScreenScaleFactor() const;
    
    // Convert design-time position to runtime position based on current screen size
    UFUNCTION(BlueprintPure, Category = "UI|Responsive")
    FVector2D GetResponsivePosition(const FVector2D& OriginalPosition) const;
    
    // Convert design-time size to runtime size based on current screen size
    UFUNCTION(BlueprintPure, Category = "UI|Responsive")
    FVector2D GetResponsiveSize(const FVector2D& OriginalSize) const;
    
protected:
    // Base resolution for design (usually 1920x1080)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Responsive")
    FVector2D BaseScreenSize = FVector2D(1920.0f, 1080.0f);
    
    // Current screen properties
    UPROPERTY(BlueprintReadOnly, Category = "UI|Responsive")
    EScreenSizeType CurrentScreenSizeType;
    
    UPROPERTY(BlueprintReadOnly, Category = "UI|Responsive")
    EScreenOrientation CurrentScreenOrientation;
    
    UPROPERTY(BlueprintReadOnly, Category = "UI|Responsive")
    FVector2D CurrentScreenSize;
    
    UPROPERTY(BlueprintReadOnly, Category = "UI|Responsive")
    float CurrentAspectRatio;
    
    // Screen size thresholds (in dp/pixel density independent)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Responsive|Thresholds")
    float MobileSmallThreshold = 480.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Responsive|Thresholds")
    float MobileMediumThreshold = 720.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Responsive|Thresholds")
    float MobileLargeThreshold = 1080.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Responsive|Thresholds")
    float TabletThreshold = 1280.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Responsive|Thresholds")
    float DesktopThreshold = 1920.0f;
    
    // Called when screen size type changes
    UFUNCTION(BlueprintImplementableEvent, Category = "UI|Responsive")
    void OnScreenSizeTypeChanged(EScreenSizeType NewScreenSizeType);
    
    // Called when screen orientation changes
    UFUNCTION(BlueprintImplementableEvent, Category = "UI|Responsive")
    void OnScreenOrientationChanged(EScreenOrientation NewScreenOrientation);
    
    // Internal methods to determine screen properties
    virtual void DetermineScreenSizeType();
    virtual void DetermineScreenOrientation();
    
    // Blueprint implementable method to update widget layout
    UFUNCTION(BlueprintImplementableEvent, Category = "UI|Responsive")
    void ApplyResponsiveLayout();
    
    // C++ implementation of layout adjustments
    virtual void NativeApplyResponsiveLayout();
};
```

### Implementation Example

```cpp
// UResponsiveWidgetBase.cpp
#include "ResponsiveWidgetBase.h"
#include "Engine/GameViewportClient.h"
#include "Components/CanvasPanelSlot.h"
#include "Framework/Application/SlateApplication.h"

UResponsiveWidgetBase::UResponsiveWidgetBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Default values
    CurrentScreenSizeType = EScreenSizeType::Desktop;
    CurrentScreenOrientation = EScreenOrientation::Landscape;
    CurrentScreenSize = BaseScreenSize;
    CurrentAspectRatio = BaseScreenSize.X / BaseScreenSize.Y;
}

void UResponsiveWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Get current screen size on construction
    if (GEngine && GEngine->GameViewport)
    {
        FVector2D ViewportSize;
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        CurrentScreenSize = ViewportSize;
        CurrentAspectRatio = ViewportSize.X / ViewportSize.Y;
    }
    
    // Initialize based on current screen size
    DetermineScreenSizeType();
    DetermineScreenOrientation();
    UpdateResponsiveLayout();
}

void UResponsiveWidgetBase::NativePreConstruct()
{
    Super::NativePreConstruct();
    
    // For design-time preview
    if (IsDesignTime())
    {
        DetermineScreenSizeType();
        DetermineScreenOrientation();
        UpdateResponsiveLayout();
    }
}

void UResponsiveWidgetBase::NativeOnScreenSizeChanged(const FIntPoint& OldSize, const FIntPoint& NewSize)
{
    Super::NativeOnScreenSizeChanged(OldSize, NewSize);
    
    // Update properties for the new screen size
    CurrentScreenSize = FVector2D(NewSize.X, NewSize.Y);
    CurrentAspectRatio = CurrentScreenSize.X / CurrentScreenSize.Y;
    
    // Determine new screen configuration
    EScreenSizeType OldScreenSizeType = CurrentScreenSizeType;
    EScreenOrientation OldScreenOrientation = CurrentScreenOrientation;
    
    DetermineScreenSizeType();
    DetermineScreenOrientation();
    
    // Notify about changes if any
    if (OldScreenSizeType != CurrentScreenSizeType)
    {
        OnScreenSizeTypeChanged(CurrentScreenSizeType);
    }
    
    if (OldScreenOrientation != CurrentScreenOrientation)
    {
        OnScreenOrientationChanged(CurrentScreenOrientation);
    }
    
    // Apply layout changes
    UpdateResponsiveLayout();
}

void UResponsiveWidgetBase::UpdateResponsiveLayout()
{
    // Call C++ implementation
    NativeApplyResponsiveLayout();
    
    // Call Blueprint implementation
    ApplyResponsiveLayout();
}

EScreenSizeType UResponsiveWidgetBase::GetCurrentScreenSizeType() const
{
    return CurrentScreenSizeType;
}

EScreenOrientation UResponsiveWidgetBase::GetCurrentScreenOrientation() const
{
    return CurrentScreenOrientation;
}

float UResponsiveWidgetBase::GetScreenScaleFactor() const
{
    // Calculate scale factor based on width
    return CurrentScreenSize.X / BaseScreenSize.X;
}

FVector2D UResponsiveWidgetBase::GetResponsivePosition(const FVector2D& OriginalPosition) const
{
    float ScaleFactor = GetScreenScaleFactor();
    return OriginalPosition * ScaleFactor;
}

FVector2D UResponsiveWidgetBase::GetResponsiveSize(const FVector2D& OriginalSize) const
{
    float ScaleFactor = GetScreenScaleFactor();
    return OriginalSize * ScaleFactor;
}

void UResponsiveWidgetBase::DetermineScreenSizeType()
{
    // Get the smaller dimension to determine size
    float SmallerDimension = FMath::Min(CurrentScreenSize.X, CurrentScreenSize.Y);
    
    // Determine screen size type based on thresholds
    if (SmallerDimension <= MobileSmallThreshold)
    {
        CurrentScreenSizeType = EScreenSizeType::Mobile_Small;
    }
    else if (SmallerDimension <= MobileMediumThreshold)
    {
        CurrentScreenSizeType = EScreenSizeType::Mobile_Medium;
    }
    else if (SmallerDimension <= MobileLargeThreshold)
    {
        CurrentScreenSizeType = EScreenSizeType::Mobile_Large;
    }
    else if (SmallerDimension <= TabletThreshold)
    {
        CurrentScreenSizeType = EScreenSizeType::Tablet;
    }
    else if (SmallerDimension <= DesktopThreshold)
    {
        CurrentScreenSizeType = EScreenSizeType::Desktop;
    }
    else
    {
        CurrentScreenSizeType = EScreenSizeType::TV;
    }
}

void UResponsiveWidgetBase::DetermineScreenOrientation()
{
    if (CurrentScreenSize.X >= CurrentScreenSize.Y)
    {
        CurrentScreenOrientation = EScreenOrientation::Landscape;
    }
    else
    {
        CurrentScreenOrientation = EScreenOrientation::Portrait;
    }
}

void UResponsiveWidgetBase::NativeApplyResponsiveLayout()
{
    // Default implementation - handle common adjustments here
    // For example, adjust the size and position of canvas panel slots
    
    /* 
    // Example of how to adjust canvas panel slots
    TArray<UWidget*> AllChildren;
    WidgetTree->GetAllWidgets(AllChildren);
    
    for (UWidget* Child : AllChildren)
    {
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Child->Slot);
        if (CanvasSlot)
        {
            // Apply responsive adjustments based on current screen size
            FVector2D OriginalPosition = CanvasSlot->GetPosition();
            FVector2D OriginalSize = CanvasSlot->GetSize();
            
            CanvasSlot->SetPosition(GetResponsivePosition(OriginalPosition));
            CanvasSlot->SetSize(GetResponsiveSize(OriginalSize));
        }
    }
    */
    
    // Child classes should override this to implement specific layout adjustments
}
```

### Example Usage

To use this base class, create a widget that inherits from `UResponsiveWidgetBase` and implement the `ApplyResponsiveLayout` method. This method will be called whenever the screen size changes, allowing you to adjust your layout accordingly.

```cpp
// UPlayerHUDWidget.h
UCLASS()
class YOURGAME_API UPlayerHUDWidget : public UResponsiveWidgetBase
{
    GENERATED_BODY()
    
protected:
    // UI components
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* MobileControls;
    
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* MainHUDPanel;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthText;
    
    // Override NativeApplyResponsiveLayout to handle C++ layout adjustments
    virtual void NativeApplyResponsiveLayout() override;
};

// UPlayerHUDWidget.cpp
void UPlayerHUDWidget::NativeApplyResponsiveLayout()
{
    Super::NativeApplyResponsiveLayout();
    
    // Show/hide mobile controls based on platform
    if (MobileControls)
    {
        bool bIsMobilePlatform = 
            GetCurrentScreenSizeType() == EScreenSizeType::Mobile_Small || 
            GetCurrentScreenSizeType() == EScreenSizeType::Mobile_Medium || 
            GetCurrentScreenSizeType() == EScreenSizeType::Mobile_Large;
            
        MobileControls->SetVisibility(bIsMobilePlatform ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
    
    // Adjust text size based on screen size
    if (HealthText)
    {
        float BaseTextSize = 24.0f;
        float ScaleFactor = 1.0f;
        
        switch (GetCurrentScreenSizeType())
        {
            case EScreenSizeType::Mobile_Small:
                ScaleFactor = 0.7f;
                break;
            case EScreenSizeType::Mobile_Medium:
                ScaleFactor = 0.8f;
                break;
            case EScreenSizeType::Mobile_Large:
                ScaleFactor = 0.9f;
                break;
            case EScreenSizeType::Tablet:
                ScaleFactor = 1.1f;
                break;
            case EScreenSizeType::Desktop:
                ScaleFactor = 1.2f;
                break;
            case EScreenSizeType::TV:
                ScaleFactor = 1.5f;
                break;
        }
        
        HealthText->SetFontSize(BaseTextSize * ScaleFactor);
    }
    
    // Adjust the HUD layout based on orientation
    if (MainHUDPanel && GetCurrentScreenOrientation() == EScreenOrientation::Portrait)
    {
        // Reposition HUD elements for portrait mode
    }
}
```

## AI Prompt Template

```
I need to implement a responsive widget base class in UE5 that will handle different screen sizes and platforms. 
The widget should:

1. Detect the current screen size and orientation (landscape/portrait)
2. Categorize screen sizes (mobile, tablet, desktop, TV)
3. Provide utility functions for adjusting UI elements based on screen size
4. Handle screen size change events automatically
5. Include Blueprint-callable functions for derived widget classes

Please provide a C++ implementation for a UResponsiveWidgetBase class that:
- Inherits from UUserWidget
- Overrides appropriate lifecycle methods
- Implements screen size and orientation detection
- Provides helper methods for position and size calculations
- Includes both C++ and Blueprint hooks for layout adjustments

Additionally, please include a simple example of how to use this base class for a game HUD that needs to adapt its layout across different platforms.
```

## Performance Considerations
- Avoid recalculating layouts every frame; update only when screen size changes
- Cache common calculations like scale factors
- Use visibility changes instead of removing/recreating widgets
- Minimize complex layout recalculations during gameplay
- Consider pre-computing layouts for known screen configurations

## Integration Tips
- Create a UI manager that handles the creation and caching of responsive widgets
- Define clear breakpoints that match common device resolutions
- Use anchors and alignment within the UMG designer where possible
- Create separate layouts for drastically different form factors (like VR)
- Test on multiple device types throughout development

## Advanced Implementation

For a more sophisticated responsive system, consider extending this pattern with:

1. **Responsive Asset Loading**: Load different resolution textures based on the current screen size
2. **Layout Caching**: Pre-compute layouts for known screen sizes
3. **Animation Scaling**: Adjust animation timing based on screen size
4. **Input Adaptation**: Modify input hitboxes for touch vs. mouse input
5. **Platform-Specific Overrides**: Handle platform-specific UI requirements

Example of responsive asset loading:

```cpp
UTexture2D* UResponsiveWidgetBase::GetResponsiveTexture(const FString& BasePath)
{
    FString SuffixPath;
    
    // Select appropriate texture resolution based on screen size
    switch (GetCurrentScreenSizeType())
    {
        case EScreenSizeType::Mobile_Small:
        case EScreenSizeType::Mobile_Medium:
            SuffixPath = "_SM"; // Small
            break;
        case EScreenSizeType::Mobile_Large:
        case EScreenSizeType::Tablet:
            SuffixPath = "_MD"; // Medium
            break;
        case EScreenSizeType::Desktop:
            SuffixPath = "_LG"; // Large
            break;
        case EScreenSizeType::TV:
            SuffixPath = "_XL"; // Extra Large
            break;
    }
    
    // Create the full path with appropriate suffix
    FString FullPath = BasePath + SuffixPath;
    
    // Try to load the responsive version first
    UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FullPath));
    
    // Fall back to base texture if responsive version not found
    if (!Texture)
    {
        Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *BasePath));
    }
    
    return Texture;
}