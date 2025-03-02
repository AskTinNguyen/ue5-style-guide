# Performance Optimization

This document outlines patterns and techniques for optimizing UMG widget performance in C++, focusing on memory management, rendering efficiency, and input handling optimization.

## Overview

Performance optimization is crucial for maintaining smooth UI experiences, especially in complex games with many widgets. This pattern focuses on common performance bottlenecks and solutions for UMG widgets.

## Implementation

### 1. Widget Pooling System

A system for reusing widget instances to reduce memory allocation and garbage collection overhead.

```cpp
// WidgetPool.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetPool.generated.h"

/**
 * System for pooling and reusing widget instances.
 */
UCLASS()
class YOURGAME_API UWidgetPool : public UObject
{
    GENERATED_BODY()

public:
    // Get a widget from the pool
    UFUNCTION(BlueprintCallable, Category = "Widget Pool")
    class UUserWidget* GetWidget(TSubclassOf<UUserWidget> WidgetClass);

    // Return a widget to the pool
    UFUNCTION(BlueprintCallable, Category = "Widget Pool")
    void ReturnWidget(class UUserWidget* Widget);

    // Pre-populate pool with widgets
    UFUNCTION(BlueprintCallable, Category = "Widget Pool")
    void PrePopulatePool(TSubclassOf<UUserWidget> WidgetClass, int32 Count);

    // Clear the pool
    UFUNCTION(BlueprintCallable, Category = "Widget Pool")
    void ClearPool();

private:
    // Pool of inactive widgets
    TMap<TSubclassOf<UUserWidget>, TArray<TWeakObjectPtr<UUserWidget>>> InactiveWidgets;

    // Create a new widget instance
    UUserWidget* CreateWidget(TSubclassOf<UUserWidget> WidgetClass);
};
```

### 2. Visibility Optimization

A system for efficiently managing widget visibility and updating.

```cpp
// VisibilityOptimizer.h
#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "VisibilityOptimizer.generated.h"

/**
 * Component for optimizing widget visibility and updates.
 */
UCLASS()
class YOURGAME_API UVisibilityOptimizer : public UObject
{
    GENERATED_BODY()

public:
    // Set widget visibility with optimization
    UFUNCTION(BlueprintCallable, Category = "Visibility Optimization")
    void SetWidgetVisibility(class UWidget* Widget, ESlateVisibility NewVisibility);

    // Batch update widget visibility
    UFUNCTION(BlueprintCallable, Category = "Visibility Optimization")
    void BatchUpdateVisibility(const TArray<class UWidget*>& Widgets, ESlateVisibility NewVisibility);

    // Enable/disable widget updates
    UFUNCTION(BlueprintCallable, Category = "Visibility Optimization")
    void SetWidgetUpdatesEnabled(class UWidget* Widget, bool bEnabled);

private:
    // Track widget update states
    TMap<TWeakObjectPtr<UWidget>, bool> UpdateStates;

    // Check if widget needs update
    bool ShouldUpdateWidget(class UWidget* Widget) const;
};
```

### 3. Render Optimization

Techniques for optimizing widget rendering performance.

```cpp
// RenderOptimizedWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RenderOptimizedWidget.generated.h"

/**
 * Base class for render-optimized widgets.
 */
UCLASS()
class YOURGAME_API URenderOptimizedWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    URenderOptimizedWidget(const FObjectInitializer& ObjectInitializer);

    // Set render optimization level
    UFUNCTION(BlueprintCallable, Category = "Render Optimization")
    void SetOptimizationLevel(int32 Level);

    // Enable/disable render caching
    UFUNCTION(BlueprintCallable, Category = "Render Optimization")
    void SetRenderCacheEnabled(bool bEnabled);

protected:
    // Override paint methods
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    virtual void NativeOnPaint(FPaintContext& Context) const override;

    // Optimization settings
    UPROPERTY(EditAnywhere, Category = "Render Optimization")
    int32 OptimizationLevel;

    UPROPERTY(EditAnywhere, Category = "Render Optimization")
    bool bEnableRenderCache;

    UPROPERTY(EditAnywhere, Category = "Render Optimization")
    float UpdateInterval;

private:
    // Cached render data
    mutable FGeometry LastGeometry;
    mutable FDateTime LastUpdateTime;
    mutable TSharedPtr<FSlateBrush> CachedBrush;

    // Check if render cache is valid
    bool IsRenderCacheValid() const;
    
    // Update render cache
    void UpdateRenderCache() const;
};
```

### 4. Memory Management

Patterns for efficient memory management in widgets.

```cpp
// MemoryOptimizedWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MemoryOptimizedWidget.generated.h"

/**
 * Base class for memory-optimized widgets.
 */
UCLASS()
class YOURGAME_API UMemoryOptimizedWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UMemoryOptimizedWidget(const FObjectInitializer& ObjectInitializer);

    // Initialize with memory constraints
    UFUNCTION(BlueprintCallable, Category = "Memory Optimization")
    void InitializeWithMemoryConstraints(int32 MaxMemoryMB);

    // Clean up unused resources
    UFUNCTION(BlueprintCallable, Category = "Memory Optimization")
    void CleanupUnusedResources();

protected:
    // Override construction/destruction
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Memory tracking
    UPROPERTY(EditAnywhere, Category = "Memory Optimization")
    int32 MaxMemoryUsageMB;

    UPROPERTY(EditAnywhere, Category = "Memory Optimization")
    bool bAutoCleanupEnabled;

private:
    // Resource tracking
    TArray<TWeakObjectPtr<UObject>> ManagedResources;
    
    // Memory usage tracking
    void TrackMemoryUsage();
    void TrimMemoryIfNeeded();
};
```

### 5. Event Optimization

Patterns for optimizing event handling and delegates.

```cpp
// OptimizedEventHandler.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OptimizedEventHandler.generated.h"

/**
 * System for optimizing event handling in widgets.
 */
UCLASS()
class YOURGAME_API UOptimizedEventHandler : public UObject
{
    GENERATED_BODY()

public:
    // Bind event with throttling
    template<typename UserClass>
    void BindThrottledEvent(UserClass* Object, typename FEventDelegate::TUObjectMethodDelegate<UserClass>::FMethodPtr Func, float ThrottleInterval);

    // Bind event with debouncing
    template<typename UserClass>
    void BindDebouncedEvent(UserClass* Object, typename FEventDelegate::TUObjectMethodDelegate<UserClass>::FMethodPtr Func, float DebounceInterval);

    // Process pending events
    void ProcessEvents();

private:
    // Event queue
    struct FPendingEvent
    {
        TWeakObjectPtr<UObject> Object;
        FName FunctionName;
        float TimeStamp;
        float Interval;
    };

    TArray<FPendingEvent> PendingEvents;
    
    // Clean up stale events
    void CleanupStaleEvents();
};
```

## Usage Examples

### 1. Implementing Widget Pooling

```cpp
// InventoryGridWidget.cpp
void UInventoryGridWidget::UpdateGrid()
{
    // Get widget pool
    UWidgetPool* Pool = GetGameInstance()->GetSubsystem<UWidgetPool>();
    
    // Reuse or create item slots
    for (const FInventoryItem& Item : Items)
    {
        UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(
            Pool->GetWidget(UInventorySlotWidget::StaticClass())
        );
        
        SlotWidget->SetItem(Item);
        ItemContainer->AddChild(SlotWidget);
    }
}

void UInventoryGridWidget::ClearGrid()
{
    // Return widgets to pool
    UWidgetPool* Pool = GetGameInstance()->GetSubsystem<UWidgetPool>();
    
    for (UWidget* Child : ItemContainer->GetAllChildren())
    {
        if (UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(Child))
        {
            Pool->ReturnWidget(SlotWidget);
        }
    }
    
    ItemContainer->ClearChildren();
}
```

### 2. Using Render Optimization

```cpp
// GameHUDWidget.cpp
void UGameHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Set up render optimization
    SetOptimizationLevel(2);
    SetRenderCacheEnabled(true);
    
    // Only update certain elements frequently
    HealthBar->SetUpdateInterval(0.1f);
    AmmoCounter->SetUpdateInterval(0.0f); // Update every frame
    MinimapWidget->SetUpdateInterval(0.5f);
}

void UGameHUDWidget::NativeOnPaint(FPaintContext& Context) const
{
    if (!IsRenderCacheValid())
    {
        UpdateRenderCache();
    }
    
    // Use cached render data
    if (CachedBrush.IsValid())
    {
        FSlateDrawElement::MakeBox(
            Context.OutDrawElements,
            Context.MaxLayer,
            Context.AllottedGeometry.ToPaintGeometry(),
            CachedBrush.Get()
        );
    }
}
```

### 3. Memory Optimization Example

```cpp
// CharacterStatusWidget.cpp
void UCharacterStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Set memory constraints
    InitializeWithMemoryConstraints(50); // 50MB limit
    
    // Enable auto cleanup
    bAutoCleanupEnabled = true;
    
    // Load and manage resources
    LoadCharacterResources();
}

void UCharacterStatusWidget::LoadCharacterResources()
{
    // Track resource loading
    for (const FCharacterResourceData& Resource : ResourcestoLoad)
    {
        UObject* LoadedResource = LoadObject<UObject>(nullptr, *Resource.Path);
        if (LoadedResource)
        {
            ManagedResources.Add(LoadedResource);
        }
    }
    
    // Check memory usage
    TrackMemoryUsage();
}
```

## Best Practices

1. **Use Widget Pooling**: Reuse widget instances instead of creating/destroying them frequently.

2. **Optimize Visibility**: Only update visible widgets and use efficient visibility changes.

3. **Implement Render Caching**: Cache rendered content for static or infrequently changing widgets.

4. **Manage Memory**: Track and limit memory usage, clean up unused resources.

5. **Optimize Event Handling**: Use throttling and debouncing for frequent events.

6. **Batch Updates**: Combine multiple widget updates into single operations.

7. **Profile Performance**: Regularly profile widget performance and identify bottlenecks.

## Common Pitfalls

1. **Excessive Updates**: Updating widgets more frequently than necessary.

2. **Memory Leaks**: Not properly cleaning up widget resources.

3. **Render Bottlenecks**: Complex render operations in frequently updated widgets.

4. **Event Spam**: Not throttling high-frequency events.

5. **Inefficient Pooling**: Poor pool size management leading to frequent allocations.

## Related Patterns

- [Widget Base Classes](Pattern1_WidgetBaseClasses.md)
- [Widget Factory System](Pattern3_WidgetFactorySystem.md)
- [Widget Animation in C++](Pattern5_WidgetAnimationCPP.md) 