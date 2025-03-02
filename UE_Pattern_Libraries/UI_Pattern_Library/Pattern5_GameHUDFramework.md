# Pattern 5: Game HUD Framework

## Problem Definition
Creating a flexible and maintainable HUD system that can handle various gameplay elements, notifications, and contextual information while maintaining performance and scalability.

## Solution Approach
Implement a modular HUD framework that separates concerns into distinct components, manages state efficiently, and provides a clean interface for gameplay systems to interact with UI elements.

## Core Components

### 1. Base HUD Class
```cpp
// GameplayHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayHUD.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDStateChanged, EHUDState, NewState);

UENUM(BlueprintType)
enum class EHUDState : uint8
{
    Default,
    Combat,
    Inventory,
    Dialogue,
    Cinematic
};

UCLASS()
class YOURGAME_API AGameplayHUD : public AHUD
{
    GENERATED_BODY()

public:
    AGameplayHUD();

    // State management
    UFUNCTION(BlueprintCallable, Category = "HUD|State")
    void SetHUDState(EHUDState NewState);

    // Widget management
    UFUNCTION(BlueprintCallable, Category = "HUD|Widgets")
    void ShowWidget(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder = 0);

    UFUNCTION(BlueprintCallable, Category = "HUD|Widgets")
    void HideWidget(TSubclassOf<UUserWidget> WidgetClass);

    // Notification system
    UFUNCTION(BlueprintCallable, Category = "HUD|Notifications")
    void ShowNotification(const FText& Message, float Duration = 3.0f);

protected:
    // Current HUD state
    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    EHUDState CurrentState;

    // Active widgets
    UPROPERTY()
    TMap<TSubclassOf<UUserWidget>, UUserWidget*> ActiveWidgets;

    // State changed delegate
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events")
    FOnHUDStateChanged OnHUDStateChanged;

    // Initialize HUD components
    virtual void BeginPlay() override;
};
```

### 2. HUD Component System
```cpp
// HUDComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUDComponent.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup=(UI))
class YOURGAME_API UHUDComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Initialize component
    virtual void InitializeComponent() override;

    // Update component
    UFUNCTION(BlueprintCallable, Category = "HUD|Component")
    virtual void UpdateDisplay();

    // Show/Hide component
    UFUNCTION(BlueprintCallable, Category = "HUD|Component")
    virtual void Show();

    UFUNCTION(BlueprintCallable, Category = "HUD|Component")
    virtual void Hide();

protected:
    // Reference to parent HUD
    UPROPERTY()
    AGameplayHUD* ParentHUD;

    // Widget class to spawn
    UPROPERTY(EditDefaultsOnly, Category = "HUD|Component")
    TSubclassOf<UUserWidget> WidgetClass;
};
```

### 3. Notification System
```cpp
// HUDNotificationManager.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HUDNotificationManager.generated.h"

USTRUCT(BlueprintType)
struct FHUDNotification
{
    GENERATED_BODY()

    UPROPERTY()
    FText Message;

    UPROPERTY()
    float Duration;

    UPROPERTY()
    float TimeRemaining;
};

UCLASS()
class YOURGAME_API UHUDNotificationManager : public UObject
{
    GENERATED_BODY()

public:
    // Add new notification
    void AddNotification(const FText& Message, float Duration);

    // Update notifications
    void UpdateNotifications(float DeltaTime);

    // Get active notifications
    const TArray<FHUDNotification>& GetActiveNotifications() const;

private:
    UPROPERTY()
    TArray<FHUDNotification> ActiveNotifications;
};
```

## Implementation Guide

### 1. Setting Up the HUD

```cpp
// In your GameMode
void AYourGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Ensure HUD class is set
    if (AGameplayHUD* GameHUD = Cast<AGameplayHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
    {
        // Initialize HUD components
        GameHUD->InitializeComponents();
    }
}
```

### 2. Creating HUD Components

```cpp
// Example health component
UCLASS()
class YOURGAME_API UHealthHUDComponent : public UHUDComponent
{
    GENERATED_BODY()

public:
    virtual void UpdateDisplay() override
    {
        if (UHealthWidget* HealthWidget = Cast<UHealthWidget>(GetWidget()))
        {
            // Update health display
            APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
            if (Player)
            {
                HealthWidget->UpdateHealth(Player->GetCurrentHealth(), Player->GetMaxHealth());
            }
        }
    }
};
```

### 3. Implementing Notifications

```cpp
// In your HUD class
void AGameplayHUD::ShowNotification(const FText& Message, float Duration)
{
    if (NotificationManager)
    {
        NotificationManager->AddNotification(Message, Duration);
        
        // Update notification widget
        if (UNotificationWidget* Widget = Cast<UNotificationWidget>(GetWidget(NotificationWidgetClass)))
        {
            Widget->RefreshNotifications(NotificationManager->GetActiveNotifications());
        }
    }
}
```

## Integration Tips

1. **Component Organization**:
   - Group related HUD elements into components
   - Use data-driven approach for component configuration
   - Implement clear communication between components

2. **State Management**:
   - Define clear state transitions
   - Handle component visibility based on state
   - Implement state-specific behaviors

3. **Performance Optimization**:
   - Use widget pooling for frequently shown/hidden elements
   - Implement efficient update mechanisms
   - Batch visual updates when possible

## Performance Considerations

1. **Widget Management**:
   - Pool commonly used widgets
   - Minimize widget creation/destruction
   - Use widget visibility over creation/destruction

2. **Update Frequency**:
   - Implement smart update throttling
   - Use event-driven updates when possible
   - Batch multiple updates together

## Blueprint Usage Example

```cpp
// Blueprint-exposed functions
UFUNCTION(BlueprintCallable, Category = "HUD|Components")
void RegisterHUDComponent(TSubclassOf<UHUDComponent> ComponentClass)
{
    if (ComponentClass)
    {
        UHUDComponent* NewComponent = NewObject<UHUDComponent>(this, ComponentClass);
        if (NewComponent)
        {
            NewComponent->RegisterComponent();
            HUDComponents.Add(NewComponent);
        }
    }
}
```

## AI Prompt Templates

1. **Creating HUD Components**:
```
Help me create a HUD component for my UE5 game that:
- Displays player status effects
- Updates based on game events
- Supports different visual states
```

2. **Implementing Notifications**:
```
I need a notification system that:
- Shows temporary messages
- Supports different notification types
- Has smooth animations
```

3. **State Management**:
```
Help me implement HUD state management for:
- Combat mode
- Inventory screen
- Dialogue system
- Cinematic sequences
```

## Common Issues and Solutions

1. **Performance Issues**
   - Implement widget pooling
   - Use efficient update methods
   - Optimize widget hierarchy

2. **State Conflicts**
   - Implement proper state hierarchy
   - Handle state transitions
   - Manage component dependencies

3. **Update Timing**
   - Use appropriate tick groups
   - Implement update throttling
   - Handle frame rate independence

## Testing Recommendations

1. **Component Testing**:
   - Test component initialization
   - Verify update mechanisms
   - Check state transitions

2. **Performance Testing**:
   - Monitor frame rate impact
   - Test with many active elements
   - Profile memory usage

3. **Integration Testing**:
   - Test component interactions
   - Verify state management
   - Check notification system

## Related Patterns

- [Pattern 1: Responsive Widget Base](Pattern1_ResponsiveWidgetBase.md)
- [Pattern 2: Data-Binding Widget](Pattern2_DataBindingWidget.md)
- [Pattern 3: Animated UI Components](Pattern3_AnimatedUIComponents.md)
- [Pattern 4: Input-Driven UI Management](Pattern4_InputDrivenUIManagement.md)

## Example Usage Scenarios

1. **Combat HUD**:
```cpp
// Combat HUD component example
UCLASS()
class UCombatHUDComponent : public UHUDComponent
{
    GENERATED_BODY()

public:
    // Update combat information
    void UpdateCombatInfo(float Health, float Stamina, const TArray<FStatusEffect>& StatusEffects)
    {
        if (UCombatWidget* Widget = Cast<UCombatWidget>(GetWidget()))
        {
            Widget->UpdateHealth(Health);
            Widget->UpdateStamina(Stamina);
            Widget->UpdateStatusEffects(StatusEffects);
        }
    }
};
```

2. **Inventory Display**:
```cpp
// Inventory HUD component
UCLASS()
class UInventoryHUDComponent : public UHUDComponent
{
    GENERATED_BODY()

public:
    // Show inventory grid
    void ShowInventory(const TArray<FInventoryItem>& Items)
    {
        if (UInventoryWidget* Widget = Cast<UInventoryWidget>(GetWidget()))
        {
            Widget->PopulateGrid(Items);
            Show();
        }
    }
};
```

3. **Quest Tracker**:
```cpp
// Quest tracker component
UCLASS()
class UQuestTrackerComponent : public UHUDComponent
{
    GENERATED_BODY()

public:
    // Update quest progress
    void UpdateQuestProgress(const FQuest& Quest, float Progress)
    {
        if (UQuestWidget* Widget = Cast<UQuestWidget>(GetWidget()))
        {
            Widget->UpdateQuest(Quest, Progress);
        }
    }
};
``` 