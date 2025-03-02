# Camera System Setup

## Overview

This guide covers setting up a robust camera system for your hack and slash game. We'll implement features like smooth camera following, collision avoidance, and combat-focused camera behaviors.

## Prerequisites

Before starting this guide, you should:
- Have completed [Player Input and Controls](02_Player_Input_Controls.md)
- Understand basic camera components in UE5
- Be familiar with spring arm functionality

## Basic Camera Setup

### 1. Camera Components

```cpp
// HackAndSlashCharacter.h
protected:
    // Camera boom positioning the camera behind the character
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class USpringArmComponent* CameraBoom;
    
    // Follow camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* FollowCamera;
    
    // Camera settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    float BaseCameraDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    float CombatCameraDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    FVector CameraOffset;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    float CameraLagSpeed;
```

### 2. Initialize Camera Components

```cpp
// HackAndSlashCharacter.cpp
AHackAndSlashCharacter::AHackAndSlashCharacter()
{
    // Create camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = BaseCameraDistance;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SocketOffset = CameraOffset;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = CameraLagSpeed;
    
    // Create follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}
```

## Advanced Camera Features

### 1. Camera Manager Class

```cpp
// HackAndSlashCameraManager.h
#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "HackAndSlashCameraManager.generated.h"

UCLASS()
class MYGAME_API AHackAndSlashCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()
    
public:
    AHackAndSlashCameraManager();
    
    // Camera modes
    void SetCombatMode(bool bEnabled);
    void FocusOnTarget(AActor* Target);
    void ResetCamera();
    
protected:
    virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
    
    UPROPERTY()
    AActor* CurrentTarget;
    
    UPROPERTY(EditAnywhere, Category = Camera)
    float CombatFOV;
    
    UPROPERTY(EditAnywhere, Category = Camera)
    float NormalFOV;
    
    UPROPERTY(EditAnywhere, Category = Camera)
    float FOVTransitionSpeed;
    
    bool bIsInCombatMode;
};
```

### 2. Implement Camera Manager

```cpp
// HackAndSlashCameraManager.cpp
#include "HackAndSlashCameraManager.h"

AHackAndSlashCameraManager::AHackAndSlashCameraManager()
{
    CombatFOV = 90.0f;
    NormalFOV = 75.0f;
    FOVTransitionSpeed = 5.0f;
    bIsInCombatMode = false;
}

void AHackAndSlashCameraManager::SetCombatMode(bool bEnabled)
{
    bIsInCombatMode = bEnabled;
    
    // Start FOV transition
    if (bEnabled)
    {
        SetFOV(CombatFOV);
    }
    else
    {
        SetFOV(NormalFOV);
    }
}

void AHackAndSlashCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    Super::UpdateViewTarget(OutVT, DeltaTime);
    
    // Handle target focusing
    if (CurrentTarget)
    {
        FVector TargetLocation = CurrentTarget->GetActorLocation();
        FVector CameraLocation = OutVT.POV.Location;
        
        // Calculate ideal look-at rotation
        FRotator TargetRotation = (TargetLocation - CameraLocation).Rotation();
        
        // Smoothly interpolate to target rotation
        FRotator NewRotation = FMath::RInterpTo(
            OutVT.POV.Rotation,
            TargetRotation,
            DeltaTime,
            5.0f
        );
        
        OutVT.POV.Rotation = NewRotation;
    }
}

void AHackAndSlashCameraManager::FocusOnTarget(AActor* Target)
{
    CurrentTarget = Target;
}

void AHackAndSlashCameraManager::ResetCamera()
{
    CurrentTarget = nullptr;
}
```

## Combat Camera Features

### 1. Lock-On System

```cpp
// HackAndSlashCharacter.h
protected:
    UPROPERTY()
    AActor* LockedTarget;
    
    UPROPERTY(EditAnywhere, Category = Combat)
    float LockOnDistance;
    
    void ToggleLockOn();
    AActor* FindBestLockOnTarget();
```

### 2. Implement Lock-On

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::ToggleLockOn()
{
    if (LockedTarget)
    {
        // Disable lock-on
        LockedTarget = nullptr;
        if (AHackAndSlashCameraManager* CamManager = Cast<AHackAndSlashCameraManager>(Controller->PlayerCameraManager))
        {
            CamManager->ResetCamera();
        }
    }
    else
    {
        // Find and set new target
        LockedTarget = FindBestLockOnTarget();
        if (LockedTarget)
        {
            if (AHackAndSlashCameraManager* CamManager = Cast<AHackAndSlashCameraManager>(Controller->PlayerCameraManager))
            {
                CamManager->FocusOnTarget(LockedTarget);
            }
        }
    }
}

AActor* AHackAndSlashCharacter::FindBestLockOnTarget()
{
    // Get all potential targets
    TArray<AActor*> PotentialTargets;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UTargetableInterface::StaticClass(), PotentialTargets);
    
    AActor* BestTarget = nullptr;
    float ClosestDistance = LockOnDistance;
    
    for (AActor* Target : PotentialTargets)
    {
        float Distance = FVector::Distance(GetActorLocation(), Target->GetActorLocation());
        
        // Check if target is in front of the player
        FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
        ToTarget.Normalize();
        float DotProduct = FVector::DotProduct(GetActorForwardVector(), ToTarget);
        
        if (Distance < ClosestDistance && DotProduct > 0.0f)
        {
            ClosestDistance = Distance;
            BestTarget = Target;
        }
    }
    
    return BestTarget;
}
```

## Camera Shake and Effects

### 1. Create Camera Shake Class

```cpp
// HackAndSlashCameraShake.h
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "HackAndSlashCameraShake.generated.h"

UCLASS()
class MYGAME_API UHackAndSlashCameraShake : public UCameraShakeBase
{
    GENERATED_BODY()
    
public:
    UHackAndSlashCameraShake();
};
```

### 2. Implement Camera Shake

```cpp
// HackAndSlashCameraShake.cpp
#include "HackAndSlashCameraShake.h"

UHackAndSlashCameraShake::UHackAndSlashCameraShake()
{
    OscillationDuration = 0.25f;
    OscillationBlendInTime = 0.05f;
    OscillationBlendOutTime = 0.05f;
    
    RotOscillation.Pitch.Amplitude = 2.0f;
    RotOscillation.Pitch.Frequency = 10.0f;
    
    RotOscillation.Yaw.Amplitude = 2.0f;
    RotOscillation.Yaw.Frequency = 10.0f;
}
```

### 3. Apply Camera Effects

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::OnHit()
{
    // Play camera shake on hit
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->ClientStartCameraShake(UHackAndSlashCameraShake::StaticClass());
    }
}
```

## Camera Collision Handling

### 1. Advanced Spring Arm Settings

```cpp
// HackAndSlashCharacter.cpp
void AHackAndSlashCharacter::SetupCameraCollision()
{
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->ProbeSize = 12.0f;
    CameraBoom->ProbeChannel = ECollisionChannel::ECC_Camera;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->bEnableCameraRotationLag = true;
    CameraBoom->CameraLagSpeed = 10.0f;
    CameraBoom->CameraRotationLagSpeed = 10.0f;
    CameraBoom->CameraLagMaxDistance = 100.0f;
}
```

### 2. Custom Collision Handling

```cpp
void AHackAndSlashCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Handle camera obstruction
    FVector Start = GetActorLocation();
    FVector End = FollowCamera->GetComponentLocation();
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Camera, QueryParams))
    {
        // Camera is obstructed, adjust spring arm length
        float NewLength = (HitResult.Location - Start).Size();
        CameraBoom->TargetArmLength = FMath::Min(NewLength, BaseCameraDistance);
    }
    else
    {
        // No obstruction, return to normal distance
        CameraBoom->TargetArmLength = FMath::FInterpTo(
            CameraBoom->TargetArmLength,
            BaseCameraDistance,
            DeltaTime,
            5.0f
        );
    }
}
```

## Common Issues and Solutions

### Camera Clipping

1. Adjust ProbeSize and collision settings
2. Implement smooth camera position interpolation
3. Use proper collision channels

### Jerky Camera Movement

1. Enable camera lag
2. Adjust lag speed and max distance
3. Implement smooth rotation interpolation

### Lock-On Issues

1. Check target visibility
2. Implement proper target switching
3. Add distance and angle checks

## Next Steps

Now that you have a robust camera system, you can:

1. Add movement animations in [Movement and Animation Fundamentals](04_Movement_Animation_Basics.md)
2. Set up game modes in [Game Mode and States](05_Game_Mode_States.md)
3. Implement combat mechanics in [Basic Attack System](../03_CombatSystems/01_Basic_Attack_System.md)

