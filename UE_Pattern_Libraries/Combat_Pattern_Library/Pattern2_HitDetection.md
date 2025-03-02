# Hit Detection and Collision Pattern

## Problem
Implementing efficient and accurate hit detection for melee weapons and projectiles with support for:
- Multiple weapon trace shapes
- Hit stop/freeze frame effects
- Rich hit feedback and visual effects

## Solution Pattern
Use Unreal's collision system with configurable trace shapes, a hit stop manager for timing effects, and a modular hit feedback system.

## Code Example

```cpp
// FWeaponTraceSettings.h
#pragma once

#include "CoreMinimal.h"
#include "FWeaponTraceSettings.generated.h"

UENUM(BlueprintType)
enum class EWeaponTraceShape : uint8
{
    Sphere UMETA(DisplayName = "Sphere"),
    Box UMETA(DisplayName = "Box"),
    Capsule UMETA(DisplayName = "Capsule")
};

USTRUCT(BlueprintType)
struct YOURGAME_API FWeaponTraceSettings
{
    GENERATED_BODY()
    
    // Type of trace shape to use
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Trace")
    EWeaponTraceShape TraceShape = EWeaponTraceShape::Sphere;
    
    // Dimensions for trace shapes
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Trace")
    float SphereRadius = 10.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Trace")
    FVector BoxHalfExtent = FVector(10.0f, 5.0f, 5.0f);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Trace")
    float CapsuleRadius = 8.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Trace")
    float CapsuleHalfHeight = 20.0f;
};

// UHitStopManager.h
UCLASS()
class YOURGAME_API UHitStopManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // Apply hit stop effect
    UFUNCTION(BlueprintCallable, Category = "Combat|HitStop")
    void ApplyHitStop(float Duration, float TimeScale = 0.1f);
    
    // Update hit stop state
    virtual void Tick(float DeltaTime) override;
    
private:
    float CurrentHitStopDuration = 0.0f;
    float TargetTimeScale = 1.0f;
    bool bIsHitStopActive = false;
};

// FHitFeedback.h
USTRUCT(BlueprintType)
struct YOURGAME_API FHitFeedback
{
    GENERATED_BODY()
    
    // Visual effects
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Feedback|Visual")
    UNiagaraSystem* ImpactVFX = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Feedback|Visual")
    float VFXScale = 1.0f;
    
    // Sound effects
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Feedback|Audio")
    USoundBase* ImpactSound = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Feedback|Audio")
    float VolumeMultiplier = 1.0f;
    
    // Camera shake
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Feedback|Camera")
    TSubclassOf<UCameraShakeBase> CameraShake;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Feedback|Camera")
    float CameraShakeScale = 1.0f;
};

// AMeleeWeapon.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FWeaponTraceSettings.h"
#include "FHitFeedback.h"
#include "MeleeWeapon.generated.h"

UCLASS()
class YOURGAME_API AMeleeWeapon : public AActor
{
    GENERATED_BODY()
    
public:
    AMeleeWeapon();
    
    // Begin weapon trace for hit detection
    UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
    void BeginWeaponTrace();
    
    // End weapon trace and process hits
    UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
    void EndWeaponTrace();
    
protected:
    // Component for weapon mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WeaponMesh;
    
    // Socket name for weapon tip
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon")
    FName TipSocketName = "WeaponTip";
    
    // Socket name for weapon base
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon")
    FName BaseSocketName = "WeaponBase";
    
    // Damage data for this weapon
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Damage")
    FDamageData WeaponDamageData;
    
    // Previous position of weapon tip and base for sweep tests
    FVector PreviousTipLocation;
    FVector PreviousBaseLocation;
    
    // Actors that have already been hit in current swing
    UPROPERTY()
    TSet<AActor*> ActorsHitThisSwing;
    
    // Weapon trace settings
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon")
    FWeaponTraceSettings TraceSettings;
    
    // Hit feedback settings
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Feedback")
    FHitFeedback HitFeedback;
    
    // Hit stop settings
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|HitStop")
    float HitStopDuration = 0.1f;
    
    // Debug draw of weapon trace
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
    bool bShowDebugTrace = false;
    
    // Color for debug visualization
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug", meta = (EditCondition = "bShowDebugTrace"))
    FColor DebugTraceColor = FColor::Red;
    
    // Process a hit on an actor
    UFUNCTION()
    void ProcessHit(AActor* HitActor, const FHitResult& HitResult);
    
    // Perform weapon trace based on shape
    void PerformShapeTrace(TArray<FHitResult>& OutHitResults);
    
    // Apply hit feedback effects
    void ApplyHitFeedback(const FHitResult& HitResult);
};
```

### Implementation Example

```cpp
// UHitStopManager.cpp
void UHitStopManager::ApplyHitStop(float Duration, float TimeScale)
{
    if (Duration > CurrentHitStopDuration)
    {
        CurrentHitStopDuration = Duration;
        TargetTimeScale = TimeScale;
        bIsHitStopActive = true;
        
        // Set global time dilation
        if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) > TimeScale)
        {
            UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeScale);
        }
    }
}

void UHitStopManager::Tick(float DeltaTime)
{
    if (bIsHitStopActive)
    {
        CurrentHitStopDuration -= DeltaTime / TargetTimeScale;
        
        if (CurrentHitStopDuration <= 0.0f)
        {
            bIsHitStopActive = false;
            UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
        }
    }
}

// AMeleeWeapon.cpp
void AMeleeWeapon::PerformShapeTrace(TArray<FHitResult>& OutHitResults)
{
    if (!WeaponMesh)
        return;
    
    // Get current positions
    FVector CurrentTipLocation = WeaponMesh->GetSocketLocation(TipSocketName);
    FVector CurrentBaseLocation = WeaponMesh->GetSocketLocation(BaseSocketName);
    
    // Create query params
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = true;
    
    // Define object types
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    
    switch (TraceSettings.TraceShape)
    {
        case EWeaponTraceShape::Sphere:
            UKismetSystemLibrary::SphereTraceMultiForObjects(
                GetWorld(),
                PreviousTipLocation,
                CurrentTipLocation,
                TraceSettings.SphereRadius,
                ObjectTypes,
                false,
                TArray<AActor*>(),
                bShowDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                OutHitResults,
                true,
                DebugTraceColor,
                FColor::Green,
                1.0f
            );
            break;
            
        case EWeaponTraceShape::Box:
            UKismetSystemLibrary::BoxTraceMultiForObjects(
                GetWorld(),
                PreviousTipLocation,
                CurrentTipLocation,
                TraceSettings.BoxHalfExtent,
                FRotator::ZeroRotator,
                ObjectTypes,
                false,
                TArray<AActor*>(),
                bShowDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                OutHitResults,
                true,
                DebugTraceColor,
                FColor::Green,
                1.0f
            );
            break;
            
        case EWeaponTraceShape::Capsule:
            UKismetSystemLibrary::CapsuleTraceMultiForObjects(
                GetWorld(),
                PreviousTipLocation,
                CurrentTipLocation,
                TraceSettings.CapsuleRadius,
                TraceSettings.CapsuleHalfHeight,
                ObjectTypes,
                false,
                TArray<AActor*>(),
                bShowDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                OutHitResults,
                true,
                DebugTraceColor,
                FColor::Green,
                1.0f
            );
            break;
    }
}

void AMeleeWeapon::EndWeaponTrace()
{
    TArray<FHitResult> HitResults;
    PerformShapeTrace(HitResults);
    
    for (const FHitResult& HitResult : HitResults)
    {
        AActor* HitActor = HitResult.GetActor();
        
        if (!HitActor || ActorsHitThisSwing.Contains(HitActor))
            continue;
            
        ActorsHitThisSwing.Add(HitActor);
        ProcessHit(HitActor, HitResult);
    }
    
    // Update previous positions
    if (WeaponMesh)
    {
        PreviousTipLocation = WeaponMesh->GetSocketLocation(TipSocketName);
        PreviousBaseLocation = WeaponMesh->GetSocketLocation(BaseSocketName);
    }
}

void AMeleeWeapon::ProcessHit(AActor* HitActor, const FHitResult& HitResult)
{
    if (!HitActor)
        return;
    
    // Apply hit stop
    if (UHitStopManager* HitStopManager = GetGameInstance()->GetSubsystem<UHitStopManager>())
    {
        HitStopManager->ApplyHitStop(HitStopDuration);
    }
    
    // Apply hit feedback
    ApplyHitFeedback(HitResult);
    
    // Set target for our damage data
    WeaponDamageData.DamageTarget = HitActor;
    
    // Try to find damage handler component on hit actor
    UDamageHandlerComponent* DamageHandler = HitActor->FindComponentByClass<UDamageHandlerComponent>();
    if (DamageHandler)
    {
        // Process damage through our system
        float FinalDamage = DamageHandler->ProcessDamage(WeaponDamageData);
        
        // Spawn hit effects - could be moved to after damage is processed in the handler
        // to know exactly how much damage was applied
        if (FinalDamage > 0.0f)
        {
            // Play impact sound
            UGameplayStatics::PlaySoundAtLocation(
                this,
                HitFeedback.ImpactSound,
                HitResult.Location,
                HitFeedback.VolumeMultiplier
            );
            
            // Spawn impact particle
            if (HitFeedback.ImpactVFX)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    GetWorld(),
                    HitFeedback.ImpactVFX,
                    HitResult.Location,
                    HitResult.Normal.Rotation(),
                    FVector(HitFeedback.VFXScale),
                    true,
                    true,
                    ENCPoolMethod::AutoRelease
                );
            }
        }
    }
    else
    {
        // Fallback to standard UE damage system
        HitActor->TakeDamage(
            WeaponDamageData.BaseDamage,
            FDamageEvent(),
            GetInstigatorController(),
            this
        );
    }
}

void AMeleeWeapon::ApplyHitFeedback(const FHitResult& HitResult)
{
    // Spawn impact VFX
    if (HitFeedback.ImpactVFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            HitFeedback.ImpactVFX,
            HitResult.Location,
            HitResult.Normal.Rotation(),
            FVector(HitFeedback.VFXScale),
            true,
            true,
            ENCPoolMethod::AutoRelease
        );
    }
    
    // Play impact sound
    if (HitFeedback.ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            HitFeedback.ImpactSound,
            HitResult.Location,
            HitFeedback.VolumeMultiplier
        );
    }
    
    // Apply camera shake
    if (HitFeedback.CameraShake)
    {
        APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        if (CameraManager)
        {
            CameraManager->StartCameraShake(
                HitFeedback.CameraShake,
                HitFeedback.CameraShakeScale
            );
        }
    }
}
```

## AI Prompt Template

```
I need to implement weapon trace functionality for melee combat in Unreal Engine 5. 
Please implement the BeginWeaponTrace and EndWeaponTrace methods for a AMeleeWeapon class that:

1. In BeginWeaponTrace:
   - Clear the list of already hit actors
   - Store the current weapon tip and base socket positions

2. In EndWeaponTrace:
   - Get the new tip and base socket positions
   - Perform a sweep test from previous to current positions
   - For each hit actor (that wasn't hit already in this swing):
     - Apply the weapon's damage data
     - Add visual and audio feedback
     - Add the actor to the already hit list

3. Ensure we avoid hitting the weapon owner or already hit actors
4. Implement proper debug visualization when needed

Please provide C++ implementations following UE5 best practices.
```

## Performance Considerations
- Use object pooling for frequently spawned VFX/SFX
- Consider LOD system for hit effects at distance
- Optimize trace frequency based on weapon speed
- Cache transform lookups for socket locations

## Integration Tips
- Connect with animation system for proper weapon movement
- Use animation notifies to trigger weapon traces
- Consider adding material parameter collections for hit flash effects
- Implement hit surface types for different effects per material

## Example Usage with Animation Notifies

```cpp
// In a character animation blueprint
UCLASS()
class YOURGAME_API UMeleeAnimNotifyState : public UAnimNotifyState
{
    GENERATED_BODY()
    
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override
    {
        if (MeshComp && MeshComp->GetOwner())
        {
            // Find character's weapon
            ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
            if (Character)
            {
                // Get current weapon (implementation depends on your equipment system)
                AMeleeWeapon* Weapon = Character->GetCurrentMeleeWeapon();
                
                if (Weapon)
                {
                    // Begin the trace
                    Weapon->BeginWeaponTrace();
                }
            }
        }
    }
    
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override
    {
        if (MeshComp && MeshComp->GetOwner())
        {
            // Find character's weapon
            ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
            if (Character)
            {
                // Get current weapon
                AMeleeWeapon* Weapon = Character->GetCurrentMeleeWeapon();
                
                if (Weapon)
                {
                    // End the trace
                    Weapon->EndWeaponTrace();
                }
            }
        }
    }
    
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override
    {
        // Optional: For continuous weapons like chainsaws, you might want to check every tick
        // This is usually not needed for standard melee weapons
    }
};
``` 