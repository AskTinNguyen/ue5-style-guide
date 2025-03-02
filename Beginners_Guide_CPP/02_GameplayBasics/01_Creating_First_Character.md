# Creating Your First Character

## Overview

This guide walks you through creating a basic character for your hack and slash game in Unreal Engine 5 using C++. We'll create a character with basic movement, camera control, and the foundation for combat mechanics.

## Prerequisites

Before starting this guide, you should:
- Have completed all guides in the [Fundamentals](../01_Fundamentals/) section
- Have a new or existing UE5 C++ project ready
- Understand basic UE5 class hierarchy

## Creating the Character Class

### 1. Base Character Setup

First, create a new C++ character class:

```cpp
// HackAndSlashCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HackAndSlashCharacter.generated.h"

UCLASS()
class MYGAME_API AHackAndSlashCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    AHackAndSlashCharacter();
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Camera boom to handle collision
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class USpringArmComponent* CameraBoom;
    
    // Follow camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* FollowCamera;
    
    // Movement functions
    void MoveForward(float Value);
    void MoveRight(float Value);
    
    // Camera control
    void LookUp(float Value);
    void Turn(float Value);
};
```

### 2. Component Setup

Implement the constructor to set up components:

```cpp
// HackAndSlashCharacter.cpp
#include "HackAndSlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AHackAndSlashCharacter::AHackAndSlashCharacter()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;
    
    // Create camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;
    
    // Create follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    
    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 500.0f;
    GetCharacterMovement()->AirControl = 0.2f;
    
    // Don't rotate when the controller rotates
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}
```

### 3. Input Setup

Add input handling functions:

```cpp
void AHackAndSlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Bind movement functions
    PlayerInputComponent->BindAxis("MoveForward", this, &AHackAndSlashCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AHackAndSlashCharacter::MoveRight);
    
    // Bind camera control
    PlayerInputComponent->BindAxis("LookUp", this, &AHackAndSlashCharacter::LookUp);
    PlayerInputComponent->BindAxis("Turn", this, &AHackAndSlashCharacter::Turn);
    
    // Bind action functions
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void AHackAndSlashCharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // Find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        
        // Add movement in that direction
        AddMovementInput(Direction, Value);
    }
}

void AHackAndSlashCharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // Find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        
        // Add movement in that direction
        AddMovementInput(Direction, Value);
    }
}

void AHackAndSlashCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AHackAndSlashCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}
```

## Setting Up Input Mappings

1. Open Project Settings > Input
2. Add the following Axis Mappings:

| Action Name | Scale | Key |
|------------|-------|-----|
| MoveForward | 1.0 | W |
| MoveForward | -1.0 | S |
| MoveRight | 1.0 | D |
| MoveRight | -1.0 | A |
| LookUp | -1.0 | Mouse Y |
| Turn | 1.0 | Mouse X |

3. Add Action Mappings:

| Action Name | Key |
|------------|-----|
| Jump | Space Bar |

## Adding Basic Animation Support

### 1. Create Animation Variables

```cpp
// HackAndSlashCharacter.h
UPROPERTY(BlueprintReadOnly, Category = Movement)
float MovementSpeed;

UPROPERTY(BlueprintReadOnly, Category = Movement)
bool bIsInAir;

// Update these variables
virtual void Tick(float DeltaTime) override;
```

### 2. Implement Animation Updates

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update movement speed
    MovementSpeed = GetVelocity().Size();
    
    // Update air state
    bIsInAir = GetCharacterMovement()->IsFalling();
}
```

## Testing Your Character

1. Compile your code
2. In the editor, create a new Blueprint class based on your C++ character
3. Set this Blueprint as the default pawn in your GameMode
4. Add a skeletal mesh to your character Blueprint
5. Create a basic Animation Blueprint with:
   - Idle animation when standing still
   - Walk/run animations based on MovementSpeed
   - Jump animation when bIsInAir is true

## Adding Combat Foundations

### 1. Add Combat Variables

```cpp
// HackAndSlashCharacter.h
protected:
    // Combat state
    UPROPERTY(BlueprintReadOnly, Category = Combat)
    bool bIsAttacking;
    
    // Basic combat stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
    float BaseDamage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
    float AttackSpeed;
    
    // Combat functions
    UFUNCTION(BlueprintCallable, Category = Combat)
    virtual void StartAttack();
    
    UFUNCTION(BlueprintCallable, Category = Combat)
    virtual void EndAttack();
```

### 2. Implement Basic Combat

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::StartAttack()
{
    if (!bIsAttacking)
    {
        bIsAttacking = true;
        
        // Log for debugging
        UE_LOG(LogTemp, Display, TEXT("Attack Started"));
        
        // You'll implement actual attack logic in the next guide
    }
}

void AHackAndSlashCharacter::EndAttack()
{
    bIsAttacking = false;
    
    // Log for debugging
    UE_LOG(LogTemp, Display, TEXT("Attack Ended"));
}
```

## Common Issues and Solutions

### Character Won't Move

1. Check input mappings in Project Settings
2. Verify the GameMode is using your character class
3. Ensure movement component is properly configured

### Camera Issues

1. Verify SpringArm and Camera component setup
2. Check camera rotation settings
3. Confirm input bindings for look controls

### Animation Problems

1. Make sure skeletal mesh is properly set up
2. Verify Animation Blueprint references
3. Check animation variable updates in Tick function

## Next Steps

Now that you have a basic character set up, you can:

1. Add more advanced movement mechanics in [Player Input and Controls](02_Player_Input_Controls.md)
2. Set up the camera system in [Camera System Setup](03_Camera_System_Setup.md)
3. Implement combat mechanics in [Basic Attack System](../03_CombatSystems/01_Basic_Attack_System.md)