# Basic Attack System in C++

## Overview

This guide will walk you through implementing a basic melee attack system in C++ for a hack and slash game in Unreal Engine 5. We'll cover the fundamental components needed for a responsive and satisfying attack system.

## Prerequisites

Before you begin this guide, you should:
- Have completed the [Creating Your First Character](../02_GameplayBasics/01_Creating_First_Character.md) guide
- Understand basic C++ concepts from [C++ Basics for Unreal Engine](../01_Fundamentals/02_CPP_Basics_For_UE5.md)
- Have your development environment set up as described in [Setting Up Your Development Environment](../01_Fundamentals/01_Setting_Up_Environment.md)

## Core Concepts

A basic attack system in a hack and slash game typically involves:

1. **Input Handling** - Detecting when the player presses the attack button
2. **Animation Triggering** - Playing the appropriate attack animation
3. **Hit Detection** - Determining when and what the attack hits
4. **Damage Application** - Applying damage to hit targets
5. **Visual and Audio Feedback** - Providing satisfying feedback to the player

## Implementation Steps

### Step 1: Setting Up the Character Class

First, let's modify our character class to include attack functionality:

```cpp
// YourCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "YourCharacter.generated.h"

UCLASS()
class YOURGAME_API AYourCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    AYourCharacter();
    
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    // Attack function that will be called when the attack button is pressed
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Attack();
    
    // Function to end the attack (called from animation notify)
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EndAttack();
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Flag to track if character is currently attacking
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsAttacking;
    
    // Attack montage to play
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    class UAnimMontage* AttackMontage;
};
```

### Step 2: Implementing the Attack Logic

Now, let's implement the attack functions in the CPP file:

```cpp
// YourCharacter.cpp
#include "YourCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/PlayerController.h"

AYourCharacter::AYourCharacter()
{
    // Set default values
    bIsAttacking = false;
}

void AYourCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AYourCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Bind the attack action to the Attack function
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AYourCharacter::Attack);
}

void AYourCharacter::Attack()
{
    // Check if we're already attacking
    if (bIsAttacking)
        return;
    
    // Check if we have a valid attack montage
    if (AttackMontage)
    {
        // Get the animation instance
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            // Set attacking flag to true
            bIsAttacking = true;
            
            // Play the attack montage
            float PlayRate = 1.0f;
            AnimInstance->Montage_Play(AttackMontage, PlayRate);
            
            // Log for debugging
            UE_LOG(LogTemp, Display, TEXT("Attack started"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttackMontage is not set!"));
    }
}

void AYourCharacter::EndAttack()
{
    // Reset the attacking flag
    bIsAttacking = false;
    
    // Log for debugging
    UE_LOG(LogTemp, Display, TEXT("Attack ended"));
}
```

### Step 3: Creating an Animation Notify for Hit Detection

We'll use Animation Notifies to determine when the weapon should check for hits:

1. In the Unreal Editor, open your attack animation
2. Right-click in the timeline where the weapon would make contact
3. Select "Add Notify" > "New Notify..."
4. Name it "AN_WeaponHit"

Now create a C++ class for the Notify:

```cpp
// WeaponHitNotify.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "WeaponHitNotify.generated.h"

UCLASS()
class YOURGAME_API UWeaponHitNotify : public UAnimNotify
{
    GENERATED_BODY()
    
public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
```

```cpp
// WeaponHitNotify.cpp
#include "WeaponHitNotify.h"
#include "YourCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

void UWeaponHitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    // Call parent implementation
    Super::Notify(MeshComp, Animation);
    
    if (!MeshComp || !MeshComp->GetOwner())
        return;
    
    // Try to cast the owner to your character class
    AYourCharacter* Character = Cast<AYourCharacter>(MeshComp->GetOwner());
    if (!Character)
        return;
    
    // Define the trace start and end locations
    FVector SocketLocation = MeshComp->GetSocketLocation("WeaponTip");
    FVector TraceStart = MeshComp->GetSocketLocation("WeaponRoot");
    FVector TraceEnd = SocketLocation;
    
    // Prepare collision parameters
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Character); // Ignore the character performing the attack
    
    // Array to store hit results
    TArray<FHitResult> HitResults;
    
    // Perform the sweep trace
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        TraceStart,
        TraceEnd,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(50.0f), // Adjust radius as needed
        QueryParams
    );
    
    // Debug visualization
    #if WITH_EDITOR
    DrawDebugSphere(GetWorld(), TraceEnd, 50.0f, 8, bHit ? FColor::Green : FColor::Red, false, 3.0f);
    DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 3.0f);
    #endif
    
    // Process the hits
    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            // Make sure we hit an actor
            if (Hit.GetActor())
            {
                UE_LOG(LogTemp, Display, TEXT("Hit actor: %s"), *Hit.GetActor()->GetName());
                
                // Apply damage to the hit actor
                // You'll implement this in the next guide
            }
        }
    }
}
```

### Step 4: Setting Up Input Mapping

1. Open your project settings
2. Navigate to Engine > Input
3. Add a new Action Mapping:
   - Name: "Attack"
   - Key: Left Mouse Button (or your preferred input)

### Step 5: Setting Up the Animation Blueprint

1. Open your character's Animation Blueprint
2. Create a new boolean variable named "IsAttacking"
3. In the AnimGraph, add logic to play the attack animation when IsAttacking is true
4. In the Event Graph, update IsAttacking based on the bIsAttacking value from your character

## Testing Your Implementation

To test your attack system:

1. Ensure your character has a weapon model attached (or use debug visualization)
2. Make sure the weapon has the appropriate sockets ("WeaponRoot" and "WeaponTip")
3. Play the game in the editor
4. Press the attack button (Left Mouse Button by default)
5. Verify that:
   - The attack animation plays
   - Debug traces appear (if enabled)
   - Log messages appear in the Output Log

## Common Issues and Solutions

### Attack Animation Doesn't Play

**Problem**: Pressing the attack button doesn't trigger the animation.  
**Solution**: 
- Ensure the AttackMontage is set in the editor
- Check if the input binding is correctly set up
- Verify the animation blueprint is properly connected

### Hit Detection Not Working

**Problem**: The attack doesn't detect hits on enemies.  
**Solution**:
- Verify socket names ("WeaponRoot" and "WeaponTip") exist on your weapon
- Check collision channels and make sure enemies are on the correct channel
- Increase the collision sphere radius for easier testing

### Character Gets Stuck in Attack State

**Problem**: The bIsAttacking flag never gets reset.  
**Solution**:
- Ensure the EndAttack() function is called at the end of the animation
- Add an animation notify at the end of your attack animation
- Verify the animation montage is set to proper loop settings (typically "Play Once")

## Next Steps

Now that you have a basic attack system, you can expand it in several ways:

1. Implement a damage system (covered in [Damage and Health Management](03_Damage_Health_Management.md))
2. Create a combo system (covered in [Combo Systems](02_Combo_Systems.md))
3. Add visual and audio effects (covered in [VFX and Feedback Systems](05_VFX_Feedback_Systems.md))
