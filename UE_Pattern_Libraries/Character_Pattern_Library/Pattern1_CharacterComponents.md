# Pattern 1: Character Component Framework

## Problem Definition

Game characters often require numerous interrelated systems (health, abilities, inventory, etc.) that can quickly lead to:
- Monolithic character classes with thousands of lines of code
- Tight coupling between systems
- Difficulty in reusing functionality across different character types
- Challenges in maintaining and extending character functionality
- Testing complications due to interdependencies

## Solution Approach

Implement a modular component-based architecture where:
1. Core character functionality is split into focused, reusable components
2. Components communicate through a well-defined interface
3. Dependencies between components are explicit and manageable
4. New character types can be created by composing different components

## Implementation

### Core Component Base Class

```cpp
// CharacterComponentBase.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterComponentBase.generated.h"

class ABaseCharacter;

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup=(Character), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UCharacterComponentBase : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterComponentBase();
    
    virtual void InitializeComponent() override;
    virtual void UninitializeComponent();
    
    // Called when all components have been initialized
    virtual void OnAllComponentsInitialized();
    
    // Component communication interface
    UFUNCTION(BlueprintCallable, Category = "Character|Component")
    virtual void ReceiveMessage(FName MessageName, const FString& MessageData);
    
    UFUNCTION(BlueprintCallable, Category = "Character|Component")
    void BroadcastMessage(FName MessageName, const FString& MessageData);
    
    // Getters
    FORCEINLINE ABaseCharacter* GetOwningCharacter() const { return OwningCharacter; }
    
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    UPROPERTY()
    ABaseCharacter* OwningCharacter;
    
    // Optional: Component dependencies
    UPROPERTY(EditAnywhere, Category = "Dependencies")
    TArray<TSubclassOf<UCharacterComponentBase>> RequiredComponents;
    
    // Check if all required components are available
    bool AreRequiredComponentsAvailable() const;
};
```

### Character Class Implementation

```cpp
// BaseCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCharacterComponentBase;

UCLASS()
class YOURGAME_API ABaseCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    ABaseCharacter();
    
    virtual void PostInitializeComponents() override;
    
    // Component retrieval helpers
    UFUNCTION(BlueprintCallable, Category = "Character|Components")
    UCharacterComponentBase* GetCharacterComponent(TSubclassOf<UCharacterComponentBase> ComponentClass) const;
    
    template<typename T>
    T* GetCharacterComponentByClass() const
    {
        return Cast<T>(GetCharacterComponent(T::StaticClass()));
    }
    
    // Broadcast message to all components
    UFUNCTION(BlueprintCallable, Category = "Character|Components")
    void BroadcastToComponents(FName MessageName, const FString& MessageData);
    
protected:
    virtual void BeginPlay() override;
    
    // Cache character components for quick access
    UPROPERTY()
    TArray<UCharacterComponentBase*> CharacterComponents;
    
    // Initialize all character components
    void InitializeCharacterComponents();
    
    // Notify all components that initialization is complete
    void NotifyAllComponentsInitialized();
};
```

### Example Component: Health System

```cpp
// HealthComponent.h
#pragma once

#include "CoreMinimal.h"
#include "CharacterComponentBase.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, HealthDelta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup=(Character), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UHealthComponent : public UCharacterComponentBase
{
    GENERATED_BODY()
    
public:
    UHealthComponent();
    
    virtual void InitializeComponent() override;
    
    // Health manipulation
    UFUNCTION(BlueprintCallable, Category = "Character|Health")
    bool TakeDamage(float DamageAmount, bool bCanKill = true);
    
    UFUNCTION(BlueprintCallable, Category = "Character|Health")
    bool HealDamage(float HealAmount);
    
    // Getters
    UFUNCTION(BlueprintPure, Category = "Character|Health")
    float GetCurrentHealth() const { return CurrentHealth; }
    
    UFUNCTION(BlueprintPure, Category = "Character|Health")
    float GetMaxHealth() const { return MaxHealth; }
    
    UFUNCTION(BlueprintPure, Category = "Character|Health")
    float GetHealthPercentage() const { return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f; }
    
    UFUNCTION(BlueprintPure, Category = "Character|Health")
    bool IsAlive() const { return !bIsDead; }
    
    // Message handling
    virtual void ReceiveMessage(FName MessageName, const FString& MessageData) override;
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Character|Health")
    FOnHealthChanged OnHealthChanged;
    
    UPROPERTY(BlueprintAssignable, Category = "Character|Health")
    FOnDeath OnDeath;
    
protected:
    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "0.0"))
    float MaxHealth;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
    bool bStartAtMaxHealth;
    
    // State
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Health")
    float CurrentHealth;
    
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health")
    bool bIsDead;
    
    // Replication
    UFUNCTION()
    void OnRep_CurrentHealth(float OldHealth);
    
    // Handle death
    virtual void Die();
    
    // Network
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

## Integration Tips

1. **Component Registration**:
   - Add components to your character blueprint or in C++ constructor
   - Ensure components are properly initialized in the correct order

2. **Component Communication**:
   - Use the message system for loose coupling between components
   - For performance-critical paths, consider direct references via GetCharacterComponentByClass<>()

3. **Replication Considerations**:
   - Mark component properties that need replication
   - Use OnRep functions for visual updates on clients

4. **Blueprint Extension**:
   - Make components blueprint-extendable for designer customization
   - Expose key functionality to blueprints with UFUNCTION(BlueprintCallable)

## Example Usage Scenarios

### Creating a New Character Type

```cpp
// Warrior.h
#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Warrior.generated.h"

UCLASS()
class YOURGAME_API AWarrior : public ABaseCharacter
{
    GENERATED_BODY()
    
public:
    AWarrior();
    
protected:
    // Add warrior-specific components in constructor
    // AWarrior::AWarrior()
    // {
    //     HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    //     StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
    //     CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
    // }
};
```

### Component Interaction Example

```cpp
// In CombatComponent.cpp
bool UCombatComponent::PerformAttack()
{
    // Check if we have enough stamina
    UStaminaComponent* StaminaComp = GetOwningCharacter()->GetCharacterComponentByClass<UStaminaComponent>();
    if (StaminaComp && !StaminaComp->ConsumeStamina(AttackStaminaCost))
    {
        // Not enough stamina
        return false;
    }
    
    // Perform the attack logic
    // ...
    
    return true;
}
```

## AI Prompt Templates

### Creating a New Character Component

```
Create a new character component for [SPECIFIC FUNCTIONALITY] that extends UCharacterComponentBase.
The component should handle [SPECIFIC REQUIREMENTS] and integrate with [OTHER COMPONENTS] through the message system.
Include proper replication for multiplayer support and expose key functionality to Blueprints.
```

### Debugging Component Interactions

```
Help me debug an issue with my character component system where [COMPONENT A] isn't properly communicating with [COMPONENT B].
The expected behavior is [DESCRIPTION], but I'm seeing [ACTUAL BEHAVIOR].
Here's my component implementation: [CODE]
```

## Performance Considerations

1. **Component Count**: 
   - Keep the number of components reasonable (typically under 10-15 per character)
   - Consider combining closely related functionality into a single component

2. **Message Optimization**:
   - Use direct component references for frequent communications
   - Reserve the message system for occasional events or loose coupling

3. **Initialization Order**:
   - Be mindful of dependencies between components
   - Use the OnAllComponentsInitialized callback for operations that require all components to be ready

## Common Pitfalls

1. **Circular Dependencies**: 
   - Avoid circular dependencies between components
   - Use the message system to break dependency cycles

2. **Over-componentization**: 
   - Don't create components for tiny pieces of functionality
   - Balance modularity with practical component granularity

3. **Replication Overhead**: 
   - Be selective about which properties need replication
   - Use RPCs for one-time events instead of replicated variables 