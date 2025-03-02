# Pattern 1: Enemy Base Framework

## Problem Statement
Creating a scalable and maintainable base architecture for 100+ enemy types that supports inheritance, component-based design, and efficient runtime performance while remaining accessible to non-engineers.

## Solution Overview
A hierarchical framework combining abstract base classes, interfaces, and component-based architecture to create a flexible and extensible enemy system.

## Core Components

### 1. Base Classes Hierarchy

```cpp
// Abstract base class for all enemies
UCLASS(Abstract)
class GAME_API ABaseEnemy : public ACharacter
{
    GENERATED_BODY()
public:
    // Core enemy interface
    virtual void InitializeEnemy();
    virtual float GetHealth() const;
    virtual void TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent) override;
    
protected:
    // Common enemy properties
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float MaxHealth;
    
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
    float CurrentHealth;
};

// Base class for standard enemies
UCLASS(Abstract)
class GAME_API AStandardEnemy : public ABaseEnemy
{
    GENERATED_BODY()
    // Standard enemy implementation
};

// Base class for elite enemies
UCLASS(Abstract)
class GAME_API AEliteEnemy : public ABaseEnemy
{
    GENERATED_BODY()
    // Elite enemy implementation with additional features
};

// Base class for boss enemies
UCLASS(Abstract)
class GAME_API ABossEnemy : public ABaseEnemy
{
    GENERATED_BODY()
    // Boss-specific implementation
};
```

### 2. Core Components

```cpp
// Base component for enemy abilities
UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class GAME_API UEnemyAbilityComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    // Interface for ability management
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    virtual void ActivateAbility(FName AbilityName);
    
protected:
    // Ability data structure
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<FEnemyAbilityData> Abilities;
};

// Component for enemy AI integration
UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class GAME_API UEnemyAIComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    // AI interface methods
    virtual void InitializeAI();
    virtual void UpdateBehavior();
    
protected:
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    class UBehaviorTree* BehaviorTree;
};
```

### 3. Data Assets

```cpp
// Enemy definition data asset
UCLASS()
class GAME_API UEnemyDefinition : public UDataAsset
{
    GENERATED_BODY()
public:
    // Basic enemy properties
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    FEnemyStats BaseStats;
    
    // Visual and gameplay settings
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    TSoftObjectPtr<USkeletalMesh> EnemyMesh;
    
    // AI and behavior settings
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> DefaultBehavior;
};
```

## Usage Examples

### 1. Creating a New Enemy Type

```cpp
// Example of creating a specific enemy type
UCLASS()
class GAME_API AArcherEnemy : public AStandardEnemy
{
    GENERATED_BODY()
public:
    AArcherEnemy();
    
    virtual void InitializeEnemy() override;
    virtual void BeginPlay() override;
    
protected:
    UPROPERTY(VisibleAnywhere)
    UEnemyRangedCombatComponent* RangedCombatComp;
    
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FArcherCombatSettings CombatSettings;
};
```

### 2. Component Setup

```cpp
// In the enemy constructor
AArcherEnemy::AArcherEnemy()
{
    // Create and setup components
    RangedCombatComp = CreateDefaultSubobject<UEnemyRangedCombatComponent>(TEXT("RangedCombatComp"));
    
    // Setup default values
    MaxHealth = 100.0f;
    
    // Enable replication
    bReplicates = true;
}
```

### 3. Data-Driven Configuration

```cpp
// Example of loading enemy configuration
void AArcherEnemy::InitializeEnemy()
{
    Super::InitializeEnemy();
    
    if (UEnemyDefinition* Definition = GetEnemyDefinition())
    {
        // Apply base stats
        MaxHealth = Definition->BaseStats.Health;
        CurrentHealth = MaxHealth;
        
        // Setup mesh
        if (Definition->EnemyMesh.IsValid())
        {
            GetMesh()->SetSkeletalMesh(Definition->EnemyMesh.Get());
        }
        
        // Initialize AI
        if (UEnemyAIComponent* AIComp = GetComponentByClass<UEnemyAIComponent>())
        {
            AIComp->InitializeAI();
        }
    }
}
```

## Best Practices

1. **Component Organization**
   - Keep components focused and single-purpose
   - Use interfaces for component communication
   - Implement proper component lifecycle management

2. **Performance Optimization**
   - Use object pooling for frequently spawned enemies
   - Implement proper LOD systems
   - Optimize network replication

3. **Data-Driven Design**
   - Store configuration in data assets
   - Use asset references for resources
   - Implement validation systems

## Integration Guidelines

1. **With Character System**
   ```cpp
   // Example of integrating with character movement
   void ABaseEnemy::SetupMovementComponent()
   {
       if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
       {
           // Apply movement settings
           MovementComp->bUseControllerDesiredRotation = true;
           MovementComp->bOrientRotationToMovement = true;
           MovementComp->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
       }
   }
   ```

2. **With Combat System**
   ```cpp
   // Example of combat system integration
   void ABaseEnemy::OnCombatStart()
   {
       // Notify components
       for (UActorComponent* Component : GetComponents())
       {
           if (IEnemyCombatInterface* CombatInterface = Cast<IEnemyCombatInterface>(Component))
           {
               CombatInterface->OnCombatStart();
           }
       }
   }
   ```

## Common Pitfalls

1. **Over-complexity in Inheritance**
   - Keep inheritance depth manageable
   - Use composition over inheritance when possible
   - Document inheritance relationships

2. **Performance Issues**
   - Monitor memory usage
   - Profile CPU usage
   - Optimize network traffic

3. **Data Management**
   - Implement proper data validation
   - Handle missing or invalid data
   - Provide clear error messages

## Testing Strategy

1. **Unit Tests**
   ```cpp
   // Example test case
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyBaseTest, "Enemy.Base", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   bool FEnemyBaseTest::RunTest(const FString& Parameters)
   {
       // Test enemy initialization
       ABaseEnemy* Enemy = CreateTestEnemy();
       TestTrue("Enemy should be properly initialized", Enemy->IsInitialized());
       
       // Test health system
       Enemy->TakeDamage(50.0f, FDamageEvent());
       TestEqual("Enemy health should be reduced", Enemy->GetHealth(), 50.0f);
       
       return true;
   }
   ```

2. **Performance Tests**
   - Measure spawn times
   - Test with maximum enemy counts
   - Profile memory usage

## Debugging Tools

1. **Visual Debugging**
   ```cpp
   void ABaseEnemy::DrawDebugInfo()
   {
       if (CVarShowEnemyDebug.GetValueOnGameThread())
       {
           // Draw debug shapes
           DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0f, 12, FColor::Red);
           
           // Draw debug text
           DrawDebugString(GetWorld(), GetActorLocation(), 
               FString::Printf(TEXT("Health: %.1f"), CurrentHealth),
               nullptr, FColor::White, 0.0f);
       }
   }
   ```

2. **Console Commands**
   ```cpp
   // Example console commands
   static FAutoConsoleCommand CVarSpawnEnemy(
       TEXT("Enemy.Spawn"),
       TEXT("Spawns an enemy of the specified type"),
       FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
       {
           // Implementation
       }));
   ```

## AI Integration

1. **Behavior Tree Setup**
   ```cpp
   void UEnemyAIComponent::InitializeAI()
   {
       if (AAIController* AIController = Cast<AAIController>(GetOwner()->GetController()))
       {
           if (BehaviorTree)
           {
               AIController->RunBehaviorTree(BehaviorTree);
           }
       }
   }
   ```

2. **Blackboard Integration**
   ```cpp
   void UEnemyAIComponent::UpdateBlackboard()
   {
       if (UBlackboardComponent* Blackboard = GetBlackboard())
       {
           // Update AI state
           Blackboard->SetValueAsVector(TEXT("HomeLocation"), GetOwner()->GetActorLocation());
           Blackboard->SetValueAsFloat(TEXT("HealthPercentage"), GetHealthPercentage());
       }
   }
   ```

## Network Considerations

1. **Replication Setup**
   ```cpp
   void ABaseEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
   {
       Super::GetLifetimeReplicatedProps(OutLifetimeProps);
       
       DOREPLIFETIME(ABaseEnemy, CurrentHealth);
       DOREPLIFETIME_CONDITION(ABaseEnemy, CombatState, COND_SkipOwner);
   }
   ```

2. **RPC Implementation**
   ```cpp
   UFUNCTION(NetMulticast, Reliable)
   void ABaseEnemy::MulticastOnDeath()
   {
       // Handle death effects and cleanup
       if (!IsNetMode(NM_DedicatedServer))
       {
           // Play death animation
           // Spawn effects
       }
   }
   ```

## Credits

This pattern is part of the Enemy Pattern Library, designed to provide a robust foundation for AAA game enemy systems in Unreal Engine 5. 