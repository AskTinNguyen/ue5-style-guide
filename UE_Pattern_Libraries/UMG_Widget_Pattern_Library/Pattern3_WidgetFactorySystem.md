# Widget Factory System

This document outlines a centralized widget creation and management system for UMG widgets in C++.

## Overview

A Widget Factory System provides a centralized approach to creating, caching, and managing UMG widgets. This pattern helps reduce code duplication, improves performance through widget reuse, and provides a consistent interface for widget creation across your project.

## Implementation

### 1. Widget Factory Manager

The core of the system is a manager class that handles widget creation, caching, and retrieval.

```cpp
// WidgetFactoryManager.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "WidgetFactoryManager.generated.h"

/**
 * Manager class for creating and caching UMG widgets.
 */
UCLASS(BlueprintType, Blueprintable)
class YOURGAME_API UWidgetFactoryManager : public UObject
{
    GENERATED_BODY()

public:
    UWidgetFactoryManager();

    // Initialize the manager
    void Initialize(APlayerController* InPlayerController);

    // Create or retrieve a widget of the specified class
    UFUNCTION(BlueprintCallable, Category = "Widget Factory")
    UUserWidget* GetWidget(TSubclassOf<UUserWidget> WidgetClass, bool bCreateIfNotFound = true);

    // Create a new widget instance
    UFUNCTION(BlueprintCallable, Category = "Widget Factory")
    UUserWidget* CreateWidget(TSubclassOf<UUserWidget> WidgetClass);

    // Remove a widget from the cache
    UFUNCTION(BlueprintCallable, Category = "Widget Factory")
    void RemoveWidget(UUserWidget* Widget);

    // Clear all widgets from the cache
    UFUNCTION(BlueprintCallable, Category = "Widget Factory")
    void ClearWidgetCache();

    // Get the singleton instance
    UFUNCTION(BlueprintCallable, Category = "Widget Factory", meta = (WorldContext = "WorldContextObject"))
    static UWidgetFactoryManager* GetInstance(UObject* WorldContextObject);

private:
    // Map of widget classes to widget instances
    UPROPERTY()
    TMap<TSubclassOf<UUserWidget>, UUserWidget*> WidgetCache;

    // Reference to the player controller
    UPROPERTY()
    TWeakObjectPtr<APlayerController> PlayerController;

    // Singleton instance
    static UWidgetFactoryManager* Instance;
};
```

```cpp
// WidgetFactoryManager.cpp
#include "UI/WidgetFactoryManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UWidgetFactoryManager* UWidgetFactoryManager::Instance = nullptr;

UWidgetFactoryManager::UWidgetFactoryManager()
{
    // Set this instance as the singleton if not already set
    if (!Instance)
    {
        Instance = this;
    }
}

void UWidgetFactoryManager::Initialize(APlayerController* InPlayerController)
{
    PlayerController = InPlayerController;
}

UUserWidget* UWidgetFactoryManager::GetWidget(TSubclassOf<UUserWidget> WidgetClass, bool bCreateIfNotFound)
{
    if (!WidgetClass)
    {
        return nullptr;
    }

    // Check if the widget is already in the cache
    UUserWidget** FoundWidget = WidgetCache.Find(WidgetClass);
    if (FoundWidget && *FoundWidget && !(*FoundWidget)->IsUnreachable())
    {
        return *FoundWidget;
    }

    // Create a new widget if requested and not found
    if (bCreateIfNotFound)
    {
        UUserWidget* NewWidget = CreateWidget(WidgetClass);
        if (NewWidget)
        {
            WidgetCache.Add(WidgetClass, NewWidget);
            return NewWidget;
        }
    }

    return nullptr;
}

UUserWidget* UWidgetFactoryManager::CreateWidget(TSubclassOf<UUserWidget> WidgetClass)
{
    if (!WidgetClass || !PlayerController.IsValid())
    {
        return nullptr;
    }

    return ::CreateWidget<UUserWidget>(PlayerController.Get(), WidgetClass);
}

void UWidgetFactoryManager::RemoveWidget(UUserWidget* Widget)
{
    if (!Widget)
    {
        return;
    }

    // Find and remove the widget from the cache
    for (auto It = WidgetCache.CreateIterator(); It; ++It)
    {
        if (It.Value() == Widget)
        {
            It.RemoveCurrent();
            break;
        }
    }
}

void UWidgetFactoryManager::ClearWidgetCache()
{
    WidgetCache.Empty();
}

UWidgetFactoryManager* UWidgetFactoryManager::GetInstance(UObject* WorldContextObject)
{
    if (Instance)
    {
        return Instance;
    }

    // Create a new instance if one doesn't exist
    UWidgetFactoryManager* NewInstance = NewObject<UWidgetFactoryManager>(WorldContextObject);
    
    // Initialize with the player controller
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
    if (PlayerController)
    {
        NewInstance->Initialize(PlayerController);
    }

    return NewInstance;
}
```

### 2. Widget Factory Subsystem

For better integration with Unreal Engine's subsystem architecture, you can implement the factory as a game instance subsystem.

```cpp
// WidgetFactorySubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "WidgetFactorySubsystem.generated.h"

/**
 * Subsystem for creating and caching UMG widgets.
 */
UCLASS()
class YOURGAME_API UWidgetFactorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Initialize the subsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Deinitialize the subsystem
    virtual void Deinitialize() override;

    // Create or retrieve a widget of the specified class
    UFUNCTION(BlueprintCallable, Category = "Widget Factory")
    UUserWidget* GetWidget(TSubclassOf<UUserWidget> WidgetClass, bool bCreateIfNotFound = true);

    // Create a new widget instance
    UFUNCTION(BlueprintCallable, Category = "Widget Factory")
    UUserWidget* CreateWidget(TSubclassOf<UUserWidget> WidgetClass);

    // Remove a widget from the cache
    UFUNCTION(BlueprintCallable, Category = "Widget Factory")
    void RemoveWidget(UUserWidget* Widget);

    // Clear all widgets from the cache
    UFUNCTION(BlueprintCallable, Category = "Widget Factory")
    void ClearWidgetCache();

private:
    // Map of widget classes to widget instances
    UPROPERTY()
    TMap<TSubclassOf<UUserWidget>, UUserWidget*> WidgetCache;
};
```

```cpp
// WidgetFactorySubsystem.cpp
#include "UI/WidgetFactorySubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UWidgetFactorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Any initialization code here
}

void UWidgetFactorySubsystem::Deinitialize()
{
    // Clean up widget cache
    ClearWidgetCache();
    
    Super::Deinitialize();
}

UUserWidget* UWidgetFactorySubsystem::GetWidget(TSubclassOf<UUserWidget> WidgetClass, bool bCreateIfNotFound)
{
    if (!WidgetClass)
    {
        return nullptr;
    }

    // Check if the widget is already in the cache
    UUserWidget** FoundWidget = WidgetCache.Find(WidgetClass);
    if (FoundWidget && *FoundWidget && !(*FoundWidget)->IsUnreachable())
    {
        return *FoundWidget;
    }

    // Create a new widget if requested and not found
    if (bCreateIfNotFound)
    {
        UUserWidget* NewWidget = CreateWidget(WidgetClass);
        if (NewWidget)
        {
            WidgetCache.Add(WidgetClass, NewWidget);
            return NewWidget;
        }
    }

    return nullptr;
}

UUserWidget* UWidgetFactorySubsystem::CreateWidget(TSubclassOf<UUserWidget> WidgetClass)
{
    if (!WidgetClass)
    {
        return nullptr;
    }

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
    {
        return nullptr;
    }

    return ::CreateWidget<UUserWidget>(PlayerController, WidgetClass);
}

void UWidgetFactorySubsystem::RemoveWidget(UUserWidget* Widget)
{
    if (!Widget)
    {
        return;
    }

    // Find and remove the widget from the cache
    for (auto It = WidgetCache.CreateIterator(); It; ++It)
    {
        if (It.Value() == Widget)
        {
            It.RemoveCurrent();
            break;
        }
    }
}

void UWidgetFactorySubsystem::ClearWidgetCache()
{
    WidgetCache.Empty();
}
```

### 3. Specialized Widget Factories

For more complex widget systems, you can create specialized factories for different widget types.

```cpp
// HUDWidgetFactory.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/HUDWidget.h"
#include "HUDWidgetFactory.generated.h"

/**
 * Factory for creating and managing HUD widgets.
 */
UCLASS(BlueprintType, Blueprintable)
class YOURGAME_API UHUDWidgetFactory : public UObject
{
    GENERATED_BODY()

public:
    // Create a health bar widget
    UFUNCTION(BlueprintCallable, Category = "HUD Factory", meta = (WorldContext = "WorldContextObject"))
    static UPlayerHealthBar* CreateHealthBar(UObject* WorldContextObject, class APlayerCharacter* PlayerCharacter);

    // Create a stamina bar widget
    UFUNCTION(BlueprintCallable, Category = "HUD Factory", meta = (WorldContext = "WorldContextObject"))
    static UStaminaBar* CreateStaminaBar(UObject* WorldContextObject, class APlayerCharacter* PlayerCharacter);

    // Create a complete player HUD
    UFUNCTION(BlueprintCallable, Category = "HUD Factory", meta = (WorldContext = "WorldContextObject"))
    static UPlayerHUD* CreatePlayerHUD(UObject* WorldContextObject, class APlayerCharacter* PlayerCharacter);

private:
    // Helper to get the player controller
    static APlayerController* GetPlayerController(UObject* WorldContextObject);
};
```

```cpp
// HUDWidgetFactory.cpp
#include "UI/HUDWidgetFactory.h"
#include "UI/PlayerHealthBar.h"
#include "UI/StaminaBar.h"
#include "UI/PlayerHUD.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

UPlayerHealthBar* UHUDWidgetFactory::CreateHealthBar(UObject* WorldContextObject, APlayerCharacter* PlayerCharacter)
{
    if (!WorldContextObject || !PlayerCharacter)
    {
        return nullptr;
    }

    APlayerController* PlayerController = GetPlayerController(WorldContextObject);
    if (!PlayerController)
    {
        return nullptr;
    }

    // Get the health bar widget class from the game's data
    TSubclassOf<UPlayerHealthBar> HealthBarClass = LoadClass<UPlayerHealthBar>(nullptr, TEXT("/Game/UI/Widgets/W_PlayerHealthBar"));
    if (!HealthBarClass)
    {
        return nullptr;
    }

    // Create the health bar widget
    UPlayerHealthBar* HealthBar = CreateWidget<UPlayerHealthBar>(PlayerController, HealthBarClass);
    if (HealthBar)
    {
        // Initialize the health bar with player data
        HealthBar->UpdateHealth(PlayerCharacter->GetCurrentHealth(), PlayerCharacter->GetMaxHealth());
    }

    return HealthBar;
}

UStaminaBar* UHUDWidgetFactory::CreateStaminaBar(UObject* WorldContextObject, APlayerCharacter* PlayerCharacter)
{
    if (!WorldContextObject || !PlayerCharacter)
    {
        return nullptr;
    }

    APlayerController* PlayerController = GetPlayerController(WorldContextObject);
    if (!PlayerController)
    {
        return nullptr;
    }

    // Get the stamina bar widget class from the game's data
    TSubclassOf<UStaminaBar> StaminaBarClass = LoadClass<UStaminaBar>(nullptr, TEXT("/Game/UI/Widgets/W_StaminaBar"));
    if (!StaminaBarClass)
    {
        return nullptr;
    }

    // Create the stamina bar widget
    UStaminaBar* StaminaBar = CreateWidget<UStaminaBar>(PlayerController, StaminaBarClass);
    if (StaminaBar)
    {
        // Initialize the stamina bar with player data
        StaminaBar->UpdateStamina(PlayerCharacter->GetCurrentStamina(), PlayerCharacter->GetMaxStamina());
    }

    return StaminaBar;
}

UPlayerHUD* UHUDWidgetFactory::CreatePlayerHUD(UObject* WorldContextObject, APlayerCharacter* PlayerCharacter)
{
    if (!WorldContextObject || !PlayerCharacter)
    {
        return nullptr;
    }

    APlayerController* PlayerController = GetPlayerController(WorldContextObject);
    if (!PlayerController)
    {
        return nullptr;
    }

    // Get the player HUD widget class from the game's data
    TSubclassOf<UPlayerHUD> PlayerHUDClass = LoadClass<UPlayerHUD>(nullptr, TEXT("/Game/UI/Widgets/W_PlayerHUD"));
    if (!PlayerHUDClass)
    {
        return nullptr;
    }

    // Create the player HUD widget
    UPlayerHUD* PlayerHUD = CreateWidget<UPlayerHUD>(PlayerController, PlayerHUDClass);
    if (PlayerHUD)
    {
        // Initialize the player HUD with player data
        PlayerHUD->UpdateHUD(PlayerCharacter);
    }

    return PlayerHUD;
}

APlayerController* UHUDWidgetFactory::GetPlayerController(UObject* WorldContextObject)
{
    return UGameplayStatics::GetPlayerController(WorldContextObject, 0);
}
```

### 4. Widget Object Pool

For frequently created and destroyed widgets, an object pool can improve performance.

```cpp
// WidgetObjectPool.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "WidgetObjectPool.generated.h"

/**
 * Object pool for UMG widgets to improve performance.
 */
UCLASS(BlueprintType, Blueprintable)
class YOURGAME_API UWidgetObjectPool : public UObject
{
    GENERATED_BODY()

public:
    UWidgetObjectPool();

    // Initialize the pool with a specific widget class and size
    UFUNCTION(BlueprintCallable, Category = "Widget Pool", meta = (WorldContext = "WorldContextObject"))
    void Initialize(UObject* WorldContextObject, TSubclassOf<UUserWidget> InWidgetClass, int32 InitialPoolSize = 10);

    // Get a widget from the pool
    UFUNCTION(BlueprintCallable, Category = "Widget Pool")
    UUserWidget* GetWidget();

    // Return a widget to the pool
    UFUNCTION(BlueprintCallable, Category = "Widget Pool")
    void ReturnWidget(UUserWidget* Widget);

    // Clear the pool
    UFUNCTION(BlueprintCallable, Category = "Widget Pool")
    void ClearPool();

    // Get the singleton instance
    UFUNCTION(BlueprintCallable, Category = "Widget Pool", meta = (WorldContext = "WorldContextObject"))
    static UWidgetObjectPool* GetInstance(UObject* WorldContextObject);

private:
    // The class of widgets in this pool
    UPROPERTY()
    TSubclassOf<UUserWidget> WidgetClass;

    // The pool of available widgets
    UPROPERTY()
    TArray<UUserWidget*> AvailableWidgets;

    // The pool of active widgets
    UPROPERTY()
    TArray<UUserWidget*> ActiveWidgets;

    // Reference to the player controller
    UPROPERTY()
    TWeakObjectPtr<APlayerController> PlayerController;

    // Singleton instance
    static UWidgetObjectPool* Instance;

    // Create a new widget for the pool
    UUserWidget* CreateNewWidget();
};
```

```cpp
// WidgetObjectPool.cpp
#include "UI/WidgetObjectPool.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UWidgetObjectPool* UWidgetObjectPool::Instance = nullptr;

UWidgetObjectPool::UWidgetObjectPool()
{
    // Set this instance as the singleton if not already set
    if (!Instance)
    {
        Instance = this;
    }
}

void UWidgetObjectPool::Initialize(UObject* WorldContextObject, TSubclassOf<UUserWidget> InWidgetClass, int32 InitialPoolSize)
{
    if (!InWidgetClass)
    {
        return;
    }

    WidgetClass = InWidgetClass;
    PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);

    // Pre-create widgets for the pool
    for (int32 i = 0; i < InitialPoolSize; ++i)
    {
        UUserWidget* NewWidget = CreateNewWidget();
        if (NewWidget)
        {
            NewWidget->SetVisibility(ESlateVisibility::Hidden);
            AvailableWidgets.Add(NewWidget);
        }
    }
}

UUserWidget* UWidgetObjectPool::GetWidget()
{
    // Get a widget from the available pool or create a new one
    UUserWidget* Widget = nullptr;
    
    if (AvailableWidgets.Num() > 0)
    {
        Widget = AvailableWidgets.Pop();
    }
    else
    {
        Widget = CreateNewWidget();
    }

    if (Widget)
    {
        Widget->SetVisibility(ESlateVisibility::Visible);
        ActiveWidgets.Add(Widget);
    }

    return Widget;
}

void UWidgetObjectPool::ReturnWidget(UUserWidget* Widget)
{
    if (!Widget)
    {
        return;
    }

    // Remove from active widgets
    ActiveWidgets.Remove(Widget);

    // Reset the widget and add to available pool
    Widget->SetVisibility(ESlateVisibility::Hidden);
    AvailableWidgets.Add(Widget);
}

void UWidgetObjectPool::ClearPool()
{
    AvailableWidgets.Empty();
    ActiveWidgets.Empty();
}

UWidgetObjectPool* UWidgetObjectPool::GetInstance(UObject* WorldContextObject)
{
    if (Instance)
    {
        return Instance;
    }

    // Create a new instance if one doesn't exist
    UWidgetObjectPool* NewInstance = NewObject<UWidgetObjectPool>(WorldContextObject);
    return NewInstance;
}

UUserWidget* UWidgetObjectPool::CreateNewWidget()
{
    if (!WidgetClass || !PlayerController.IsValid())
    {
        return nullptr;
    }

    return CreateWidget<UUserWidget>(PlayerController.Get(), WidgetClass);
}
```

## Usage Examples

### Using the Widget Factory Manager

```cpp
// In your game code
UWidgetFactoryManager* WidgetFactory = UWidgetFactoryManager::GetInstance(this);
if (WidgetFactory)
{
    // Get or create a widget
    UPlayerHUD* PlayerHUD = Cast<UPlayerHUD>(WidgetFactory->GetWidget(PlayerHUDClass));
    if (PlayerHUD)
    {
        PlayerHUD->UpdateHUD(PlayerCharacter);
        PlayerHUD->AddToViewport();
    }
}
```

### Using the Widget Factory Subsystem

```cpp
// In your game code
UWidgetFactorySubsystem* WidgetFactory = GetGameInstance()->GetSubsystem<UWidgetFactorySubsystem>();
if (WidgetFactory)
{
    // Get or create a widget
    UInventoryWidget* InventoryWidget = Cast<UInventoryWidget>(WidgetFactory->GetWidget(InventoryWidgetClass));
    if (InventoryWidget)
    {
        InventoryWidget->UpdateInventory(PlayerInventory);
        InventoryWidget->AddToViewport();
    }
}
```

### Using a Specialized Widget Factory

```cpp
// In your game code
UPlayerHealthBar* HealthBar = UHUDWidgetFactory::CreateHealthBar(this, PlayerCharacter);
if (HealthBar)
{
    HealthBar->AddToViewport();
}
```

### Using the Widget Object Pool

```cpp
// Initialize the pool
UWidgetObjectPool* DamageTextPool = UWidgetObjectPool::GetInstance(this);
DamageTextPool->Initialize(this, DamageTextWidgetClass, 20);

// Later, when you need to display damage text
UDamageTextWidget* DamageText = Cast<UDamageTextWidget>(DamageTextPool->GetWidget());
if (DamageText)
{
    DamageText->SetDamageValue(DamageAmount);
    DamageText->SetWorldLocation(DamageLocation);
    DamageText->AddToViewport();
    
    // Set up a timer to return the widget to the pool after animation completes
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        FTimerDelegate::CreateLambda([DamageText, DamageTextPool]()
        {
            DamageTextPool->ReturnWidget(DamageText);
        }),
        2.0f,
        false
    );
}
```

## Best Practices

1. **Use Subsystems for Global Access**: Implement your widget factory as a game instance subsystem for easy access from anywhere in your code.

2. **Cache Frequently Used Widgets**: Cache widgets that are frequently shown and hidden rather than creating and destroying them.

3. **Use Object Pools for Transient Widgets**: Use object pools for widgets that are frequently created and destroyed, such as damage numbers or item pickups.

4. **Separate Widget Creation from Logic**: Keep widget creation separate from game logic to improve maintainability.

5. **Implement Lazy Loading**: Only create widgets when they are needed to improve startup performance.

6. **Clean Up Properly**: Ensure that widgets are properly removed from the viewport and cache when no longer needed.

7. **Use Type Safety**: Use template functions or casting to ensure type safety when retrieving widgets from the factory.

## Common Pitfalls

1. **Memory Leaks**: Failing to properly manage widget references can lead to memory leaks.

2. **Performance Issues**: Creating too many widgets at once can cause performance issues, especially on lower-end devices.

3. **Stale References**: Be careful of stale references to widgets that have been removed from the cache.

4. **Thread Safety**: UMG widgets should only be accessed from the game thread.

5. **Circular Dependencies**: Be careful of circular dependencies between widget factories and other systems.

## Related Patterns

- [Widget Base Classes](Pattern1_WidgetBaseClasses.md)
- [Data Binding Patterns](Pattern2_DataBindingPatterns.md)
- [Performance Optimization](Pattern7_PerformanceOptimization.md) 