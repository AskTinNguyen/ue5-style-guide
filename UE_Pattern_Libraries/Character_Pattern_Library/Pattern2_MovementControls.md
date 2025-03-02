# Pattern 2: Movement and Controls

## Problem Definition

Character movement in modern games requires sophisticated systems that can:
- Handle multiple movement modes (walking, running, swimming, flying, etc.)
- Support smooth transitions between states
- Provide responsive and predictable controls
- Integrate with animation systems
- Work reliably in networked environments
- Allow for game-specific movement mechanics

Standard movement components often lack the flexibility needed for unique gameplay mechanics, while completely custom solutions can be complex and error-prone.

## Solution Approach

Extend Unreal's character movement component with a state-based architecture that:
1. Encapsulates movement behaviors in distinct, swappable states
2. Provides a clean interface for animation systems
3. Handles network prediction and correction
4. Supports custom movement mechanics through composition rather than inheritance

## Implementation

### Enhanced Movement Component

```cpp
// EnhancedCharacterMovement.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedCharacterMovement.generated.h"

class UMovementStateBase;

// Custom movement modes (extending the built-in EMovementMode)
UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
    CMOVE_None = 0,
    CMOVE_Climbing,
    CMOVE_Sliding,
    CMOVE_Vaulting,
    CMOVE_WallRunning,
    CMOVE_MAX
};

UCLASS()
class YOURGAME_API UEnhancedCharacterMovement : public UCharacterMovementComponent
{
    GENERATED_BODY()
    
public:
    UEnhancedCharacterMovement();
    
    // Movement state management
    UFUNCTION(BlueprintCallable, Category = "Character Movement")
    void SetMovementState(TSubclassOf<UMovementStateBase> NewState);
    
    UFUNCTION(BlueprintCallable, Category = "Character Movement")
    UMovementStateBase* GetCurrentMovementState() const { return CurrentMovementState; }
    
    UFUNCTION(BlueprintCallable, Category = "Character Movement")
    bool IsInMovementState(TSubclassOf<UMovementStateBase> StateClass) const;
    
    // Custom movement mode helpers
    UFUNCTION(BlueprintCallable, Category = "Character Movement")
    bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
    
    UFUNCTION(BlueprintCallable, Category = "Character Movement")
    void SetCustomMovementMode(ECustomMovementMode NewCustomMovementMode);
    
    // Movement capability queries
    UFUNCTION(BlueprintCallable, Category = "Character Movement")
    bool CanAttemptJump() const;
    
    UFUNCTION(BlueprintCallable, Category = "Character Movement")
    bool CanEnterState(TSubclassOf<UMovementStateBase> StateClass) const;
    
    // Input handling
    virtual void ProcessInputVector(FVector& InputVector) override;
    
    // Animation data access
    UFUNCTION(BlueprintCallable, Category = "Character Movement|Animation")
    FVector GetMovementVelocityForAnimation() const;
    
    UFUNCTION(BlueprintCallable, Category = "Character Movement|Animation")
    float GetMovementDirectionAngle() const;
    
protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Movement state handling
    UPROPERTY()
    UMovementStateBase* CurrentMovementState;
    
    UPROPERTY(EditDefaultsOnly, Category = "Movement States")
    TSubclassOf<UMovementStateBase> DefaultMovementState;
    
    UPROPERTY(EditDefaultsOnly, Category = "Movement States")
    TMap<TSubclassOf<UMovementStateBase>, TSubclassOf<UMovementStateBase>> StateTransitionMap;
    
    // Override movement physics
    virtual void PhysWalking(float deltaTime, int32 Iterations) override;
    virtual void PhysCustom(float deltaTime, int32 Iterations) override;
    
    // Network
    virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
    virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
};
```

### Movement State Base Class

```cpp
// MovementStateBase.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MovementStateBase.generated.h"

class UEnhancedCharacterMovement;
class ACharacter;

UCLASS(Abstract, BlueprintType, Blueprintable)
class YOURGAME_API UMovementStateBase : public UObject
{
    GENERATED_BODY()
    
public:
    UMovementStateBase();
    
    // State lifecycle
    virtual void InitializeState(UEnhancedCharacterMovement* MovementComponent);
    virtual void UninitializeState();
    
    virtual void OnEnterState(UMovementStateBase* PreviousState);
    virtual void OnExitState(UMovementStateBase* NextState);
    
    virtual void UpdateState(float DeltaTime);
    
    // State queries
    UFUNCTION(BlueprintCallable, Category = "Movement State")
    virtual bool CanEnterState() const;
    
    UFUNCTION(BlueprintCallable, Category = "Movement State")
    virtual bool CanExitState() const;
    
    // Input handling
    virtual void ProcessInputVector(FVector& InputVector);
    
    // Movement physics
    virtual void PhysicsUpdate(float DeltaTime);
    
    // Animation data
    virtual FVector GetVelocityForAnimation() const;
    
    // Getters
    FORCEINLINE UEnhancedCharacterMovement* GetMovementComponent() const { return MovementComponent; }
    FORCEINLINE ACharacter* GetCharacterOwner() const { return CharacterOwner; }
    
protected:
    UPROPERTY()
    UEnhancedCharacterMovement* MovementComponent;
    
    UPROPERTY()
    ACharacter* CharacterOwner;
    
    // Helper functions
    FVector GetCharacterForwardVector() const;
    FVector GetCharacterRightVector() const;
    FVector GetCharacterUpVector() const;
    
    float GetCharacterSpeed() const;
    FVector GetCharacterVelocity() const;
};
```

### Example Movement State: Sprinting

```cpp
// SprintingState.h
#pragma once

#include "CoreMinimal.h"
#include "MovementStateBase.h"
#include "SprintingState.generated.h"

UCLASS()
class YOURGAME_API USprintingState : public UMovementStateBase
{
    GENERATED_BODY()
    
public:
    USprintingState();
    
    virtual void OnEnterState(UMovementStateBase* PreviousState) override;
    virtual void OnExitState(UMovementStateBase* NextState) override;
    virtual void UpdateState(float DeltaTime) override;
    
    virtual bool CanEnterState() const override;
    virtual bool CanExitState() const override;
    
    virtual void ProcessInputVector(FVector& InputVector) override;
    
protected:
    // Configuration
    UPROPERTY(EditDefaultsOnly, Category = "Sprint")
    float SprintSpeedMultiplier;
    
    UPROPERTY(EditDefaultsOnly, Category = "Sprint")
    float SprintAccelerationMultiplier;
    
    UPROPERTY(EditDefaultsOnly, Category = "Sprint")
    float StaminaDrainRate;
    
    UPROPERTY(EditDefaultsOnly, Category = "Sprint")
    float MinimumStaminaToSprint;
    
    // State
    float OriginalMaxWalkSpeed;
    float OriginalMaxAcceleration;
    
    // Helpers
    bool HasEnoughStamina() const;
    void DrainStamina(float DeltaTime);
};
```

### Input Handling

```cpp
// EnhancedPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedPlayerController.generated.h"

UCLASS()
class YOURGAME_API AEnhancedPlayerController : public APlayerController
{
    GENERATED_BODY()
    
public:
    AEnhancedPlayerController();
    
    // Input action handlers
    UFUNCTION(BlueprintCallable, Category = "Character|Input")
    void OnMoveForward(float Value);
    
    UFUNCTION(BlueprintCallable, Category = "Character|Input")
    void OnMoveRight(float Value);
    
    UFUNCTION(BlueprintCallable, Category = "Character|Input")
    void OnStartJump();
    
    UFUNCTION(BlueprintCallable, Category = "Character|Input")
    void OnStopJump();
    
    UFUNCTION(BlueprintCallable, Category = "Character|Input")
    void OnStartSprint();
    
    UFUNCTION(BlueprintCallable, Category = "Character|Input")
    void OnStopSprint();
    
    UFUNCTION(BlueprintCallable, Category = "Character|Input")
    void OnStartCrouch();
    
    UFUNCTION(BlueprintCallable, Category = "Character|Input")
    void OnStopCrouch();
    
protected:
    virtual void SetupInputComponent() override;
    
    // Input state
    FVector MovementInput;
    
    // Helper functions
    class ACharacter* GetControlledCharacter() const;
    class UEnhancedCharacterMovement* GetEnhancedMovement() const;
};
```

## Integration with Animation

### Animation Interface

```cpp
// CharacterAnimationInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterAnimationInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCharacterAnimationInterface : public UInterface
{
    GENERATED_BODY()
};

class YOURGAME_API ICharacterAnimationInterface
{
    GENERATED_BODY()
    
public:
    // Movement data for animation
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    FVector GetMovementVelocity() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    float GetMovementDirection() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    bool IsInAir() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    bool IsCrouching() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    bool IsSprinting() const;
    
    // State queries for animation
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    FName GetCurrentMovementStateName() const;
    
    // Upper body animation control
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    bool IsAiming() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    float GetAimPitch() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    float GetAimYaw() const;
};
```

## Integration Tips

1. **Character Setup**:
   - Replace the standard CharacterMovementComponent with EnhancedCharacterMovement
   - Implement the CharacterAnimationInterface on your character class
   - Set up appropriate animation blueprints that query the interface

2. **Input Mapping**:
   - Map player inputs to the controller's input handlers
   - Consider using Enhanced Input system for more complex input scenarios

3. **Movement State Transitions**:
   - Define clear rules for state transitions in the StateTransitionMap
   - Implement CanEnterState and CanExitState for each movement state

4. **Network Considerations**:
   - Ensure movement states are properly replicated
   - Use server authority for critical movement decisions
   - Implement client prediction for responsive controls

## Example Usage Scenarios

### Setting Up a Character with Enhanced Movement

```cpp
// YourCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterAnimationInterface.h"
#include "YourCharacter.generated.h"

class UEnhancedCharacterMovement;

UCLASS()
class YOURGAME_API AYourCharacter : public ACharacter, public ICharacterAnimationInterface
{
    GENERATED_BODY()
    
public:
    AYourCharacter(const FObjectInitializer& ObjectInitializer);
    
    // Animation interface implementation
    virtual FVector GetMovementVelocity_Implementation() const override;
    virtual float GetMovementDirection_Implementation() const override;
    virtual bool IsInAir_Implementation() const override;
    virtual bool IsCrouching_Implementation() const override;
    virtual bool IsSprinting_Implementation() const override;
    virtual FName GetCurrentMovementStateName_Implementation() const override;
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEnhancedCharacterMovement* EnhancedMovement;
};

// YourCharacter.cpp
AYourCharacter::AYourCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UEnhancedCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
    // Get the enhanced movement component
    EnhancedMovement = Cast<UEnhancedCharacterMovement>(GetCharacterMovement());
}
```

### Creating a Custom Movement State

```cpp
// WallRunState.h
#pragma once

#include "CoreMinimal.h"
#include "MovementStateBase.h"
#include "WallRunState.generated.h"

UCLASS()
class YOURGAME_API UWallRunState : public UMovementStateBase
{
    GENERATED_BODY()
    
public:
    UWallRunState();
    
    virtual void OnEnterState(UMovementStateBase* PreviousState) override;
    virtual void OnExitState(UMovementStateBase* NextState) override;
    virtual void UpdateState(float DeltaTime) override;
    virtual bool CanEnterState() const override;
    
    virtual void PhysicsUpdate(float DeltaTime) override;
    
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Wall Run")
    float WallRunSpeed;
    
    UPROPERTY(EditDefaultsOnly, Category = "Wall Run")
    float WallRunGravityScale;
    
    UPROPERTY(EditDefaultsOnly, Category = "Wall Run")
    float WallCheckDistance;
    
    UPROPERTY(EditDefaultsOnly, Category = "Wall Run")
    float MinWallRunHeight;
    
    UPROPERTY(EditDefaultsOnly, Category = "Wall Run")
    float MaxWallRunTime;
    
    // State
    float CurrentWallRunTime;
    FVector WallNormal;
    
    // Helpers
    bool FindWallRunSurface(FVector& OutWallNormal) const;
    void ApplyWallRunForces(float DeltaTime);
};
```

## AI Prompt Templates

### Creating a New Movement State

```
Create a new movement state for [SPECIFIC MOVEMENT TYPE] that extends UMovementStateBase.
The state should handle [SPECIFIC REQUIREMENTS] and integrate with the EnhancedCharacterMovement component.
Include proper physics calculations, state transitions, and animation data exposure.
```

### Debugging Movement Issues

```
Help me debug an issue with my character movement where [SPECIFIC PROBLEM].
The expected behavior is [DESCRIPTION], but I'm seeing [ACTUAL BEHAVIOR].
Here's my movement state implementation: [CODE]
```

## Performance Considerations

1. **Physics Calculations**:
   - Keep physics calculations efficient, especially in frequently used states
   - Consider using simplified physics for distant characters (LOD for movement)

2. **State Transitions**:
   - Avoid rapid state transitions that can cause jitter
   - Implement hysteresis for threshold-based transitions

3. **Network Bandwidth**:
   - Minimize the data needed to replicate movement states
   - Use compression techniques for movement data

## Common Pitfalls

1. **Physics Complexity**:
   - Custom physics can easily become unstable
   - Test edge cases thoroughly (high speed, collision corners, etc.)

2. **Animation Synchronization**:
   - Ensure movement states properly communicate with animation system
   - Handle transitions smoothly to avoid animation pops

3. **Input Responsiveness**:
   - Balance between responsive controls and realistic movement
   - Consider input buffering for action sequences