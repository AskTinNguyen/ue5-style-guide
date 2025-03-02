# Pattern 4: Input-Driven UI Management

## Problem Definition
Managing input across multiple UI layers in a game can become complex, especially when dealing with different input methods (keyboard, gamepad, touch) and maintaining proper focus navigation between widgets.

## Solution Approach
Create a robust input management system that handles UI navigation, focus management, and input prioritization across different widget layers while supporting multiple input devices.

## Core Components

### 1. Input Manager Class
```cpp
// InputUIManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "InputUIManager.generated.h"

UENUM(BlueprintType)
enum class EUIInputMode : uint8
{
    Game,           // Game receives input
    UI,             // UI receives input
    GameAndUI       // Both receive input
};

UCLASS()
class YOURGAME_API UInputUIManager : public UObject
{
    GENERATED_BODY()

public:
    UInputUIManager();

    // Initialize the manager
    void Initialize();

    // Set current input mode
    UFUNCTION(BlueprintCallable, Category = "UI|Input")
    void SetInputMode(EUIInputMode NewMode);

    // Handle focus changes between widgets
    UFUNCTION(BlueprintCallable, Category = "UI|Input")
    void SetFocusedWidget(UWidget* Widget);

    // Push/Pop UI layers
    void PushUILayer(class UUILayerComponent* Layer);
    void PopUILayer();

protected:
    // Current input mode
    UPROPERTY()
    EUIInputMode CurrentInputMode;

    // Stack of active UI layers
    UPROPERTY()
    TArray<class UUILayerComponent*> UILayerStack;

    // Currently focused widget
    UPROPERTY()
    TWeakObjectPtr<UWidget> FocusedWidget;

    // Handle input mode changes
    void UpdateInputMode();
};
```

### 2. UI Layer Component
```cpp
// UILayerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UILayerComponent.generated.h"

UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UUILayerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Layer priority (higher numbers = higher priority)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    int32 LayerPriority;

    // Whether this layer blocks input to lower layers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    bool bBlocksLowerLayers;

    // Input handling methods
    virtual bool HandleKeyInput(const FKeyEvent& KeyEvent);
    virtual bool HandleAnalogInput(const FAnalogInputEvent& AnalogInputEvent);
    virtual bool HandleMouseInput(const FPointerEvent& MouseEvent);

protected:
    // Reference to the widget owned by this layer
    UPROPERTY()
    class UUserWidget* LayerWidget;
};
```

### 3. Focus Navigation System
```cpp
// FocusNavigationSystem.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FocusNavigationSystem.generated.h"

UCLASS()
class YOURGAME_API UFocusNavigationSystem : public UObject
{
    GENERATED_BODY()

public:
    // Navigate focus in specified direction
    UFUNCTION(BlueprintCallable, Category = "UI|Navigation")
    bool NavigateFocus(EUINavigation Direction);

    // Set custom navigation rules
    UFUNCTION(BlueprintCallable, Category = "UI|Navigation")
    void SetCustomNavigation(UWidget* Widget, EUINavigation Direction, UWidget* TargetWidget);

protected:
    // Map of custom navigation rules
    TMap<TWeakObjectPtr<UWidget>, TMap<EUINavigation, TWeakObjectPtr<UWidget>>> CustomNavigationRules;

    // Find next focusable widget
    UWidget* FindNextFocusableWidget(UWidget* CurrentWidget, EUINavigation Direction);
};
```

## Implementation Guide

### 1. Setting Up the Input Manager

```cpp
// In your GameMode or similar initialization point
void AYourGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Create and initialize input manager
    InputManager = NewObject<UInputUIManager>(this);
    InputManager->Initialize();
}
```

### 2. Creating UI Layers

```cpp
// Example UI layer setup in Blueprint or C++
UCLASS()
class YOURGAME_API UMainMenuLayer : public UUILayerComponent
{
    GENERATED_BODY()

public:
    UMainMenuLayer()
    {
        LayerPriority = 100;
        bBlocksLowerLayers = true;
    }

    virtual bool HandleKeyInput(const FKeyEvent& KeyEvent) override
    {
        // Handle specific key inputs for this layer
        if (KeyEvent.GetKey() == EKeys::Escape)
        {
            // Handle escape key
            return true; // Input handled
        }
        return false; // Input not handled
    }
};
```

### 3. Implementing Focus Navigation

```cpp
// In your widget class
void UYourWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Set up custom navigation
    UFocusNavigationSystem* NavSystem = GetGameInstance()->GetSubsystem<UFocusNavigationSystem>();
    if (NavSystem)
    {
        NavSystem->SetCustomNavigation(Button1, EUINavigation::Down, Button2);
        NavSystem->SetCustomNavigation(Button2, EUINavigation::Up, Button1);
    }
}
```

## Integration Tips

1. **Layer Management**:
   - Maintain a clear hierarchy of UI layers
   - Use priorities to determine input handling order
   - Consider layer transitions and animations

2. **Input Handling**:
   - Implement input buffering for responsive UI
   - Support multiple input devices simultaneously
   - Handle edge cases like disconnected controllers

3. **Focus Management**:
   - Implement clear focus indicators
   - Support keyboard/gamepad navigation
   - Handle focus restoration when layers are closed

## Performance Considerations

1. **Input Processing**:
   - Batch input events when possible
   - Use event throttling for continuous inputs
   - Cache frequently accessed widgets

2. **Memory Management**:
   - Use weak pointers for widget references
   - Clean up custom navigation rules
   - Properly handle widget destruction

## Blueprint Usage Example

```cpp
// Blueprint-exposed functions for easy integration
UFUNCTION(BlueprintCallable, Category = "UI|Input")
void SetupUINavigation(UWidget* FromWidget, UWidget* ToWidget, EUINavigation Direction)
{
    if (UFocusNavigationSystem* NavSystem = GetFocusNavigationSystem())
    {
        NavSystem->SetCustomNavigation(FromWidget, Direction, ToWidget);
    }
}
```

## AI Prompt Templates

1. **Setting Up Input Manager**:
```
Help me implement an input manager for my UE5 UI system that needs to:
- Handle both keyboard and gamepad input
- Support multiple UI layers
- Manage focus navigation between widgets
```

2. **Creating Custom Navigation**:
```
I need to create custom navigation rules for my UI where:
- Widget A should focus Widget B when pressing down
- Widget B should focus Widget C when pressing right
- The navigation should work with both keyboard and gamepad
```

3. **Implementing Layer Management**:
```
Help me create a UI layer system that:
- Manages multiple overlapping UI elements
- Handles input priority between layers
- Supports modal dialogs and popups
```

## Common Issues and Solutions

1. **Input Not Reaching Widgets**
   - Check input mode settings
   - Verify layer priorities
   - Ensure proper focus management

2. **Focus Navigation Issues**
   - Validate widget visibility
   - Check custom navigation rules
   - Verify widget hierarchy

3. **Controller Support Problems**
   - Implement proper gamepad focus indicators
   - Handle controller disconnection
   - Support multiple controller types

## Testing Recommendations

1. **Input Testing**:
   - Test all supported input devices
   - Verify input blocking behavior
   - Check edge cases (rapid inputs, multiple keys)

2. **Navigation Testing**:
   - Test all navigation directions
   - Verify focus indicators
   - Check keyboard/gamepad compatibility

3. **Layer Testing**:
   - Test layer stack operations
   - Verify priority system
   - Check transition animations

## Related Patterns

- [Pattern 1: Responsive Widget Base](Pattern1_ResponsiveWidgetBase.md)
- [Pattern 2: Data-Binding Widget](Pattern2_DataBindingWidget.md)
- [Pattern 3: Animated UI Components](Pattern3_AnimatedUIComponents.md)
- [Pattern 5: Game HUD Framework](Pattern5_GameHUDFramework.md) 