# Status Effect System Pattern

## Problem
Creating a flexible system for temporary status effects (buffs/debuffs) that can modify character attributes and behaviors.

## Solution Pattern
Implement a component-based status effect system with stackable, duration-based effects.

## Code Example

```cpp
// FStatusEffect.h
#pragma once

#include "CoreMinimal.h"
#include "FStatusEffect.generated.h"

UENUM(BlueprintType)
enum class EStatusEffectType : uint8
{
    Buff UMETA(DisplayName = "Buff"),
    Debuff UMETA(DisplayName = "Debuff"),
    Neutral UMETA(DisplayName = "Neutral")
};

USTRUCT(BlueprintType)
struct YOURGAME_API FStatusEffect
{
    GENERATED_BODY()
    
    // Unique identifier for this type of effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    FName EffectId;
    
    // Display name of the effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    FText DisplayName;
    
    // Type of effect (buff, debuff, neutral)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    EStatusEffectType EffectType = EStatusEffectType::Neutral;
    
    // Duration in seconds, 0 = permanent until removed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    float Duration = 5.0f;
    
    // Whether this effect can stack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    bool bCanStack = false;
    
    // Maximum stacks, 0 = unlimited
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    int32 MaxStacks = 0;
    
    // Strength multiplier for the effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    float Magnitude = 1.0f;
    
    // Source that applied the effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    AActor* EffectSource = nullptr;
    
    // Icon to display in UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    UTexture2D* EffectIcon = nullptr;
};

// For tracking active status effect instances
USTRUCT(BlueprintType)
struct YOURGAME_API FStatusEffectInstance
{
    GENERATED_BODY()
    
    // The base status effect data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    FStatusEffect EffectData;
    
    // Current stack count
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    int32 CurrentStacks = 1;
    
    // Remaining time for this effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    float RemainingTime = 0.0f;
    
    // When effect was applied (for UI sorting)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    float TimeApplied = 0.0f;
    
    // Visual effect associated with this instance
    UPROPERTY()
    UParticleSystemComponent* VisualEffect = nullptr;
};

// UStatusEffectComponent.h
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UStatusEffectComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UStatusEffectComponent();
    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Apply a new status effect
    UFUNCTION(BlueprintCallable, Category = "Combat|Status Effects")
    bool ApplyStatusEffect(const FStatusEffect& StatusEffect);
    
    // Remove a status effect by ID
    UFUNCTION(BlueprintCallable, Category = "Combat|Status Effects")
    bool RemoveStatusEffect(FName EffectId);
    
    // Check if has a specific effect
    UFUNCTION(BlueprintCallable, Category = "Combat|Status Effects")
    bool HasStatusEffect(FName EffectId) const;
    
    // Get the magnitude multiplier for a specific effect
    UFUNCTION(BlueprintCallable, Category = "Combat|Status Effects")
    float GetTotalEffectMagnitude(FName EffectId) const;
    
    // Get all active status effects
    UFUNCTION(BlueprintCallable, Category = "Combat|Status Effects")
    TArray<FStatusEffectInstance> GetActiveStatusEffects() const;
    
protected:
    // Currently active status effects
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Status Effects")
    TMap<FName, FStatusEffectInstance> ActiveEffects;
    
    // Processes effects duration and removes expired ones
    void UpdateEffectsDuration(float DeltaTime);
    
    // Map of effect type to particle effect to spawn
    UPROPERTY(EditDefaultsOnly, Category = "Combat|Status Effects|Visual")
    TMap<FName, UParticleSystem*> EffectVisuals;
    
    // Get reliable network replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Status Effects")
    void OnStatusEffectApplied(const FStatusEffect& StatusEffect, int32 CurrentStacks);
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Status Effects")
    void OnStatusEffectRemoved(FName EffectId);
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Status Effects")
    void OnStatusEffectStackChanged(FName EffectId, int32 NewStackCount);
};
```

### Implementation Example

```cpp
// UStatusEffectComponent.cpp
#include "StatusEffectComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UStatusEffectComponent::UStatusEffectComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void UStatusEffectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UStatusEffectComponent, ActiveEffects);
}

void UStatusEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Update effect durations and remove expired effects
    UpdateEffectsDuration(DeltaTime);
}

bool UStatusEffectComponent::ApplyStatusEffect(const FStatusEffect& StatusEffect)
{
    if (StatusEffect.EffectId.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot apply status effect with empty ID"));
        return false;
    }
    
    // Check if we already have this effect
    FStatusEffectInstance* ExistingEffect = ActiveEffects.Find(StatusEffect.EffectId);
    
    if (ExistingEffect)
    {
        // If effect exists and can stack
        if (StatusEffect.bCanStack)
        {
            // Check if we're at max stacks
            if (StatusEffect.MaxStacks > 0 && ExistingEffect->CurrentStacks >= StatusEffect.MaxStacks)
            {
                // At max stacks, just refresh duration
                ExistingEffect->RemainingTime = StatusEffect.Duration;
                return true;
            }
            
            // Add a stack
            ExistingEffect->CurrentStacks++;
            
            // Refresh duration
            ExistingEffect->RemainingTime = StatusEffect.Duration;
            
            // Update effect data in case properties changed
            ExistingEffect->EffectData = StatusEffect;
            
            // Notify stack changed
            OnStatusEffectStackChanged(StatusEffect.EffectId, ExistingEffect->CurrentStacks);
            
            return true;
        }
        else
        {
            // Just refresh duration for non-stacking effects
            ExistingEffect->RemainingTime = StatusEffect.Duration;
            
            // Update effect data in case properties changed
            ExistingEffect->EffectData = StatusEffect;
            
            return true;
        }
    }
    else
    {
        // New effect, add it
        FStatusEffectInstance NewInstance;
        NewInstance.EffectData = StatusEffect;
        NewInstance.CurrentStacks = 1;
        NewInstance.RemainingTime = StatusEffect.Duration;
        NewInstance.TimeApplied = UGameplayStatics::GetTimeSeconds(GetWorld());
        
        // Add visual effect if available
        if (EffectVisuals.Contains(StatusEffect.EffectId))
        {
            UParticleSystem* EffectTemplate = EffectVisuals[StatusEffect.EffectId];
            if (EffectTemplate)
            {
                NewInstance.VisualEffect = UGameplayStatics::SpawnEmitterAttached(
                    EffectTemplate,
                    Cast<USceneComponent>(GetOwner()->GetRootComponent()),
                    NAME_None,
                    FVector::ZeroVector,
                    FRotator::ZeroRotator,
                    EAttachLocation::SnapToTarget
                );
            }
        }
        
        // Add to active effects
        ActiveEffects.Add(StatusEffect.EffectId, NewInstance);
        
        // Notify effect applied
        OnStatusEffectApplied(StatusEffect, 1);
        
        return true;
    }
    
    return false;
}

bool UStatusEffectComponent::RemoveStatusEffect(FName EffectId)
{
    if (!ActiveEffects.Contains(EffectId))
    {
        return false;
    }
    
    // Get the effect instance before removal for cleanup
    FStatusEffectInstance& Instance = ActiveEffects[EffectId];
    
    // Clean up the visual effect if it exists
    if (Instance.VisualEffect)
    {
        Instance.VisualEffect->DestroyComponent();
    }
    
    // Remove the effect
    ActiveEffects.Remove(EffectId);
    
    // Notify removal
    OnStatusEffectRemoved(EffectId);
    
    return true;
}

bool UStatusEffectComponent::HasStatusEffect(FName EffectId) const
{
    return ActiveEffects.Contains(EffectId);
}

float UStatusEffectComponent::GetTotalEffectMagnitude(FName EffectId) const
{
    if (!ActiveEffects.Contains(EffectId))
    {
        return 0.0f;
    }
    
    const FStatusEffectInstance& Instance = ActiveEffects[EffectId];
    
    // For stacking effects, multiply magnitude by stack count
    return Instance.EffectData.Magnitude * Instance.CurrentStacks;
}

TArray<FStatusEffectInstance> UStatusEffectComponent::GetActiveStatusEffects() const
{
    TArray<FStatusEffectInstance> Result;
    ActiveEffects.GenerateValueArray(Result);
    return Result;
}

void UStatusEffectComponent::UpdateEffectsDuration(float DeltaTime)
{
    // Create a copy of effect IDs to avoid iterator invalidation when removing
    TArray<FName> EffectIds;
    ActiveEffects.GetKeys(EffectIds);
    
    for (const FName& EffectId : EffectIds)
    {
        // Skip if effect was removed during iteration
        if (!ActiveEffects.Contains(EffectId))
        {
            continue;
        }
        
        FStatusEffectInstance& Instance = ActiveEffects[EffectId];
        
        // If effect is permanent (Duration == 0), skip
        if (Instance.EffectData.Duration <= 0.0f)
        {
            continue;
        }
        
        // Reduce remaining time
        Instance.RemainingTime -= DeltaTime;
        
        // If expired, remove it
        if (Instance.RemainingTime <= 0.0f)
        {
            RemoveStatusEffect(EffectId);
        }
    }
}
```

## AI Prompt Template

```
I need to implement a flexible status effect system for my Unreal Engine 5 game. 
Please provide the implementation for a UStatusEffectComponent that:

1. Manages application of temporary buffs/debuffs through FStatusEffect structs that include:
   - Effect identifier, display name, and type (buff/debuff/neutral)
   - Duration, stack capability, max stacks, and magnitude
   - Source actor and UI icon

2. Implements these key functions:
   - ApplyStatusEffect: Adds or refreshes an effect, handling stacking logic
   - RemoveStatusEffect: Removes an effect by ID
   - HasStatusEffect: Checks if an effect is active
   - GetTotalEffectMagnitude: Returns combined strength of stacked effects
   - UpdateEffectsDuration: Decrements timers and removes expired effects

3. Handles replication for multiplayer support
4. Provides blueprint events for effect application and removal

Please implement the core functionality in C++ following UE5 best practices.
```

## Performance Considerations
- Use a sparse data structure for active effects rather than a large array
- Consider batching status effect updates rather than updating every tick
- For visual effects, pool particle systems instead of spawning new ones
- Use a custom update rate for effects that don't need to update every frame

## Integration Tips
- Connect with UI system for buff/debuff indicators
- Hook into attribute system for stat modifications
- Use for both combat effects (burning, poisoned) and utility effects (speed boost)
- Attribute modifiers can be implemented as a separate system that queries the status effect component

## Example Usage

```cpp
// Apply a burning status effect to a target
void ADamageDealer::ApplyBurningEffect(AActor* Target)
{
    UStatusEffectComponent* StatusComp = Target->FindComponentByClass<UStatusEffectComponent>();
    if (StatusComp)
    {
        // Create a burning effect
        FStatusEffect BurningEffect;
        BurningEffect.EffectId = FName("Burning");
        BurningEffect.DisplayName = FText::FromString("Burning");
        BurningEffect.EffectType = EStatusEffectType::Debuff;
        BurningEffect.Duration = 5.0f;
        BurningEffect.bCanStack = true;
        BurningEffect.MaxStacks = 3;
        BurningEffect.Magnitude = 10.0f; // 10 damage per tick
        BurningEffect.EffectSource = this;
        BurningEffect.EffectIcon = BurningIcon; // Assigned in editor
        
        // Apply the effect
        StatusComp->ApplyStatusEffect(BurningEffect);
    }
}

// In a character class, apply damage based on burning effect
void AMyCharacter::ApplyDamageFromStatusEffects(float DeltaTime)
{
    UStatusEffectComponent* StatusComp = FindComponentByClass<UStatusEffectComponent>();
    if (StatusComp && StatusComp->HasStatusEffect(FName("Burning")))
    {
        // Get total magnitude of burning effect (base damage × stack count)
        float BurnDamage = StatusComp->GetTotalEffectMagnitude(FName("Burning")) * DeltaTime;
        
        // Apply damage
        Health -= BurnDamage;
        
        // Show burning visual feedback
        if (!BurningParticle && Health > 0)
        {
            BurningParticle = UGameplayStatics::SpawnEmitterAttached(
                BurningParticleTemplate,
                GetMesh(),
                NAME_None,
                FVector(0, 0, 50),  // Offset above character
                FRotator::ZeroRotator,
                EAttachLocation::SnapToTarget
            );
        }
    }
    else if (BurningParticle)
    {
        // No longer burning, remove visual effect
        BurningParticle->DestroyComponent();
        BurningParticle = nullptr;
    }
}
```

## Advanced Implementation

### Enhanced Data Structures

```cpp
// Advanced Status Effect Definition
USTRUCT(BlueprintType)
struct FAdvancedStatusEffect : public FStatusEffect
{
    GENERATED_BODY()
    
    // Immunity period after effect expires
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect|Advanced")
    float ImmunityDuration = 0.0f;
    
    // Whether effect can be cleansed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect|Advanced")
    bool bCanBeCleansed = true;
    
    // Priority level for conflicting effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect|Advanced")
    int32 Priority = 0;
    
    // Gameplay tags required to apply this effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect|Advanced")
    FGameplayTagContainer RequiredTags;
    
    // Gameplay tags that prevent this effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect|Advanced")
    FGameplayTagContainer BlockingTags;
    
    // Effect application chance (0-1)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect|Advanced", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ApplicationChance = 1.0f;
    
    // Custom curve for effect magnitude over time
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect|Advanced")
    UCurveFloat* MagnitudeOverTime = nullptr;
};

// Advanced Stack Management
USTRUCT(BlueprintType)
struct FStatusEffectStack
{
    GENERATED_BODY()
    
    // Individual stack data
    UPROPERTY()
    float RemainingDuration;
    
    UPROPERTY()
    float CurrentMagnitude;
    
    UPROPERTY()
    float TimeApplied;
    
    // Source that applied this stack
    UPROPERTY()
    TWeakObjectPtr<AActor> StackSource;
};

USTRUCT(BlueprintType)
struct FAdvancedStatusEffectInstance : public FStatusEffectInstance
{
    GENERATED_BODY()
    
    // Track individual stacks
    UPROPERTY()
    TArray<FStatusEffectStack> Stacks;
    
    // Last time immunity expired
    UPROPERTY()
    float LastImmunityTime = 0.0f;
    
    // Current effect potency (for resistance calculation)
    UPROPERTY()
    float CurrentPotency = 1.0f;
};
```

### Performance Optimizations

```cpp
UCLASS()
class UOptimizedStatusEffectComponent : public UStatusEffectComponent
{
    GENERATED_BODY()
    
public:
    UOptimizedStatusEffectComponent()
    {
        PrimaryComponentTick.bCanEverTick = false; // Disable tick
    }
    
protected:
    // Timer handle for effect updates
    FTimerHandle EffectUpdateTimer;
    
    // Configurable update interval
    UPROPERTY(EditDefaultsOnly, Category = "Status Effects|Performance")
    float UpdateInterval = 0.1f;
    
    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        
        // Set up timer-based updates
        GetWorld()->GetTimerManager().SetTimer(
            EffectUpdateTimer,
            this,
            &UOptimizedStatusEffectComponent::ProcessEffectUpdates,
            UpdateInterval,
            true
        );
    }
    
    void ProcessEffectUpdates()
    {
        // Process effects in batches
        const int32 MaxEffectsPerFrame = 5;
        int32 ProcessedEffects = 0;
        
        TArray<FName> EffectIds;
        ActiveEffects.GetKeys(EffectIds);
        
        for (const FName& EffectId : EffectIds)
        {
            if (ProcessedEffects >= MaxEffectsPerFrame)
            {
                break;
            }
            
            UpdateSingleEffect(EffectId);
            ProcessedEffects++;
        }
    }
};
```

### Combat Integration Features

```cpp
// Effect Resistance System
USTRUCT(BlueprintType)
struct FStatusEffectResistance
{
    GENERATED_BODY()
    
    // Base resistance value (0-1)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Resistance")
    float BaseResistance = 0.0f;
    
    // Additional resistance per stack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Resistance")
    float StackResistance = 0.1f;
    
    // Maximum resistance possible
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Resistance")
    float MaxResistance = 0.75f;
};

// Effect Combination System
USTRUCT(BlueprintType)
struct FStatusEffectCombination
{
    GENERATED_BODY()
    
    // Required effects for combination
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Combinations")
    TArray<FName> RequiredEffects;
    
    // Effect to apply when combination occurs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Combinations")
    FAdvancedStatusEffect ResultEffect;
    
    // Whether to consume the source effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Combinations")
    bool bConsumeSourceEffects = true;
};
```

### Visual Feedback System

```cpp
USTRUCT(BlueprintType)
struct FStatusEffectVisuals
{
    GENERATED_BODY()
    
    // Particle system for the effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Feedback")
    UNiagaraSystem* EffectParticles;
    
    // Sound to play when effect is applied
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Feedback")
    USoundBase* ApplySound;
    
    // Sound to play when effect is removed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Feedback")
    USoundBase* RemoveSound;
    
    // Material parameter collection for effect visualization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Feedback")
    UMaterialParameterCollection* MaterialParameters;
    
    // Post process settings for this effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Feedback")
    FPostProcessSettings PostProcessSettings;
};
```

### Network Optimization

```cpp
UCLASS()
class UNetworkedStatusEffectComponent : public UStatusEffectComponent
{
    GENERATED_BODY()
    
protected:
    // Replicated array of active effects for efficient networking
    UPROPERTY(ReplicatedUsing = OnRep_ActiveEffects)
    TArray<FCompressedStatusEffect> CompressedActiveEffects;
    
    UFUNCTION()
    void OnRep_ActiveEffects()
    {
        // Decompress and apply effects on remote clients
        for (const FCompressedStatusEffect& CompressedEffect : CompressedActiveEffects)
        {
            FAdvancedStatusEffect DecompressedEffect = DecompressStatusEffect(CompressedEffect);
            ApplyStatusEffect(DecompressedEffect);
        }
    }
};
```

## Advanced Usage Examples

```cpp
// Example of applying a complex burning effect with resistance
void ACombatCharacter::ApplyAdvancedBurningEffect(AActor* Target)
{
    UOptimizedStatusEffectComponent* StatusComp = Target->FindComponentByClass<UOptimizedStatusEffectComponent>();
    if (StatusComp)
    {
        FAdvancedStatusEffect BurningEffect;
        BurningEffect.EffectId = FName("Burning");
        BurningEffect.DisplayName = FText::FromString("Burning");
        BurningEffect.EffectType = EStatusEffectType::Debuff;
        BurningEffect.Duration = 5.0f;
        BurningEffect.bCanStack = true;
        BurningEffect.MaxStacks = 3;
        BurningEffect.Magnitude = 10.0f;
        BurningEffect.ImmunityDuration = 1.0f;
        BurningEffect.ApplicationChance = 0.8f;
        
        // Add required tags
        BurningEffect.RequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Status.CanBurn")));
        BurningEffect.BlockingTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Status.FireImmune")));
        
        // Apply with resistance check
        if (UStatusEffectResistanceComponent* ResistComp = Target->FindComponentByClass<UStatusEffectResistanceComponent>())
        {
            float Resistance = ResistComp->CalculateResistance(BurningEffect.EffectId);
            if (FMath::FRand() > Resistance)
            {
                StatusComp->ApplyStatusEffect(BurningEffect);
            }
        }
    }
}
```

## Advanced Performance Considerations

1. **Effect Update Optimization**
   - Use timer-based updates instead of tick
   - Process effects in batches
   - Implement effect pooling for frequently applied effects

2. **Memory Management**
   - Use weak pointers for effect sources
   - Pool particle systems and sounds
   - Compress network data for replicated effects

3. **Network Optimization**
   - Implement custom serialization for large effect arrays
   - Use relevancy checks for effect replication
   - Batch network updates for multiple effects

## Advanced Integration Tips

1. **Combat System Integration**
   - Hook into the damage system for reactive effects
   - Implement effect immunity system
   - Create effect combination manager

2. **Animation Integration**
   - Sync effect timings with animations
   - Use gameplay tags for effect states
   - Implement custom notify states for effects

3. **UI Integration**
   - Create dynamic UI feedback system
   - Implement effect priority for UI display
   - Add detailed tooltips for effect combinations

// ... existing code ... 