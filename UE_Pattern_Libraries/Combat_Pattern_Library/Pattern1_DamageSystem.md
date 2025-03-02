# Damage System Implementation Pattern

## Problem
Implementing a flexible damage system that supports different damage types, resistances, and modifiers.

## Solution Pattern
Use a struct-based damage data approach with a centralized damage handling system that processes incoming damage requests.

## Code Example

```cpp
// FDamageData.h
#pragma once

#include "CoreMinimal.h"
#include "FDamageData.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical UMETA(DisplayName = "Physical"),
    Fire UMETA(DisplayName = "Fire"),
    Ice UMETA(DisplayName = "Ice"),
    Electric UMETA(DisplayName = "Electric"),
    Poison UMETA(DisplayName = "Poison")
};

USTRUCT(BlueprintType)
struct YOURGAME_API FDamageData
{
    GENERATED_BODY()
    
    // Base damage amount
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float BaseDamage = 0.0f;
    
    // Type of damage being applied
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EDamageType DamageType = EDamageType::Physical;
    
    // Critical hit multiplier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float CriticalMultiplier = 1.0f;
    
    // Whether damage is critical
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    bool bIsCritical = false;
    
    // Source that caused the damage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    AActor* DamageSource = nullptr;
    
    // Target receiving the damage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    AActor* DamageTarget = nullptr;
};

// UDamageHandlerComponent.h
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UDamageHandlerComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UDamageHandlerComponent();
    
    UFUNCTION(BlueprintCallable, Category = "Combat|Damage")
    float ProcessDamage(const FDamageData& DamageData);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Damage")
    TMap<EDamageType, float> DamageResistances;
    
protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Damage")
    void OnDamageProcessed(const FDamageData& DamageData, float FinalDamage);
};
```

### Implementation Example for ProcessDamage

```cpp
// UDamageHandlerComponent.cpp
#include "UDamageHandlerComponent.h"

UDamageHandlerComponent::UDamageHandlerComponent()
{
    // Default resistances (0.0 means no resistance)
    DamageResistances.Add(EDamageType::Physical, 0.0f);
    DamageResistances.Add(EDamageType::Fire, 0.0f);
    DamageResistances.Add(EDamageType::Ice, 0.0f);
    DamageResistances.Add(EDamageType::Electric, 0.0f);
    DamageResistances.Add(EDamageType::Poison, 0.0f);
}

float UDamageHandlerComponent::ProcessDamage(const FDamageData& DamageData)
{
    if (!IsValid(DamageData.DamageTarget) || DamageData.BaseDamage <= 0.0f)
    {
        return 0.0f;
    }
    
    // Start with base damage
    float FinalDamage = DamageData.BaseDamage;
    
    // Apply resistance if we have it for this damage type
    if (DamageResistances.Contains(DamageData.DamageType))
    {
        float Resistance = DamageResistances[DamageData.DamageType];
        // Convert resistance to damage reduction (0.0 = no reduction, 1.0 = full immunity)
        FinalDamage *= (1.0f - FMath::Clamp(Resistance, 0.0f, 1.0f));
    }
    
    // Apply critical hit modifier
    if (DamageData.bIsCritical && DamageData.CriticalMultiplier > 1.0f)
    {
        FinalDamage *= DamageData.CriticalMultiplier;
    }
    
    // Custom damage type modifications could be added here
    // For example, extra damage for fire against ice enemies
    
    // Apply final damage to target using UE's damage system
    if (FinalDamage > 0.0f)
    {
        // Apply to standard UE health systems if needed
        DamageData.DamageTarget->TakeDamage(
            FinalDamage,
            FDamageEvent(),
            DamageData.DamageSource ? 
                DamageData.DamageSource->GetInstigatorController() : nullptr,
            DamageData.DamageSource
        );
        
        // Notify blueprints about the processed damage
        OnDamageProcessed(DamageData, FinalDamage);
    }
    
    return FinalDamage;
}
```

## AI Prompt Template

```
I need to implement a damage calculation function for my Unreal Engine 5 combat system. 
The function should be part of a UDamageHandlerComponent class and should:

1. Take a FDamageData struct containing:
   - Base damage amount
   - Damage type (from an enum including Physical, Fire, Ice, Electric, Poison)
   - Critical hit multiplier and boolean
   - Source and target actors

2. Calculate final damage based on:
   - Applying resistances from a TMap<EDamageType, float> DamageResistances
   - Applying critical hit multiplier if bIsCritical is true
   - Any damage type specific logic (like fire doing more damage to ice-based enemies)

3. Return the final calculated damage amount

4. Call the OnDamageProcessed event with the original damage data and final damage

Please provide the implementation of the ProcessDamage function in C++ following UE5 best practices.
```

## Performance Considerations
- For larger games, consider using a flyweight pattern for common damage types
- Cache damage calculations for rapid successive hits
- For AOE damage, use spatial data structures to optimize target selection

## Integration Tips
- Connect this system with animation feedback via AnimNotifies
- Hook into the UE5 gameplay ability system if being used
- Consider integration with particle/VFX systems for visual feedback

## Usage Example

```cpp
// Example usage in a weapon or projectile
void AWeapon::ApplyDamageToTarget(AActor* TargetActor)
{
    // Ensure target has a damage handler component
    UDamageHandlerComponent* DamageHandler = TargetActor->FindComponentByClass<UDamageHandlerComponent>();
    if (DamageHandler)
    {
        // Create damage data
        FDamageData DamageData;
        DamageData.BaseDamage = WeaponDamage;
        DamageData.DamageType = WeaponDamageType;
        DamageData.DamageSource = this;
        DamageData.DamageTarget = TargetActor;
        
        // Check for critical hit
        float CriticalChance = 0.1f; // 10% chance
        if (FMath::FRand() <= CriticalChance)
        {
            DamageData.bIsCritical = true;
            DamageData.CriticalMultiplier = 2.0f; // Double damage for crits
        }
        
        // Process the damage
        float AppliedDamage = DamageHandler->ProcessDamage(DamageData);
        
        // Optional: Spawn hit effects based on damage type and amount
        SpawnHitEffect(TargetActor->GetActorLocation(), DamageData.DamageType, AppliedDamage);
    }
}
``` 