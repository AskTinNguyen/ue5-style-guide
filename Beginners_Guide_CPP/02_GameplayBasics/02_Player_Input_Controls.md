# Player Input and Controls

## Overview

This guide expands on basic character controls by implementing advanced input handling and control systems for your hack and slash game. We'll use UE5's Enhanced Input System for more flexible and powerful input management.

## Prerequisites

Before starting this guide, you should:
- Have completed [Creating Your First Character](01_Creating_First_Character.md)
- Understand basic input binding in UE5
- Have the Enhanced Input plugin enabled in your project

## Setting Up Enhanced Input

### 1. Enable the Plugin

1. Open Project Settings
2. Search for "Enhanced Input"
3. Enable the Enhanced Input Plugin
4. Restart the editor when prompted

### 2. Create Input Action and Context Assets

Create the following assets in your Content Browser:

```
Content/
└── Input/
    ├── IA_Movement.uasset
    ├── IA_Look.uasset
    ├── IA_Jump.uasset
    ├── IA_Attack.uasset
    └── IMC_Default.uasset
```

### 3. Update Character Header

```cpp
// HackAndSlashCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "HackAndSlashCharacter.generated.h"

UCLASS()
class MYGAME_API AHackAndSlashCharacter : public ACharacter
{
    GENERATED_BODY()
    
protected:
    // Enhanced Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    class UInputAction* MovementAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    class UInputAction* LookAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    class UInputAction* JumpAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    class UInputAction* AttackAction;
    
    // Input Mapping Context
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    class UInputMappingContext* DefaultMappingContext;
    
    // Input functions
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump(const FInputActionValue& Value);
    void StopJump(const FInputActionValue& Value);
    void StartAttack(const FInputActionValue& Value);
    void StopAttack(const FInputActionValue& Value);
    
    // Advanced movement variables
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float SprintSpeedMultiplier;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float DodgeForce;
    
    // Advanced movement functions
    void StartSprint();
    void StopSprint();
    void Dodge();
};
```

### 4. Implement Enhanced Input

```cpp
// HackAndSlashCharacter.cpp
#include "HackAndSlashCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AHackAndSlashCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AHackAndSlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Bind movement
        EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AHackAndSlashCharacter::Move);
        
        // Bind looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHackAndSlashCharacter::Look);
        
        // Bind jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHackAndSlashCharacter::StartJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHackAndSlashCharacter::StopJump);
        
        // Bind attacking
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AHackAndSlashCharacter::StartAttack);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AHackAndSlashCharacter::StopAttack);
    }
}
```

### 5. Implement Input Functions

```cpp
void AHackAndSlashCharacter::Move(const FInputActionValue& Value)
{
    // Input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();
    
    if (Controller != nullptr)
    {
        // Find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        
        // Get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        
        // Get right vector
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        
        // Add movement
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AHackAndSlashCharacter::Look(const FInputActionValue& Value)
{
    // Input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}
```

## Advanced Movement Features

### 1. Sprint System

```cpp
// Add to HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::StartSprint()
{
    GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier;
}

void AHackAndSlashCharacter::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed /= SprintSpeedMultiplier;
}
```

### 2. Dodge System

```cpp
void AHackAndSlashCharacter::Dodge()
{
    if (GetCharacterMovement()->IsFalling())
        return;
    
    // Get movement direction
    FVector MovementDirection = GetLastMovementInputVector();
    
    // If no input, dodge backwards
    if (MovementDirection.IsNearlyZero())
    {
        MovementDirection = -GetActorForwardVector();
    }
    
    // Normalize and apply dodge force
    MovementDirection.Normalize();
    LaunchCharacter(MovementDirection * DodgeForce, true, false);
    
    // Optional: Play dodge animation
    // PlayAnimMontage(DodgeMontage);
}
```

## Input Action Setup

### 1. Movement Action

Configure the Movement Input Action:
- Type: Input Action (Vector2D)
- Triggers: Any
- Modifiers: None
- Expected Controls: WASD, Gamepad Left Stick

### 2. Look Action

Configure the Look Input Action:
- Type: Input Action (Vector2D)
- Triggers: Any
- Modifiers: None
- Expected Controls: Mouse XY, Gamepad Right Stick

### 3. Combat Actions

Configure the Attack Input Action:
- Type: Input Action (Button)
- Triggers: Started, Completed
- Modifiers: None
- Expected Controls: Left Mouse Button, Gamepad Face Button

## Advanced Input Features

### 1. Input Action Modifiers

```cpp
// Example of using modifiers for smooth camera movement
void AHackAndSlashCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    
    // Apply custom sensitivity curve
    float AdjustedYaw = ApplySensitivityCurve(LookAxisVector.X);
    float AdjustedPitch = ApplySensitivityCurve(LookAxisVector.Y);
    
    AddControllerYawInput(AdjustedYaw);
    AddControllerPitchInput(AdjustedPitch);
}

float AHackAndSlashCharacter::ApplySensitivityCurve(float RawInput)
{
    // Example of a custom sensitivity curve
    const float DeadZone = 0.1f;
    const float Exponent = 2.0f;
    
    if (FMath::Abs(RawInput) < DeadZone)
        return 0.0f;
    
    return FMath::Sign(RawInput) * FMath::Pow(FMath::Abs(RawInput), Exponent);
}
```

### 2. Context-Sensitive Actions

```cpp
// Example of context-sensitive input handling
void AHackAndSlashCharacter::InteractWithEnvironment(const FInputActionValue& Value)
{
    // Raycast to find interactive objects
    FHitResult HitResult;
    FVector Start = FollowCamera->GetComponentLocation();
    FVector End = Start + FollowCamera->GetForwardVector() * InteractionRange;
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
    {
        // Check for different types of interactive objects
        if (IInteractable* Interactable = Cast<IInteractable>(HitResult.GetActor()))
        {
            Interactable->Interact(this);
        }
    }
}
```

## Common Issues and Solutions

### Input Not Working

1. Check Enhanced Input Plugin is enabled
2. Verify Input Mapping Context is properly set up
3. Ensure Input Actions are correctly configured
4. Debug input bindings in the Enhanced Input Debug menu

### Movement Issues

1. Verify Character Movement Component settings
2. Check for conflicting movement modifications
3. Debug movement vectors with visual helpers

### Camera Control Problems

1. Verify camera boom setup
2. Check rotation constraints
3. Debug input values with print strings

## Next Steps

Now that you have advanced input controls, you can:

1. Set up the camera system in [Camera System Setup](03_Camera_System_Setup.md)
2. Add movement animations in [Movement and Animation Fundamentals](04_Movement_Animation_Basics.md)
3. Implement combat mechanics in [Basic Attack System](../03_CombatSystems/01_Basic_Attack_System.md)
