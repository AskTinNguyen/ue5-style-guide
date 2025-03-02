# Movement and Animation Fundamentals

## Overview

This guide covers implementing smooth character movement and animations for your hack and slash game. We'll set up a robust animation system that handles basic locomotion, combat stances, and transitions between different states.

## Prerequisites

Before starting this guide, you should:
- Have completed [Camera System Setup](03_Camera_System_Setup.md)
- Understand basic UE5 animation concepts
- Have a character mesh with an animation skeleton

## Basic Movement Setup

### 1. Movement Component Configuration

```cpp
// HackAndSlashCharacter.h
protected:
    // Movement states
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    bool bIsSprinting;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    bool bIsRolling;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    bool bIsInCombatStance;
    
    // Movement settings
    UPROPERTY(EditAnywhere, Category = Movement)
    float WalkSpeed;
    
    UPROPERTY(EditAnywhere, Category = Movement)
    float SprintSpeed;
    
    UPROPERTY(EditAnywhere, Category = Movement)
    float RollDistance;
    
    UPROPERTY(EditAnywhere, Category = Movement)
    float RollDuration;
```

### 2. Initialize Movement Values

```cpp
// HackAndSlashCharacter.cpp
AHackAndSlashCharacter::AHackAndSlashCharacter()
{
    // Set default values
    WalkSpeed = 600.0f;
    SprintSpeed = 1000.0f;
    RollDistance = 500.0f;
    RollDuration = 0.5f;
    
    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}
```

## Animation System Setup

### 1. Create Animation Instance

```cpp
// HackAndSlashAnimInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HackAndSlashAnimInstance.generated.h"

UCLASS()
class MYGAME_API UHackAndSlashAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
    
public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    
protected:
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    float Speed;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    float Direction;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    bool bIsInAir;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    bool bIsSprinting;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    bool bIsInCombatStance;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    float AimPitch;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    float AimYaw;
    
private:
    UPROPERTY()
    class AHackAndSlashCharacter* Character;
};
```

### 2. Implement Animation Updates

```cpp
// HackAndSlashAnimInstance.cpp
void UHackAndSlashAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    Character = Cast<AHackAndSlashCharacter>(TryGetPawnOwner());
}

void UHackAndSlashAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    
    if (!Character)
    {
        Character = Cast<AHackAndSlashCharacter>(TryGetPawnOwner());
        return;
    }
    
    // Update movement values
    FVector Velocity = Character->GetVelocity();
    Speed = Velocity.Size();
    
    // Calculate movement direction relative to character rotation
    FRotator Rotation = Character->GetActorRotation();
    Direction = CalculateDirection(Velocity, Rotation);
    
    // Update states
    bIsInAir = Character->GetCharacterMovement()->IsFalling();
    bIsSprinting = Character->IsSprinting();
    bIsInCombatStance = Character->IsInCombatStance();
    
    // Update aim values
    if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
    {
        FRotator ControlRotation = PC->GetControlRotation();
        AimPitch = ControlRotation.Pitch;
        AimYaw = ControlRotation.Yaw;
    }
}
```

## Advanced Movement Features

### 1. Combat Movement System

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::SetCombatStance(bool bEnabled)
{
    bIsInCombatStance = bEnabled;
    
    // Adjust movement settings for combat
    if (bEnabled)
    {
        GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * 0.8f;
        GetCharacterMovement()->bOrientRotationToMovement = false;
        bUseControllerRotationYaw = true;
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        bUseControllerRotationYaw = false;
    }
}

void AHackAndSlashCharacter::PerformRoll()
{
    if (bIsRolling || GetCharacterMovement()->IsFalling())
        return;
    
    // Get roll direction
    FVector RollDirection = GetLastMovementInputVector();
    if (RollDirection.IsNearlyZero())
    {
        RollDirection = GetActorForwardVector();
    }
    
    // Start roll
    bIsRolling = true;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    
    // Launch character
    LaunchCharacter(RollDirection * RollDistance, true, false);
    
    // Set timer to end roll
    FTimerHandle RollTimer;
    GetWorldTimerManager().SetTimer(RollTimer, this, &AHackAndSlashCharacter::EndRoll, RollDuration, false);
}

void AHackAndSlashCharacter::EndRoll()
{
    bIsRolling = false;
    GetCharacterMovement()->bOrientRotationToMovement = !bIsInCombatStance;
}
```

### 2. Root Motion Handling

```cpp
// HackAndSlashCharacter.h
protected:
    virtual void OnRep_ReplicatedMovement() override;
    virtual void OnRep_ReplicatedBasedMovement() override;
    
    UPROPERTY(EditDefaultsOnly, Category = Animation)
    class UAnimMontage* RollMontage;
    
    UPROPERTY(EditDefaultsOnly, Category = Animation)
    class UAnimMontage* DodgeMontage;
```

## Animation Montage System

### 1. Create Animation Montages

Set up the following montages in the UE5 editor:
- Locomotion blend space
- Combat stance transitions
- Roll/dodge animations
- Attack animations (basic foundation)

### 2. Implement Montage Playback

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::PlayRollAnimation()
{
    if (RollMontage)
    {
        PlayAnimMontage(RollMontage);
    }
}

void AHackAndSlashCharacter::PlayDodgeAnimation()
{
    if (DodgeMontage)
    {
        PlayAnimMontage(DodgeMontage);
    }
}

// Animation notify handlers
void AHackAndSlashCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == RollMontage)
    {
        EndRoll();
    }
}
```

## State Machine Integration

### 1. Create Character States

```cpp
// HackAndSlashCharacter.h
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    Idle,
    Walking,
    Sprinting,
    Rolling,
    Attacking,
    Stunned
};

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CharacterState)
    ECharacterState CurrentState;
    
    void SetCharacterState(ECharacterState NewState);
```

### 2. Implement State Management

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::SetCharacterState(ECharacterState NewState)
{
    if (CurrentState == NewState)
        return;
    
    PreviousState = CurrentState;
    CurrentState = NewState;
    
    // Handle state-specific logic
    switch (CurrentState)
    {
        case ECharacterState::Sprinting:
            GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
            break;
            
        case ECharacterState::Rolling:
            PlayRollAnimation();
            break;
            
        case ECharacterState::Attacking:
            // Will be implemented in combat system
            break;
            
        default:
            GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
            break;
    }
}
```

## Movement Quality Improvements

### 1. Smooth Rotation

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bIsInCombatStance)
    {
        // Smooth rotation towards movement direction
        FRotator TargetRotation = GetControlRotation();
        FRotator CurrentRotation = GetActorRotation();
        
        FRotator NewRotation = FMath::RInterpTo(
            CurrentRotation,
            TargetRotation,
            DeltaTime,
            10.0f
        );
        
        SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
    }
}
```

### 2. Movement Acceleration

```cpp
void AHackAndSlashCharacter::ConfigureMovementAcceleration()
{
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    
    MovementComp->MaxAcceleration = 2048.0f;
    MovementComp->BrakingDecelerationWalking = 2048.0f;
    MovementComp->GroundFriction = 8.0f;
    
    // Air control
    MovementComp->AirControl = 0.35f;
    MovementComp->AirControlBoostMultiplier = 2.0f;
    MovementComp->FallingLateralFriction = 0.15f;
}
```

## Common Issues and Solutions

### Animation Desyncs

1. Ensure proper replication of movement and states
2. Validate animation notify events
3. Check montage sync groups

### Movement Stuttering

1. Adjust acceleration and deceleration values
2. Verify network update frequency
3. Check for frame rate dependencies

### State Transition Issues

1. Implement proper state validation
2. Add transition guards
3. Handle interrupted states correctly

## Next Steps

Now that you have a solid movement and animation system, you can:

1. Set up game modes in [Game Mode and States](05_Game_Mode_States.md)
2. Implement combat mechanics in [Basic Attack System](../03_CombatSystems/01_Basic_Attack_System.md)
3. Add advanced movement abilities in [Advanced Movement](../04_AdvancedMechanics/01_Advanced_Movement.md) 