# Animated UI Components Pattern

## Problem
Creating reusable, performant, and visually appealing UI animations in Unreal Engine 5 that maintain consistency across the game while being easy to implement and modify.

## Solution Pattern
Implement a flexible animation system for UI components using a combination of UMG animations, C++ driven animations, and a centralized animation manager for consistent timing and easing functions.

## Code Example

```cpp
// FUIAnimationPreset.h
#pragma once

#include "CoreMinimal.h"
#include "FUIAnimationPreset.generated.h"

UENUM(BlueprintType)
enum class EUIAnimationType : uint8
{
    FadeInOut UMETA(DisplayName = "Fade In/Out"),
    SlideInOut UMETA(DisplayName = "Slide In/Out"),
    ScaleInOut UMETA(DisplayName = "Scale In/Out"),
    Bounce UMETA(DisplayName = "Bounce"),
    Custom UMETA(DisplayName = "Custom")
};

UENUM(BlueprintType)
enum class EUIEaseType : uint8
{
    Linear UMETA(DisplayName = "Linear"),
    EaseIn UMETA(DisplayName = "Ease In"),
    EaseOut UMETA(DisplayName = "Ease Out"),
    EaseInOut UMETA(DisplayName = "Ease In Out"),
    ExpoIn UMETA(DisplayName = "Expo In"),
    ExpoOut UMETA(DisplayName = "Expo Out"),
    ExpoInOut UMETA(DisplayName = "Expo In Out")
};

USTRUCT(BlueprintType)
struct YOURGAME_API FUIAnimationPreset
{
    GENERATED_BODY()
    
    // Type of animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Animation")
    EUIAnimationType AnimationType = EUIAnimationType::FadeInOut;
    
    // Easing function to use
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Animation")
    EUIEaseType EaseType = EUIEaseType::EaseInOut;
    
    // Duration in seconds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Animation")
    float Duration = 0.3f;
    
    // Delay before starting
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Animation")
    float StartDelay = 0.0f;
    
    // Whether to loop the animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Animation")
    bool bLooping = false;
    
    // Custom curve if using custom animation type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Animation")
    UCurveFloat* CustomCurve = nullptr;
};

// UAnimatedUserWidget.h
UCLASS()
class YOURGAME_API UAnimatedUserWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UAnimatedUserWidget(const FObjectInitializer& ObjectInitializer);
    
    // Play an animation preset
    UFUNCTION(BlueprintCallable, Category = "UI|Animation")
    void PlayAnimation(const FUIAnimationPreset& Preset);
    
    // Stop current animation
    UFUNCTION(BlueprintCallable, Category = "UI|Animation")
    void StopAnimation();
    
    // Reverse current animation
    UFUNCTION(BlueprintCallable, Category = "UI|Animation")
    void ReverseAnimation();
    
protected:
    // Current animation state
    UPROPERTY()
    FUIAnimationPreset CurrentPreset;
    
    // Animation timeline
    UPROPERTY()
    FTimeline AnimationTimeline;
    
    // Initial transform for resetting
    UPROPERTY()
    FWidgetTransform InitialTransform;
    
    // Setup animation timeline
    virtual void NativeConstruct() override;
    
    // Timeline callback
    UFUNCTION()
    void OnAnimationTick(float Alpha);
    
    // Animation finished callback
    UFUNCTION()
    void OnAnimationFinished();
    
    // Apply the animated transform
    void ApplyAnimatedTransform(float Alpha);
};

// UUIAnimationManager.h
UCLASS()
class YOURGAME_API UUIAnimationManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // Initialize subsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    
    // Get animation preset by name
    UFUNCTION(BlueprintCallable, Category = "UI|Animation")
    FUIAnimationPreset GetPreset(FName PresetName) const;
    
    // Register a new preset
    UFUNCTION(BlueprintCallable, Category = "UI|Animation")
    void RegisterPreset(FName PresetName, const FUIAnimationPreset& Preset);
    
protected:
    // Stored animation presets
    UPROPERTY()
    TMap<FName, FUIAnimationPreset> AnimationPresets;
    
    // Load default presets
    void LoadDefaultPresets();
};
```

### Implementation Example

```cpp
// UAnimatedUserWidget.cpp
#include "AnimatedUserWidget.h"
#include "TimerManager.h"
#include "Curves/CurveFloat.h"

UAnimatedUserWidget::UAnimatedUserWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAnimatedUserWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Store initial transform
    InitialTransform = RootWidget->GetRenderTransform();
    
    // Setup timeline
    FOnTimelineFloat ProgressCallback;
    ProgressCallback.BindDynamic(this, &UAnimatedUserWidget::OnAnimationTick);
    
    FOnTimelineEvent FinishedCallback;
    FinishedCallback.BindDynamic(this, &UAnimatedUserWidget::OnAnimationFinished);
    
    AnimationTimeline.AddInterpFloat(nullptr, ProgressCallback);
    AnimationTimeline.SetTimelineFinishedFunc(FinishedCallback);
}

void UAnimatedUserWidget::PlayAnimation(const FUIAnimationPreset& Preset)
{
    CurrentPreset = Preset;
    
    // Setup curve based on ease type
    UCurveFloat* Curve = CurrentPreset.CustomCurve;
    if (!Curve)
    {
        Curve = NewObject<UCurveFloat>(this);
        
        // Add keys based on ease type
        switch (CurrentPreset.EaseType)
        {
            case EUIEaseType::Linear:
                Curve->FloatCurve.AddKey(0.0f, 0.0f);
                Curve->FloatCurve.AddKey(1.0f, 1.0f);
                break;
            case EUIEaseType::EaseIn:
                Curve->FloatCurve.AddKey(0.0f, 0.0f, 0.0f, 0.0f, RCIM_Cubic);
                Curve->FloatCurve.AddKey(1.0f, 1.0f, 2.0f, 0.0f, RCIM_Cubic);
                break;
            // Add other ease types...
        }
    }
    
    // Update timeline
    AnimationTimeline.SetPlayRate(1.0f / CurrentPreset.Duration);
    AnimationTimeline.SetNewTime(0.0f);
    
    // Start animation after delay
    if (CurrentPreset.StartDelay > 0.0f)
    {
        FTimerHandle DelayHandle;
        GetWorld()->GetTimerManager().SetTimer(
            DelayHandle,
            [this]() { AnimationTimeline.Play(); },
            CurrentPreset.StartDelay,
            false
        );
    }
    else
    {
        AnimationTimeline.Play();
    }
}

void UAnimatedUserWidget::OnAnimationTick(float Alpha)
{
    ApplyAnimatedTransform(Alpha);
}

void UAnimatedUserWidget::OnAnimationFinished()
{
    if (CurrentPreset.bLooping)
    {
        AnimationTimeline.PlayFromStart();
    }
}

void UAnimatedUserWidget::ApplyAnimatedTransform(float Alpha)
{
    FWidgetTransform NewTransform = InitialTransform;
    
    switch (CurrentPreset.AnimationType)
    {
        case EUIAnimationType::FadeInOut:
            SetRenderOpacity(Alpha);
            break;
            
        case EUIAnimationType::SlideInOut:
            NewTransform.Translation = FVector2D(
                InitialTransform.Translation.X * (1.0f - Alpha),
                InitialTransform.Translation.Y
            );
            break;
            
        case EUIAnimationType::ScaleInOut:
            NewTransform.Scale = FVector2D(Alpha, Alpha);
            break;
            
        case EUIAnimationType::Bounce:
            // Implement bounce animation...
            break;
    }
    
    RootWidget->SetRenderTransform(NewTransform);
}
```

## Usage Example

```cpp
// In your widget blueprint or C++ code:

// Create and configure an animation preset
FUIAnimationPreset FadeInPreset;
FadeInPreset.AnimationType = EUIAnimationType::FadeInOut;
FadeInPreset.EaseType = EUIEaseType::EaseInOut;
FadeInPreset.Duration = 0.3f;

// Play the animation
UAnimatedUserWidget* Widget = Cast<UAnimatedUserWidget>(YourWidget);
if (Widget)
{
    Widget->PlayAnimation(FadeInPreset);
}

// Using the animation manager
UUIAnimationManager* AnimManager = GetGameInstance()->GetSubsystem<UUIAnimationManager>();
if (AnimManager)
{
    // Get a preset
    FUIAnimationPreset Preset = AnimManager->GetPreset(TEXT("DefaultFadeIn"));
    
    // Play it on your widget
    Widget->PlayAnimation(Preset);
}
```

## Best Practices

1. **Performance Optimization**:
   - Use GPU-driven animations when possible
   - Batch similar animations together
   - Avoid animating off-screen widgets
   - Use animation pooling for frequently spawned widgets

2. **Design Consistency**:
   - Store common animation presets in the animation manager
   - Use consistent timing and easing across similar UI elements
   - Document animation standards in your style guide

3. **Maintainability**:
   - Keep animation logic separate from widget logic
   - Use data-driven animation configurations
   - Create reusable animation blueprints for complex sequences

4. **Accessibility**:
   - Provide options to reduce or disable animations
   - Ensure animations don't interfere with UI navigation
   - Consider motion sensitivity in animation design

5. **Debug Support**:
   - Add debug visualization for animation paths
   - Log animation performance metrics
   - Implement slow-motion preview tools

## Integration Tips

1. **Blueprint Integration**:
   - Expose key animation parameters to blueprints
   - Create blueprint functions for common animation sequences
   - Use animation events for synchronization

2. **Asset Management**:
   - Create a library of reusable animation curves
   - Implement preset management system
   - Version control your animation assets

3. **Runtime Considerations**:
   - Handle interrupted animations gracefully
   - Support dynamic resolution scaling
   - Implement proper cleanup for removed widgets

## Customization Points

The pattern can be extended in several ways:

1. **Custom Animation Types**:
   - Add new animation types to `EUIAnimationType`
   - Implement custom interpolation methods
   - Create compound animations

2. **Advanced Features**:
   - Add support for spring physics
   - Implement path-based animations
   - Add particle system integration

3. **Tool Support**:
   - Create custom editor tools
   - Add preview functionality
   - Implement animation debugging tools

## Performance Considerations

1. **Optimization Techniques**:
   - Use GPU transforms when possible
   - Implement animation culling
   - Batch similar animations
   - Use timeline pooling

2. **Memory Management**:
   - Cache commonly used curves
   - Implement proper cleanup
   - Use object pooling for frequent animations

3. **CPU/GPU Balance**:
   - Choose appropriate animation methods
   - Monitor performance metrics
   - Implement LOD system for animations

## Troubleshooting

Common issues and solutions:

1. **Jerky Animations**:
   - Check frame rate consistency
   - Verify delta time usage
   - Review interpolation methods

2. **Memory Leaks**:
   - Ensure proper timeline cleanup
   - Check widget lifecycle management
   - Monitor animation system memory usage

3. **Performance Issues**:
   - Profile animation performance
   - Check batching effectiveness
   - Review animation complexity 