# Widget Animation in C++

This document outlines patterns and techniques for creating, managing, and controlling UMG widget animations programmatically in C++.

## Overview

While UMG animations can be created in the UMG Editor, implementing them in C++ provides more control, reusability, and runtime flexibility. This pattern focuses on different approaches to widget animation and best practices for performance and maintainability.

## Implementation

### 1. Animation Base Class

A base class that provides common functionality for widget animations.

```cpp
// WidgetAnimationBase.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Animation/UMGSequencePlayer.h"
#include "WidgetAnimationBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationFinished);

/**
 * Base class for widget animations.
 * Provides common functionality for animation control and events.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UWidgetAnimationBase : public UObject
{
    GENERATED_BODY()

public:
    UWidgetAnimationBase();

    // Initialize the animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    virtual void Initialize(class UUserWidget* InOwningWidget);

    // Play the animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    virtual void Play(float StartAtTime = 0.0f, int32 NumLoops = 1, EUMGSequencePlayMode::Type PlayMode = EUMGSequencePlayMode::Forward);

    // Stop the animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    virtual void Stop();

    // Pause the animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    virtual void Pause();

    // Resume the animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    virtual void Resume();

    // Reverse the animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    virtual void Reverse();

    // Get the current animation time
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    float GetCurrentTime() const;

    // Get the total duration of the animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    float GetTotalDuration() const;

    // Event fired when animation finishes
    UPROPERTY(BlueprintAssignable, Category = "Widget Animation")
    FOnAnimationFinished OnAnimationFinished;

protected:
    // The widget that owns this animation
    UPROPERTY()
    TWeakObjectPtr<class UUserWidget> OwningWidget;

    // The animation sequence player
    UPROPERTY()
    UUMGSequencePlayer* SequencePlayer;

    // Animation duration
    UPROPERTY(EditAnywhere, Category = "Widget Animation")
    float Duration;

    // Whether the animation is currently playing
    bool bIsPlaying;

    // Whether the animation is currently paused
    bool bIsPaused;

    // Current playback position
    float CurrentTime;

    // Handle animation finished
    UFUNCTION()
    virtual void HandleAnimationFinished();
};
```

```cpp
// WidgetAnimationBase.cpp
#include "UI/Animation/WidgetAnimationBase.h"
#include "Blueprint/UserWidget.h"

UWidgetAnimationBase::UWidgetAnimationBase()
    : Duration(1.0f)
    , bIsPlaying(false)
    , bIsPaused(false)
    , CurrentTime(0.0f)
{
}

void UWidgetAnimationBase::Initialize(UUserWidget* InOwningWidget)
{
    OwningWidget = InOwningWidget;
}

void UWidgetAnimationBase::Play(float StartAtTime, int32 NumLoops, EUMGSequencePlayMode::Type PlayMode)
{
    if (!OwningWidget.IsValid() || !SequencePlayer)
    {
        return;
    }

    bIsPlaying = true;
    bIsPaused = false;
    CurrentTime = StartAtTime;

    SequencePlayer->Play(StartAtTime, NumLoops, PlayMode);
}

void UWidgetAnimationBase::Stop()
{
    if (!SequencePlayer)
    {
        return;
    }

    bIsPlaying = false;
    bIsPaused = false;
    CurrentTime = 0.0f;

    SequencePlayer->Stop();
}

void UWidgetAnimationBase::Pause()
{
    if (!SequencePlayer || !bIsPlaying)
    {
        return;
    }

    bIsPaused = true;
    CurrentTime = SequencePlayer->GetCurrentTime();

    SequencePlayer->Pause();
}

void UWidgetAnimationBase::Resume()
{
    if (!SequencePlayer || !bIsPaused)
    {
        return;
    }

    bIsPaused = false;
    SequencePlayer->Play(CurrentTime);
}

void UWidgetAnimationBase::Reverse()
{
    if (!SequencePlayer)
    {
        return;
    }

    bIsPlaying = true;
    bIsPaused = false;

    SequencePlayer->PlayReverse();
}

float UWidgetAnimationBase::GetCurrentTime() const
{
    return SequencePlayer ? SequencePlayer->GetCurrentTime() : 0.0f;
}

float UWidgetAnimationBase::GetTotalDuration() const
{
    return Duration;
}

void UWidgetAnimationBase::HandleAnimationFinished()
{
    bIsPlaying = false;
    bIsPaused = false;
    CurrentTime = 0.0f;

    OnAnimationFinished.Broadcast();
}
```

### 2. Property Animation

A class for animating widget properties (e.g., opacity, position, scale).

```cpp
// PropertyAnimation.h
#pragma once

#include "CoreMinimal.h"
#include "UI/Animation/WidgetAnimationBase.h"
#include "PropertyAnimation.generated.h"

UENUM(BlueprintType)
enum class EPropertyAnimationType : uint8
{
    Float,
    Vector2D,
    LinearColor,
    Transform2D
};

/**
 * Class for animating widget properties.
 */
UCLASS()
class YOURGAME_API UPropertyAnimation : public UWidgetAnimationBase
{
    GENERATED_BODY()

public:
    UPropertyAnimation();

    // Initialize the property animation
    virtual void Initialize(class UUserWidget* InOwningWidget) override;

    // Set the property to animate
    UFUNCTION(BlueprintCallable, Category = "Property Animation")
    void SetPropertyPath(const FString& Path);

    // Set the start value
    template<typename T>
    void SetStartValue(const T& Value);

    // Set the end value
    template<typename T>
    void SetEndValue(const T& Value);

    // Set the animation curve
    UFUNCTION(BlueprintCallable, Category = "Property Animation")
    void SetAnimationCurve(UCurveFloat* Curve);

protected:
    // Property path
    UPROPERTY(EditAnywhere, Category = "Property Animation")
    FString PropertyPath;

    // Animation type
    UPROPERTY(EditAnywhere, Category = "Property Animation")
    EPropertyAnimationType AnimationType;

    // Animation curve
    UPROPERTY(EditAnywhere, Category = "Property Animation")
    UCurveFloat* AnimationCurve;

    // Start and end values
    FProperty* TargetProperty;
    void* StartValue;
    void* EndValue;

    // Update animation
    virtual void Tick(float DeltaTime);

private:
    // Helper to get property value
    template<typename T>
    T LerpValue(const T& Start, const T& End, float Alpha) const;
};
```

### 3. Sequence Animation

A class for creating complex animation sequences.

```cpp
// SequenceAnimation.h
#pragma once

#include "CoreMinimal.h"
#include "UI/Animation/WidgetAnimationBase.h"
#include "SequenceAnimation.generated.h"

USTRUCT(BlueprintType)
struct FAnimationStep
{
    GENERATED_BODY()

    // The animation to play
    UPROPERTY(EditAnywhere, Category = "Animation")
    UWidgetAnimationBase* Animation;

    // Delay before playing this step
    UPROPERTY(EditAnywhere, Category = "Animation")
    float Delay;

    // Whether to wait for this animation to finish before playing the next
    UPROPERTY(EditAnywhere, Category = "Animation")
    bool bWaitForCompletion;
};

/**
 * Class for creating animation sequences.
 */
UCLASS()
class YOURGAME_API USequenceAnimation : public UWidgetAnimationBase
{
    GENERATED_BODY()

public:
    USequenceAnimation();

    // Initialize the sequence
    virtual void Initialize(class UUserWidget* InOwningWidget) override;

    // Add an animation step
    UFUNCTION(BlueprintCallable, Category = "Sequence Animation")
    void AddStep(UWidgetAnimationBase* Animation, float Delay = 0.0f, bool bWaitForCompletion = true);

    // Play the sequence
    virtual void Play(float StartAtTime = 0.0f, int32 NumLoops = 1, EUMGSequencePlayMode::Type PlayMode = EUMGSequencePlayMode::Forward) override;

protected:
    // Animation steps
    UPROPERTY(EditAnywhere, Category = "Sequence Animation")
    TArray<FAnimationStep> Steps;

    // Current step index
    int32 CurrentStepIndex;

    // Play next step
    void PlayNextStep();

    // Handle step finished
    UFUNCTION()
    void HandleStepFinished();
};
```

### 4. Animation Manager

A manager class for handling multiple widget animations.

```cpp
// WidgetAnimationManager.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/Animation/WidgetAnimationBase.h"
#include "WidgetAnimationManager.generated.h"

/**
 * Manager class for widget animations.
 */
UCLASS()
class YOURGAME_API UWidgetAnimationManager : public UObject
{
    GENERATED_BODY()

public:
    // Create an animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    UWidgetAnimationBase* CreateAnimation(TSubclassOf<UWidgetAnimationBase> AnimationClass, UUserWidget* OwningWidget);

    // Play an animation
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    void PlayAnimation(UWidgetAnimationBase* Animation);

    // Stop all animations for a widget
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    void StopAllAnimations(UUserWidget* Widget);

    // Get active animations for a widget
    UFUNCTION(BlueprintCallable, Category = "Widget Animation")
    TArray<UWidgetAnimationBase*> GetActiveAnimations(UUserWidget* Widget) const;

private:
    // Map of widgets to their active animations
    TMap<TWeakObjectPtr<UUserWidget>, TArray<UWidgetAnimationBase*>> ActiveAnimations;

    // Clean up completed animations
    void CleanupAnimations();
};
```

## Usage Examples

### 1. Simple Property Animation

```cpp
// Example of using property animation to fade a widget
UPropertyAnimation* FadeAnimation = Cast<UPropertyAnimation>(
    AnimationManager->CreateAnimation(UPropertyAnimation::StaticClass(), this)
);

if (FadeAnimation)
{
    FadeAnimation->SetPropertyPath("RenderOpacity");
    FadeAnimation->SetStartValue<float>(1.0f);
    FadeAnimation->SetEndValue<float>(0.0f);
    FadeAnimation->SetDuration(0.5f);
    
    // Optional: Set a custom curve
    UCurveFloat* FadeCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/UI/Curves/FadeOutCurve"));
    if (FadeCurve)
    {
        FadeAnimation->SetAnimationCurve(FadeCurve);
    }
    
    FadeAnimation->Play();
}
```

### 2. Complex Animation Sequence

```cpp
// Example of creating an animation sequence
USequenceAnimation* MenuSequence = Cast<USequenceAnimation>(
    AnimationManager->CreateAnimation(USequenceAnimation::StaticClass(), this)
);

if (MenuSequence)
{
    // Create and add fade in animation
    UPropertyAnimation* FadeIn = Cast<UPropertyAnimation>(
        AnimationManager->CreateAnimation(UPropertyAnimation::StaticClass(), this)
    );
    FadeIn->SetPropertyPath("RenderOpacity");
    FadeIn->SetStartValue<float>(0.0f);
    FadeIn->SetEndValue<float>(1.0f);
    FadeIn->SetDuration(0.3f);
    
    // Create and add scale animation
    UPropertyAnimation* ScaleUp = Cast<UPropertyAnimation>(
        AnimationManager->CreateAnimation(UPropertyAnimation::StaticClass(), this)
    );
    ScaleUp->SetPropertyPath("RenderTransform.Scale");
    ScaleUp->SetStartValue<FVector2D>(FVector2D(0.8f, 0.8f));
    ScaleUp->SetEndValue<FVector2D>(FVector2D(1.0f, 1.0f));
    ScaleUp->SetDuration(0.4f);
    
    // Add steps to sequence
    MenuSequence->AddStep(FadeIn, 0.0f, true);
    MenuSequence->AddStep(ScaleUp, 0.1f, true);
    
    // Play the sequence
    MenuSequence->Play();
}
```

### 3. Animation with Events

```cpp
// Example of using animation events
UPropertyAnimation* SlideAnimation = Cast<UPropertyAnimation>(
    AnimationManager->CreateAnimation(UPropertyAnimation::StaticClass(), this)
);

if (SlideAnimation)
{
    SlideAnimation->SetPropertyPath("RenderTransform.Translation");
    SlideAnimation->SetStartValue<FVector2D>(FVector2D(-400.0f, 0.0f));
    SlideAnimation->SetEndValue<FVector2D>(FVector2D(0.0f, 0.0f));
    SlideAnimation->SetDuration(0.6f);
    
    // Bind to animation finished event
    SlideAnimation->OnAnimationFinished.AddDynamic(this, &UMyWidget::HandleSlideFinished);
    
    SlideAnimation->Play();
}

UFUNCTION()
void UMyWidget::HandleSlideFinished()
{
    // Animation finished, perform additional actions
    OnSlideComplete.Broadcast();
}
```

## Best Practices

1. **Use Animation Pooling**: Reuse animation objects when possible to reduce memory allocation.

2. **Clean Up Animations**: Always clean up animations when widgets are destroyed.

3. **Optimize Performance**: Use simple animations for frequently updated widgets.

4. **Handle Edge Cases**: Account for interrupted animations and widget visibility changes.

5. **Use Appropriate Curves**: Choose appropriate easing curves for smooth animations.

6. **Cache Property Paths**: Cache property paths and values to avoid repeated lookups.

7. **Handle Mobile Performance**: Be mindful of animation performance on mobile devices.

## Common Pitfalls

1. **Memory Leaks**: Not cleaning up animation objects properly.

2. **Performance Issues**: Creating too many simultaneous animations.

3. **Timing Issues**: Not handling animation interruptions properly.

4. **Property Access**: Invalid property paths causing animations to fail.

5. **Resource Management**: Not managing animation resources efficiently.

## Related Patterns

- [Widget Base Classes](Pattern1_WidgetBaseClasses.md)
- [Widget Factory System](Pattern3_WidgetFactorySystem.md)
- [Performance Optimization](Pattern7_PerformanceOptimization.md) 