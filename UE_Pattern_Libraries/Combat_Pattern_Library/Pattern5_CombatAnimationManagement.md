# Combat Animation State Management Pattern

## Problem
Coordinating complex combat animations with gameplay logic, including handling attack chains, animation cancellations, and reactive animations (like hit reactions and blocks).

## Solution Pattern
Implement a specialized animation state machine with an integrated notifier system to synchronize gameplay events with animation states.

## Code Example

```cpp
// UCombatAnimInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CombatAnimInstance.generated.h"

// Custom enum for combat states
UENUM(BlueprintType)
enum class ECombatState : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    Attacking UMETA(DisplayName = "Attacking"),
    Blocking UMETA(DisplayName = "Blocking"),
    Dodging UMETA(DisplayName = "Dodging"),
    Stunned UMETA(DisplayName = "Stunned"),
    KnockedDown UMETA(DisplayName = "Knocked Down"),
    GettingUp UMETA(DisplayName = "Getting Up"),
    Dead UMETA(DisplayName = "Dead")
};

// Custom combat animation data
USTRUCT(BlueprintType)
struct FCombatAnimData
{
    GENERATED_BODY()
    
    // Current combat state
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    ECombatState CombatState = ECombatState::Idle;
    
    // Whether character is aiming/targeting
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bIsTargeting = false;
    
    // Direction of attack relative to character (for directional attacks)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FVector AttackDirection = FVector::ForwardVector;
    
    // Current attack combo count
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 ComboCount = 0;
    
    // Maximum combo for current weapon/attack type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 MaxComboCount = 3;
    
    // Whether we can transition to next combo move
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bCanComboTransition = false;
    
    // Weapon type influencing animations
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FName WeaponType = "Unarmed";
    
    // Whether character is moving while in combat
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bIsMoving = false;
    
    // Speed of movement for blending
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MovementSpeed = 0.0f;
    
    // Current stance (high/med/low)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FName CombatStance = "Medium";
    
    // Direction of hit reaction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FVector HitReactionDirection = FVector::ForwardVector;
    
    // Strength of hit reaction (for different reaction types)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float HitReactionStrength = 1.0f;
};

/**
 * Animation instance specifically designed for combat characters
 */
UCLASS()
class YOURGAME_API UCombatAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UCombatAnimInstance();
    
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    
    // Update combat parameters from character
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void UpdateCombatParameters();
    
    // Start attack animation sequence
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool StartAttackAnimation(FName AttackType, int32 ComboIndex = 0);
    
    // Try to continue combo if in valid window
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool ContinueCombo();
    
    // Play hit reaction animation
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool PlayHitReaction(FVector HitDirection, float Strength);
    
    // Reset combat state to idle
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ResetCombatState();
    
    // Combat animation data accessible to animation blueprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FCombatAnimData CombatData;
    
    // Get owner character as Combat Character (depends on your character class)
    UFUNCTION(BlueprintCallable, Category = "Combat")
    class ACharacter* GetCombatCharacterOwner() const;
    
protected:
    // Reference to the owning character
    UPROPERTY()
    class ACharacter* OwnerCharacter;
    
    // Map of attack type to montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    TMap<FName, UAnimMontage*> AttackMontages;
    
    // Map of weapon types to attack montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    TMap<FName, TMap<FName, UAnimMontage*>> WeaponAttackMontages;
    
    // Hit reaction montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    TMap<FName, UAnimMontage*> HitReactionMontages;
    
    // Dodge montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    TMap<FName, UAnimMontage*> DodgeMontages;
    
    // Death montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    TArray<UAnimMontage*> DeathMontages;
    
    // Time window after attack to allow combo
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    float ComboWindowTime = 0.3f;
    
    // End of current combo window time
    float CurrentComboWindowEnd = 0.0f;
    
    // Handler for end of montage
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    
    // Check if combo window is currently active
    bool IsInComboWindow() const;
    
    // Get appropriate montage for current weapon and attack type
    UAnimMontage* GetMontageForAttack(FName AttackType, int32 ComboIndex) const;
    
    // Get appropriate hit reaction montage
    UAnimMontage* GetHitReactionMontage(FVector HitDirection, float Strength) const;
    
    // Animation notify callback to open combo window
    UFUNCTION()
    void AnimNotify_OpenComboWindow();
    
    // Animation notify callback to close combo window
    UFUNCTION()
    void AnimNotify_CloseComboWindow();
    
    // Animation notify callback for weapon trail effects
    UFUNCTION()
    void AnimNotify_WeaponTrailStart();
    
    // Animation notify callback for weapon trail effects
    UFUNCTION()
    void AnimNotify_WeaponTrailEnd();
    
    // Animation notify callback for footstep sounds
    UFUNCTION()
    void AnimNotify_Footstep();
};
```

### Implementation Example

```cpp
// UCombatAnimInstance.cpp
#include "CombatAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCombatAnimInstance::UCombatAnimInstance()
{
    // Default initialization
}

void UCombatAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    // Cache the owner character
    OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    
    // Register for montage ended events
    OnMontageEnded.AddDynamic(this, &UCombatAnimInstance::OnMontageEnded);
}

void UCombatAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    
    UpdateCombatParameters();
}

ACharacter* UCombatAnimInstance::GetCombatCharacterOwner() const
{
    return OwnerCharacter;
}

void UCombatAnimInstance::UpdateCombatParameters()
{
    if (!OwnerCharacter)
    {
        return;
    }
    
    // Update movement-related parameters
    UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement();
    if (MovementComp)
    {
        CombatData.bIsMoving = MovementComp->Velocity.SizeSquared() > 25.0f;
        CombatData.MovementSpeed = MovementComp->Velocity.Size();
    }
    
    // In a real game, you'd interface with your combat component to get these values
    // Example (implement your own version of these functions):
    /*
    if (UCombatComponent* CombatComp = OwnerCharacter->FindComponentByClass<UCombatComponent>())
    {
        CombatData.bIsTargeting = CombatComp->IsTargeting();
        CombatData.WeaponType = CombatComp->GetCurrentWeaponType();
        CombatData.CombatStance = CombatComp->GetCurrentStance();
    }
    */
    
    // Check if combo window is still open
    if (IsInComboWindow())
    {
        CombatData.bCanComboTransition = true;
    }
    else
    {
        CombatData.bCanComboTransition = false;
    }
}

bool UCombatAnimInstance::StartAttackAnimation(FName AttackType, int32 ComboIndex)
{
    if (!OwnerCharacter)
    {
        return false;
    }
    
    // Validate combo index
    ComboIndex = FMath::Clamp(ComboIndex, 0, CombatData.MaxComboCount - 1);
    
    // Get appropriate montage for this attack
    UAnimMontage* AttackMontage = GetMontageForAttack(AttackType, ComboIndex);
    if (!AttackMontage)
    {
        return false;
    }
    
    // Update combat state
    CombatData.CombatState = ECombatState::Attacking;
    CombatData.ComboCount = ComboIndex;
    
    // Reset combo window
    CombatData.bCanComboTransition = false;
    CurrentComboWindowEnd = 0.0f;
    
    // Play the attack montage
    Montage_Play(AttackMontage);
    
    return true;
}

bool UCombatAnimInstance::ContinueCombo()
{
    // Can only continue combo if in valid window and not at max
    if (!IsInComboWindow() || CombatData.ComboCount >= CombatData.MaxComboCount - 1)
    {
        return false;
    }
    
    // Increment combo and start next attack
    int32 NextComboIndex = CombatData.ComboCount + 1;
    return StartAttackAnimation(FName("Default"), NextComboIndex);
}

bool UCombatAnimInstance::PlayHitReaction(FVector HitDirection, float Strength)
{
    if (!OwnerCharacter)
    {
        return false;
    }
    
    // Store hit reaction data
    CombatData.HitReactionDirection = HitDirection;
    CombatData.HitReactionStrength = Strength;
    
    // Get appropriate reaction montage
    UAnimMontage* ReactionMontage = GetHitReactionMontage(HitDirection, Strength);
    if (!ReactionMontage)
    {
        return false;
    }
    
    // Update combat state
    CombatData.CombatState = ECombatState::Stunned;
    
    // Play hit reaction
    Montage_Play(ReactionMontage);
    
    return true;
}

void UCombatAnimInstance::ResetCombatState()
{
    CombatData.CombatState = ECombatState::Idle;
    CombatData.ComboCount = 0;
    CombatData.bCanComboTransition = false;
    CurrentComboWindowEnd = 0.0f;
}

void UCombatAnimInstance::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // If we were in an attack state, return to idle when montage ends
    if (CombatData.CombatState == ECombatState::Attacking || 
        CombatData.CombatState == ECombatState::Stunned)
    {
        ResetCombatState();
    }
}

bool UCombatAnimInstance::IsInComboWindow() const
{
    if (CurrentComboWindowEnd <= 0.0f)
    {
        return false;
    }
    
    return UGameplayStatics::GetTimeSeconds(GetWorld()) < CurrentComboWindowEnd;
}

UAnimMontage* UCombatAnimInstance::GetMontageForAttack(FName AttackType, int32 ComboIndex) const
{
    // First try to find a weapon-specific attack montage
    if (WeaponAttackMontages.Contains(CombatData.WeaponType))
    {
        const TMap<FName, UAnimMontage*>& WeaponMontages = WeaponAttackMontages[CombatData.WeaponType];
        
        // Create attack name with combo index, e.g. "Slash_0", "Slash_1"
        FString ComboAttackName = FString::Printf(TEXT("%s_%d"), *AttackType.ToString(), ComboIndex);
        FName ComboAttackFName = FName(*ComboAttackName);
        
        // Try combo-specific attack first
        if (WeaponMontages.Contains(ComboAttackFName))
        {
            return WeaponMontages[ComboAttackFName];
        }
        
        // Fall back to base attack type
        if (WeaponMontages.Contains(AttackType))
        {
            return WeaponMontages[AttackType];
        }
    }
    
    // Fall back to general attack montages
    FString ComboAttackName = FString::Printf(TEXT("%s_%d"), *AttackType.ToString(), ComboIndex);
    FName ComboAttackFName = FName(*ComboAttackName);
    
    if (AttackMontages.Contains(ComboAttackFName))
    {
        return AttackMontages[ComboAttackFName];
    }
    
    if (AttackMontages.Contains(AttackType))
    {
        return AttackMontages[AttackType];
    }
    
    return nullptr;
}

UAnimMontage* UCombatAnimInstance::GetHitReactionMontage(FVector HitDirection, float Strength) const
{
    // Determine hit direction
    FVector OwnerForward = OwnerCharacter->GetActorForwardVector();
    float ForwardDot = FVector::DotProduct(OwnerForward, HitDirection);
    
    FName DirectionName;
    if (ForwardDot > 0.5f)
    {
        DirectionName = "Front";
    }
    else if (ForwardDot < -0.5f)
    {
        DirectionName = "Back";
    }
    else
    {
        // Determine if left or right
        FVector OwnerRight = OwnerCharacter->GetActorRightVector();
        float RightDot = FVector::DotProduct(OwnerRight, HitDirection);
        DirectionName = (RightDot > 0) ? "Right" : "Left";
    }
    
    // Determine strength name
    FName StrengthName;
    if (Strength < 0.3f)
    {
        StrengthName = "Light";
    }
    else if (Strength < 0.7f)
    {
        StrengthName = "Medium";
    }
    else
    {
        StrengthName = "Heavy";
    }
    
    // Combined name, e.g. "Front_Medium"
    FName CombinedName = FName(*(DirectionName.ToString() + "_" + StrengthName.ToString()));
    
    // Try to find specific reaction first
    if (HitReactionMontages.Contains(CombinedName))
    {
        return HitReactionMontages[CombinedName];
    }
    
    // Fall back to direction only
    if (HitReactionMontages.Contains(DirectionName))
    {
        return HitReactionMontages[DirectionName];
    }
    
    // Fall back to strength only
    if (HitReactionMontages.Contains(StrengthName))
    {
        return HitReactionMontages[StrengthName];
    }
    
    // Final fallback to default
    if (HitReactionMontages.Contains("Default"))
    {
        return HitReactionMontages["Default"];
    }
    
    return nullptr;
}

void UCombatAnimInstance::AnimNotify_OpenComboWindow()
{
    // Calculate when combo window ends
    CurrentComboWindowEnd = UGameplayStatics::GetTimeSeconds(GetWorld()) + ComboWindowTime;
    CombatData.bCanComboTransition = true;
}

void UCombatAnimInstance::AnimNotify_CloseComboWindow()
{
    CurrentComboWindowEnd = 0.0f;
    CombatData.bCanComboTransition = false;
}

void UCombatAnimInstance::AnimNotify_WeaponTrailStart()
{
    // Example of how you might handle this in a real game
    // This is a placeholder for implementation in your game
    
    // Get a component from owner character to trigger weapon trail
    /*
    if (UWeaponComponent* WeaponComp = OwnerCharacter->FindComponentByClass<UWeaponComponent>())
    {
        WeaponComp->ActivateWeaponTrail();
    }
    */
}

void UCombatAnimInstance::AnimNotify_WeaponTrailEnd()
{
    // Example placeholder
    /*
    if (UWeaponComponent* WeaponComp = OwnerCharacter->FindComponentByClass<UWeaponComponent>())
    {
        WeaponComp->DeactivateWeaponTrail();
    }
    */
}

void UCombatAnimInstance::AnimNotify_Footstep()
{
    // Placeholder for footstep sound implementation
    // You would typically play a sound based on surface type here
    
    /*
    if (OwnerCharacter)
    {
        // Get surface type under character
        FHitResult HitResult;
        FVector Start = OwnerCharacter->GetActorLocation();
        FVector End = Start + FVector(0, 0, -100);
        
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(OwnerCharacter);
        
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
        {
            // Determine surface type
            EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
            
            // Play appropriate sound
            UAudioComponent* FootstepAudio = UGameplayStatics::SpawnSoundAtLocation(
                this,
                GetFootstepSound(SurfaceType),
                HitResult.Location
            );
        }
    }
    */
}
```

## Creating Animation Blueprints for Combat

To use this pattern effectively, you'll need to create an Animation Blueprint that inherits from your `UCombatAnimInstance`. Here's how to set up the key parts of the Animation Blueprint:

1. Create state machines for different combat states
2. Implement smooth transitions between combat states
3. Setup blend spaces for directional movement
4. Configure animation notify events

## AI Prompt Template

```
I need to implement a comprehensive combat animation system for my third-person action game in Unreal Engine 5.
Please provide code for a UCombatAnimInstance class that:

1. Manages combat animation states including:
   - Idle/movement states
   - Attack combos with proper timing windows
   - Hit reactions with directional variation
   - Blocking, dodging, and special moves

2. Includes these key features:
   - A combo system with timing windows for follow-up attacks
   - Integration with different weapon types
   - Support for directional hit reactions
   - Animation notifies for gameplay events (weapon trails, footsteps, etc.)
   - State management for smooth transitions between animations

3. Provides a clean API for the character class to trigger animations:
   - StartAttackAnimation for initiating attacks
   - ContinueCombo for chaining attacks
   - PlayHitReaction for responding to damage

Please implement this in C++ following UE5 best practices while keeping performance in mind.
```

## Performance Considerations
- Use animation state caching to avoid recalculating values
- Implement LOD for animation complexity based on distance from camera
- Only check for combo opportunities during relevant attack phases
- Batch animation notify events rather than firing many small ones
- Disable unused features when in non-combat states

## Integration Tips
- Connect animation notifies with gameplay systems
- Synchronize with VFX and sound systems for hit impacts
- Use animation curves for precise combat timing
- Implement root motion for certain powerful attacks
- Create a data-driven approach for defining combos

## Example Usage

```cpp
// In your character class
void ACombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Bind attack input
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACombatCharacter::OnAttackInput);
    PlayerInputComponent->BindAction("Block", IE_Pressed, this, &ACombatCharacter::OnBlockStart);
    PlayerInputComponent->BindAction("Block", IE_Released, this, &ACombatCharacter::OnBlockEnd);
    PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ACombatCharacter::OnDodgeInput);
}

void ACombatCharacter::OnAttackInput()
{
    // Get our combat animation instance
    UCombatAnimInstance* CombatAnim = Cast<UCombatAnimInstance>(GetMesh()->GetAnimInstance());
    if (!CombatAnim)
    {
        return;
    }
    
    // If we're already in an attack and in combo window, continue combo
    if (CombatAnim->CombatData.CombatState == ECombatState::Attacking && 
        CombatAnim->CombatData.bCanComboTransition)
    {
        CombatAnim->ContinueCombo();
    }
    else if (CombatAnim->CombatData.CombatState == ECombatState::Idle)
    {
        // Start new attack sequence
        CombatAnim->StartAttackAnimation(CurrentAttackType, 0);
    }
    
    // If in other states (blocking, dodge, etc.), ignore input or queue it
}

// Example of receiving damage and playing hit reaction
float ACombatCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, 
                                  AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    // Don't play hit reaction if dead
    if (Health <= 0)
    {
        return ActualDamage;
    }
    
    UCombatAnimInstance* CombatAnim = Cast<UCombatAnimInstance>(GetMesh()->GetAnimInstance());
    if (CombatAnim)
    {
        // Calculate hit direction relative to character
        FVector HitDirection = FVector::ZeroVector;
        
        if (DamageCauser)
        {
            HitDirection = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        }
        else if (EventInstigator && EventInstigator->GetPawn())
        {
            HitDirection = (EventInstigator->GetPawn()->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        }
        
        // Calculate hit strength based on damage amount
        // This is just an example, scale appropriately for your game
        float HitStrength = FMath::Clamp(DamageAmount / 25.0f, 0.0f, 1.0f);
        
        // Play hit reaction animation
        CombatAnim->PlayHitReaction(HitDirection, HitStrength);
    }
    
    return ActualDamage;
}
``` 