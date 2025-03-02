# Target Lock System Pattern

## Problem
Implementing a reliable and responsive target locking (lock-on) system for third-person combat that allows players to focus on a specific enemy while maintaining camera control.

## Solution Pattern
Create a component-based target lock system that manages target selection, camera control, and player input handling during locked states.

## Code Example

```cpp
// UTargetLockComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetLockComponent.generated.h"

// Forward declarations
class UCameraComponent;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetLockChanged, AActor*, NewTarget);

/**
 * Component that handles targeting and locking onto enemies for combat
 */
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UTargetLockComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTargetLockComponent();
    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void BeginPlay() override;
    
    // Toggle the target lock state
    UFUNCTION(BlueprintCallable, Category = "Combat|Target Lock")
    void ToggleTargetLock();
    
    // Force lock onto a specific target
    UFUNCTION(BlueprintCallable, Category = "Combat|Target Lock")
    bool LockOnTarget(AActor* Target);
    
    // Clear current target and disable lock
    UFUNCTION(BlueprintCallable, Category = "Combat|Target Lock")
    void ClearTargetLock();
    
    // Switch to the next valid target (left/right)
    UFUNCTION(BlueprintCallable, Category = "Combat|Target Lock")
    bool SwitchTarget(bool bSwitchToRightTarget);
    
    // Get the currently locked target
    UFUNCTION(BlueprintPure, Category = "Combat|Target Lock")
    AActor* GetCurrentTarget() const { return CurrentTarget; }
    
    // Check if currently locked onto a target
    UFUNCTION(BlueprintPure, Category = "Combat|Target Lock")
    bool IsLockActive() const { return CurrentTarget != nullptr; }
    
    // Event dispatched when target changes
    UPROPERTY(BlueprintAssignable, Category = "Combat|Target Lock")
    FOnTargetLockChanged OnTargetLockChanged;
    
protected:
    // Currently locked target
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat|Target Lock")
    AActor* CurrentTarget;
    
    // Maximum distance to consider targets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Target Lock")
    float MaxTargetingDistance = 1500.0f;
    
    // Field of view angle to consider targets (in degrees)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Target Lock")
    float TargetingFOV = 60.0f;
    
    // How quickly the camera interpolates to the target
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Target Lock")
    float CameraInterpolationSpeed = 5.0f;
    
    // References to camera components
    UPROPERTY()
    UCameraComponent* PlayerCamera;
    
    UPROPERTY()
    USpringArmComponent* CameraBoom;
    
    // Tag for targetable actors
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Target Lock")
    FName TargetableTag = "Targetable";
    
    // Target indicator widget class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Target Lock")
    TSubclassOf<UUserWidget> TargetIndicatorClass;
    
    // Currently active target indicator
    UPROPERTY()
    UUserWidget* TargetIndicator;
    
    // Icon to show above locked targets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Target Lock")
    UTexture2D* LockOnIcon;
    
    // Find best target based on distance and direction
    AActor* FindBestTarget() const;
    
    // Get all valid targets in range
    TArray<AActor*> GetValidTargetsInRange() const;
    
    // Update camera when locked on
    void UpdateCameraForTargetLock(float DeltaTime);
    
    // Update target indicator position
    void UpdateTargetIndicator();
    
    // Determine if an actor is a valid target
    bool IsValidTarget(const AActor* PotentialTarget) const;
    
    // Networking
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

### Implementation Example

```cpp
// UTargetLockComponent.cpp
#include "TargetLockComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTargetLockComponent::UTargetLockComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void UTargetLockComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Find camera components on owner
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        // Try to find camera components
        PlayerCamera = OwnerCharacter->FindComponentByClass<UCameraComponent>();
        CameraBoom = OwnerCharacter->FindComponentByClass<USpringArmComponent>();
    }
}

void UTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // If we have a target but it's no longer valid, clear the lock
    if (IsLockActive() && !IsValidTarget(CurrentTarget))
    {
        ClearTargetLock();
        return;
    }
    
    // Update camera position when locked onto a target
    if (IsLockActive())
    {
        UpdateCameraForTargetLock(DeltaTime);
        UpdateTargetIndicator();
    }
}

void UTargetLockComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UTargetLockComponent, CurrentTarget);
}

void UTargetLockComponent::ToggleTargetLock()
{
    if (IsLockActive())
    {
        ClearTargetLock();
    }
    else
    {
        AActor* BestTarget = FindBestTarget();
        if (BestTarget)
        {
            LockOnTarget(BestTarget);
        }
    }
}

bool UTargetLockComponent::LockOnTarget(AActor* Target)
{
    if (!Target || !IsValidTarget(Target))
    {
        return false;
    }
    
    // Set the new target
    CurrentTarget = Target;
    
    // Create target indicator if it doesn't exist
    if (TargetIndicatorClass && !TargetIndicator && GetOwner()->IsLocallyControlled())
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            TargetIndicator = CreateWidget<UUserWidget>(PC, TargetIndicatorClass);
            if (TargetIndicator)
            {
                TargetIndicator->AddToViewport();
            }
        }
    }
    
    // Broadcast change event
    OnTargetLockChanged.Broadcast(CurrentTarget);
    
    return true;
}

void UTargetLockComponent::ClearTargetLock()
{
    // Remove target indicator
    if (TargetIndicator)
    {
        TargetIndicator->RemoveFromParent();
        TargetIndicator = nullptr;
    }
    
    AActor* OldTarget = CurrentTarget;
    CurrentTarget = nullptr;
    
    // Broadcast change event with nullptr
    OnTargetLockChanged.Broadcast(nullptr);
}

bool UTargetLockComponent::SwitchTarget(bool bSwitchToRightTarget)
{
    if (!IsLockActive())
    {
        return false;
    }
    
    TArray<AActor*> ValidTargets = GetValidTargetsInRange();
    if (ValidTargets.Num() <= 1)
    {
        return false;
    }
    
    // Get owner forward and right vectors
    FVector OwnerLocation = GetOwner()->GetActorLocation();
    FVector OwnerForward = GetOwner()->GetActorForwardVector();
    FVector OwnerRight = GetOwner()->GetActorRightVector();
    
    // Current target direction relative to player
    FVector CurrentTargetDir = (CurrentTarget->GetActorLocation() - OwnerLocation).GetSafeNormal();
    
    // Find best target to switch to
    AActor* BestSwitchTarget = nullptr;
    float BestScore = -1.0f;
    
    for (AActor* PotentialTarget : ValidTargets)
    {
        if (PotentialTarget == CurrentTarget)
        {
            continue;
        }
        
        FVector TargetDir = (PotentialTarget->GetActorLocation() - OwnerLocation).GetSafeNormal();
        
        // Calculate the angle from current target to potential target
        float ForwardDot = FVector::DotProduct(CurrentTargetDir, TargetDir);
        float RightDot = FVector::DotProduct(OwnerRight, TargetDir);
        
        // We want targets to the right if bSwitchToRightTarget is true, or to the left otherwise
        bool bIsRightSide = RightDot > 0.0f;
        if (bIsRightSide != bSwitchToRightTarget)
        {
            continue;
        }
        
        // Score based on how far to the side the target is
        // Higher right dot means more to the side
        float Score = 1.0f - ForwardDot;
        
        if (Score > BestScore)
        {
            BestScore = Score;
            BestSwitchTarget = PotentialTarget;
        }
    }
    
    if (BestSwitchTarget)
    {
        return LockOnTarget(BestSwitchTarget);
    }
    
    return false;
}

AActor* UTargetLockComponent::FindBestTarget() const
{
    TArray<AActor*> ValidTargets = GetValidTargetsInRange();
    if (ValidTargets.Num() == 0)
    {
        return nullptr;
    }
    
    FVector OwnerLocation = GetOwner()->GetActorLocation();
    FVector OwnerForward = GetOwner()->GetActorForwardVector();
    
    // Find target most directly in front of player
    AActor* BestTarget = nullptr;
    float BestDot = -1.0f;
    
    for (AActor* Target : ValidTargets)
    {
        FVector TargetDir = (Target->GetActorLocation() - OwnerLocation).GetSafeNormal();
        float DotProduct = FVector::DotProduct(OwnerForward, TargetDir);
        
        if (DotProduct > BestDot)
        {
            BestDot = DotProduct;
            BestTarget = Target;
        }
    }
    
    return BestTarget;
}

TArray<AActor*> UTargetLockComponent::GetValidTargetsInRange() const
{
    TArray<AActor*> Result;
    
    // Get all actors with the targetable tag
    TArray<AActor*> TargetableActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetableTag, TargetableActors);
    
    FVector OwnerLocation = GetOwner()->GetActorLocation();
    FVector OwnerForward = GetOwner()->GetActorForwardVector();
    
    // Filter by distance, FOV, and validity
    for (AActor* Actor : TargetableActors)
    {
        if (!IsValidTarget(Actor))
        {
            continue;
        }
        
        FVector TargetDir = (Actor->GetActorLocation() - OwnerLocation).GetSafeNormal();
        float DotProduct = FVector::DotProduct(OwnerForward, TargetDir);
        float AngleRad = FMath::Acos(DotProduct);
        float AngleDeg = FMath::RadiansToDegrees(AngleRad);
        
        // Check if within FOV
        if (AngleDeg <= TargetingFOV * 0.5f)
        {
            Result.Add(Actor);
        }
    }
    
    return Result;
}

bool UTargetLockComponent::IsValidTarget(const AActor* PotentialTarget) const
{
    if (!PotentialTarget)
    {
        return false;
    }
    
    // Ignore self
    if (PotentialTarget == GetOwner())
    {
        return false;
    }
    
    // Check for targetable tag
    if (!PotentialTarget->ActorHasTag(TargetableTag))
    {
        return false;
    }
    
    // Check if target is within max distance
    float Distance = FVector::Distance(GetOwner()->GetActorLocation(), PotentialTarget->GetActorLocation());
    if (Distance > MaxTargetingDistance)
    {
        return false;
    }
    
    // Check if target is alive/active - depends on your game's health system
    // Example (replace with your own implementation):
    /* 
    IHealthInterface* HealthInterface = Cast<IHealthInterface>(PotentialTarget);
    if (HealthInterface && HealthInterface->IsDead())
    {
        return false;
    }
    */
    
    // Line of sight check
    FHitResult HitResult;
    FVector Start = GetOwner()->GetActorLocation();
    FVector End = PotentialTarget->GetActorLocation();
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.AddIgnoredActor(PotentialTarget);
    
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);
    if (bHit)
    {
        // Something is blocking line of sight
        return false;
    }
    
    return true;
}

void UTargetLockComponent::UpdateCameraForTargetLock(float DeltaTime)
{
    if (!IsLockActive() || !PlayerCamera || !GetOwner()->IsLocallyControlled())
    {
        return;
    }
    
    FVector OwnerLocation = GetOwner()->GetActorLocation();
    FVector TargetLocation = CurrentTarget->GetActorLocation();
    
    // Calculate desired look direction
    FVector LookDirection = (TargetLocation - OwnerLocation).GetSafeNormal();
    FRotator DesiredRotation = LookDirection.Rotation();
    
    // Keep the character's Z rotation for the camera
    DesiredRotation.Roll = 0.0f;
    DesiredRotation.Pitch = FMath::Clamp(DesiredRotation.Pitch, -45.0f, 45.0f);
    
    // Get the controller
    AController* OwnerController = GetOwner()->GetInstigatorController();
    if (OwnerController)
    {
        APlayerController* PC = Cast<APlayerController>(OwnerController);
        if (PC)
        {
            // Interpolate to desired rotation
            FRotator CurrentControlRotation = PC->GetControlRotation();
            FRotator NewControlRotation = FMath::RInterpTo(
                CurrentControlRotation,
                DesiredRotation,
                DeltaTime,
                CameraInterpolationSpeed
            );
            
            PC->SetControlRotation(NewControlRotation);
        }
    }
}

void UTargetLockComponent::UpdateTargetIndicator()
{
    if (!IsLockActive() || !TargetIndicator || !GetOwner()->IsLocallyControlled())
    {
        return;
    }
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        return;
    }
    
    // Convert world location to screen location
    FVector2D ScreenLocation;
    bool bSuccess = PC->ProjectWorldLocationToScreen(
        CurrentTarget->GetActorLocation(),
        ScreenLocation,
        false
    );
    
    if (bSuccess)
    {
        // Update widget position
        // Note: This assumes the widget has a "SetPosition" function implemented in Blueprint
        // You'll need to create this function in your widget Blueprint
        static FName NAME_SetPosition = FName("SetPosition");
        FWidgetBlueprintGeneratedClass::FDynamicPropertyInfo PropertyInfo;
        PropertyInfo.SetFunctionName(NAME_SetPosition);
        PropertyInfo.AddParam(ScreenLocation);
        PropertyInfo.AddStructParam<FVector2D>();
        TargetIndicator->ProcessEvent(UWidgetBlueprintGeneratedClass::FindFunction(TargetIndicator->GetClass(), NAME_SetPosition), &PropertyInfo);
    }
}
```

## AI Prompt Template

```
I need to implement a target lock-on system for my third-person combat game in Unreal Engine 5.
Please provide the implementation for a UTargetLockComponent that:

1. Handles selecting and tracking enemies with the following features:
   - Toggle lock-on functionality
   - Automatic selection of the most appropriate target in view
   - Ability to switch between multiple valid targets
   - Support for target indicators and HUD elements

2. Implements camera control to keep the target centered during lock-on

3. Includes these core functions:
   - ToggleTargetLock: Enable/disable the lock-on system
   - LockOnTarget: Focus on a specific target
   - ClearTargetLock: Release the current target
   - SwitchTarget: Change to another nearby enemy
   - Target validation with distance and line-of-sight checks

4. Properly handles multiplayer replication

Please implement this in C++ following UE5 best practices, including proper performance considerations.
```

## Performance Considerations
- Consider using a timer rather than checking every tick for target validity
- Optimize line-of-sight checks by using simplified collision
- Cache valid targets and update the list periodically rather than scanning every frame
- Use distance-based update rates for target validation (check closer targets more frequently)
- Add frustum culling before performing line trace checks

## Integration Tips
- Connect with input system to handle lock-on toggle and target switching
- Integrate with combat system for target-relative attack directions
- Add animation adjustments to make the character face locked targets
- Implement different camera behaviors based on weapon types
- Work with UI system to display target indicators and health bars

## Example Usage

```cpp
// In your character class
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Bind target lock actions
    PlayerInputComponent->BindAction("TargetLock", IE_Pressed, this, &APlayerCharacter::ToggleTargetLock);
    PlayerInputComponent->BindAction("NextTarget", IE_Pressed, this, &APlayerCharacter::SwitchToNextTarget);
    PlayerInputComponent->BindAction("PreviousTarget", IE_Pressed, this, &APlayerCharacter::SwitchToPreviousTarget);
}

void APlayerCharacter::ToggleTargetLock()
{
    if (UTargetLockComponent* TargetComp = FindComponentByClass<UTargetLockComponent>())
    {
        TargetComp->ToggleTargetLock();
        
        // Adjust character movement based on lock state
        UCharacterMovementComponent* MoveComp = GetCharacterMovement();
        if (MoveComp)
        {
            if (TargetComp->IsLockActive())
            {
                // While locked, orient rotation to movement direction
                MoveComp->bOrientRotationToMovement = false;
                
                // Enable strafing behavior
                bUseControllerRotationYaw = true;
            }
            else
            {
                // When not locked, return to normal movement
                MoveComp->bOrientRotationToMovement = true;
                bUseControllerRotationYaw = false;
            }
        }
    }
}

void APlayerCharacter::SwitchToNextTarget()
{
    if (UTargetLockComponent* TargetComp = FindComponentByClass<UTargetLockComponent>())
    {
        if (TargetComp->IsLockActive())
        {
            TargetComp->SwitchTarget(true); // Switch to right
        }
    }
}

void APlayerCharacter::SwitchToPreviousTarget()
{
    if (UTargetLockComponent* TargetComp = FindComponentByClass<UTargetLockComponent>())
    {
        if (TargetComp->IsLockActive())
        {
            TargetComp->SwitchTarget(false); // Switch to left
        }
    }
}

// Adjust attack behavior based on target
void APlayerCharacter::PerformAttack()
{
    if (UTargetLockComponent* TargetComp = FindComponentByClass<UTargetLockComponent>())
    {
        if (TargetComp->IsLockActive())
        {
            // Get direction to target
            AActor* Target = TargetComp->GetCurrentTarget();
            FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            
            // Face the target for the attack
            SetActorRotation(DirectionToTarget.Rotation());
            
            // Perform targeted attack animation
            PlayAnimMontage(TargetedAttackMontage);
        }
        else
        {
            // Perform regular attack in forward direction
            PlayAnimMontage(RegularAttackMontage);
        }
    }
}
``` 