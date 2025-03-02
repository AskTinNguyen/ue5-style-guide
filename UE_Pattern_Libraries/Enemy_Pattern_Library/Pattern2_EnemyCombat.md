# Pattern 2: Enemy Combat & Ability System

## Problem Statement
Creating a flexible and performant combat system that allows for complex enemy abilities, inheritable combat moves, and dynamic behavior while remaining maintainable and easy to extend.

## Solution Overview
A modular combat system using component-based architecture, data-driven ability definitions, and a robust state machine for combat behavior management.

## Core Components

### 1. Combat Component Base

```cpp
// Base component for enemy combat functionality
UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class GAME_API UEnemyCombatComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    // Core combat interface
    virtual void InitializeCombat();
    virtual void StartCombat(AActor* Target);
    virtual void EndCombat();
    
    // Ability system interface
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual bool ActivateAbility(const FName& AbilityName);
    
protected:
    // Combat state
    UPROPERTY(Replicated)
    EEnemyCombatState CombatState;
    
    // Current target
    UPROPERTY(Replicated)
    AActor* CurrentTarget;
    
    // Available abilities
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<FEnemyAbilityData> Abilities;
};
```

### 2. Ability System

```cpp
// Ability data structure
USTRUCT(BlueprintType)
struct FEnemyAbilityData
{
    GENERATED_BODY()
    
    // Ability identification
    UPROPERTY(EditDefaultsOnly)
    FName AbilityName;
    
    // Ability properties
    UPROPERTY(EditDefaultsOnly)
    float Cooldown;
    
    UPROPERTY(EditDefaultsOnly)
    float Range;
    
    UPROPERTY(EditDefaultsOnly)
    float Damage;
    
    // Animation data
    UPROPERTY(EditDefaultsOnly)
    UAnimMontage* AbilityMontage;
    
    // Effects
    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayEffect>> AbilityEffects;
};

// Ability execution context
USTRUCT()
struct FAbilityExecutionContext
{
    GENERATED_BODY()
    
    UPROPERTY()
    AActor* Target;
    
    UPROPERTY()
    FVector Location;
    
    UPROPERTY()
    float ExecutionTime;
};
```

### 3. Combat State Machine

```cpp
// Combat state enumeration
UENUM(BlueprintType)
enum class EEnemyCombatState : uint8
{
    Idle,
    Approaching,
    Attacking,
    Retreating,
    Stunned,
    Dead
};

// State machine implementation
class GAME_API FEnemyCombatStateMachine
{
public:
    void Initialize(UEnemyCombatComponent* Owner);
    void Update(float DeltaTime);
    void TransitionTo(EEnemyCombatState NewState);
    
protected:
    // State handlers
    virtual void HandleIdle();
    virtual void HandleApproaching();
    virtual void HandleAttacking();
    virtual void HandleRetreating();
    
    // Transition validation
    virtual bool CanTransitionTo(EEnemyCombatState NewState) const;
};
```

## Usage Examples

### 1. Implementing a Specific Combat Component

```cpp
// Ranged combat component example
UCLASS()
class GAME_API UEnemyRangedCombatComponent : public UEnemyCombatComponent
{
    GENERATED_BODY()
public:
    virtual void InitializeCombat() override;
    virtual bool ActivateAbility(const FName& AbilityName) override;
    
protected:
    // Ranged combat specific properties
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float PreferredRange;
    
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<AActor> ProjectileClass;
    
    // Helper functions
    virtual bool IsInRange() const;
    virtual void FireProjectile(const FEnemyAbilityData& Ability);
};
```

### 2. Ability Implementation

```cpp
// Example ability activation
bool UEnemyRangedCombatComponent::ActivateAbility(const FName& AbilityName)
{
    if (!Super::ActivateAbility(AbilityName))
    {
        return false;
    }
    
    const FEnemyAbilityData* Ability = FindAbility(AbilityName);
    if (!Ability)
    {
        return false;
    }
    
    // Create execution context
    FAbilityExecutionContext Context;
    Context.Target = CurrentTarget;
    Context.Location = GetOwner()->GetActorLocation();
    Context.ExecutionTime = GetWorld()->GetTimeSeconds();
    
    // Execute ability
    if (Ability->AbilityMontage)
    {
        PlayAbilityMontage(Ability->AbilityMontage);
    }
    
    // Apply effects
    for (auto EffectClass : Ability->AbilityEffects)
    {
        ApplyGameplayEffect(EffectClass, Context);
    }
    
    return true;
}
```

### 3. Combat State Implementation

```cpp
// Example state machine update
void FEnemyCombatStateMachine::Update(float DeltaTime)
{
    switch (CurrentState)
    {
        case EEnemyCombatState::Approaching:
            if (IsInAttackRange())
            {
                TransitionTo(EEnemyCombatState::Attacking);
            }
            else
            {
                UpdateApproach(DeltaTime);
            }
            break;
            
        case EEnemyCombatState::Attacking:
            if (!IsAbilityAvailable())
            {
                TransitionTo(EEnemyCombatState::Retreating);
            }
            else
            {
                ExecuteNextAbility();
            }
            break;
    }
}
```

## Best Practices

1. **Ability Design**
   - Keep abilities modular and reusable
   - Use data assets for ability configuration
   - Implement proper validation and error handling

2. **State Management**
   - Clear state transitions
   - State-specific behavior encapsulation
   - Proper state synchronization in multiplayer

3. **Performance Optimization**
   - Efficient ability execution
   - Smart update frequency
   - Network optimization

## Integration Guidelines

1. **With Animation System**
   ```cpp
   // Example animation integration
   void UEnemyCombatComponent::PlayAbilityAnimation(UAnimMontage* Montage)
   {
       if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
       {
           if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
           {
               float Duration = AnimInstance->Montage_Play(Montage);
               
               // Setup animation notifications
               FOnMontageEnded EndDelegate;
               EndDelegate.BindUObject(this, &UEnemyCombatComponent::OnAbilityAnimationEnd);
               AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);
           }
       }
   }
   ```

2. **With AI System**
   ```cpp
   // Example AI integration
   void UEnemyCombatComponent::UpdateAI()
   {
       if (UAIPerceptionComponent* Perception = GetOwner()->FindComponentByClass<UAIPerceptionComponent>())
       {
           // Update AI knowledge
           TArray<AActor*> PerceivedActors;
           Perception->GetPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
           
           // Update combat behavior based on perception
           for (AActor* Actor : PerceivedActors)
           {
               if (IsValidTarget(Actor))
               {
                   StartCombat(Actor);
                   break;
               }
           }
       }
   }
   ```

## Common Pitfalls

1. **Ability Timing**
   - Handle animation synchronization
   - Manage ability cooldowns
   - Handle interrupted abilities

2. **State Transitions**
   - Validate state changes
   - Handle edge cases
   - Clean up on state exit

3. **Network Considerations**
   - Proper ability replication
   - State synchronization
   - Latency handling

## Testing Strategy

1. **Ability Testing**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyAbilityTest, "Enemy.Combat.Ability", 
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyAbilityTest::RunTest(const FString& Parameters)
   {
       // Test ability activation
       UEnemyCombatComponent* CombatComp = CreateTestCombatComponent();
       TestTrue("Ability should activate successfully", CombatComp->ActivateAbility("TestAbility"));
       
       // Test cooldown system
       TestFalse("Ability should be on cooldown", CombatComp->ActivateAbility("TestAbility"));
       
       return true;
   }
   ```

2. **State Machine Testing**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyCombatStateTest, "Enemy.Combat.State",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyCombatStateTest::RunTest(const FString& Parameters)
   {
       // Test state transitions
       FEnemyCombatStateMachine StateMachine;
       TestTrue("Should transition to Approaching", 
           StateMachine.TransitionTo(EEnemyCombatState::Approaching));
       
       // Test invalid transitions
       TestFalse("Should not transition to Dead from Approaching",
           StateMachine.TransitionTo(EEnemyCombatState::Dead));
       
       return true;
   }
   ```

## Debugging Tools

1. **Visual Debugging**
   ```cpp
   void UEnemyCombatComponent::DrawDebugInfo()
   {
       if (CVarShowCombatDebug.GetValueOnGameThread())
       {
           // Draw ability ranges
           for (const FEnemyAbilityData& Ability : Abilities)
           {
               DrawDebugCylinder(GetWorld(), GetOwner()->GetActorLocation(),
                   GetOwner()->GetActorLocation() + FVector(0, 0, 10),
                   Ability.Range, 32, FColor::Blue, false, -1.0f, 0, 2.0f);
           }
           
           // Draw combat state
           FString StateString = UEnum::GetValueAsString(CombatState);
           DrawDebugString(GetWorld(), GetOwner()->GetActorLocation() + FVector(0, 0, 100),
               StateString, nullptr, FColor::White, 0.0f);
       }
   }
   ```

2. **Combat Logging**
   ```cpp
   void UEnemyCombatComponent::LogCombatEvent(const FString& EventName, const FString& Description)
   {
       if (CVarLogCombatEvents.GetValueOnGameThread())
       {
           UE_LOG(LogEnemyCombat, Log, TEXT("[%s] %s: %s"),
               *GetOwner()->GetName(), *EventName, *Description);
       }
   }
   ```

## Network Considerations

1. **Ability Replication**
   ```cpp
   UFUNCTION(Server, Reliable)
   void UEnemyCombatComponent::ServerActivateAbility(const FName& AbilityName)
   {
       if (!ActivateAbility(AbilityName))
       {
           return;
       }
       
       // Notify clients
       MulticastAbilityActivated(AbilityName);
   }
   
   UFUNCTION(NetMulticast, Reliable)
   void UEnemyCombatComponent::MulticastAbilityActivated(const FName& AbilityName)
   {
       if (!GetOwner()->HasAuthority())
       {
           PlayAbilityEffects(AbilityName);
       }
   }
   ```

2. **State Synchronization**
   ```cpp
   void UEnemyCombatComponent::GetLifetimeReplicatedProps(
       TArray<FLifetimeProperty>& OutLifetimeProps) const
   {
       Super::GetLifetimeReplicatedProps(OutLifetimeProps);
       
       DOREPLIFETIME(UEnemyCombatComponent, CombatState);
       DOREPLIFETIME(UEnemyCombatComponent, CurrentTarget);
   }
   
   UFUNCTION()
   void UEnemyCombatComponent::OnRep_CombatState()
   {
       // Update visual state
       UpdateCombatVisuals();
       
       // Update AI behavior
       UpdateAIBehavior();
   }
   ```

## Credits

This pattern is part of the Enemy Pattern Library, designed to provide a robust foundation for AAA game enemy combat systems in Unreal Engine 5. 