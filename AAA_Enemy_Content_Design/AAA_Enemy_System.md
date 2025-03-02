# AAA Enemy System Documentation

## Overview

This document provides comprehensive technical documentation for implementing a Stellar Blade-inspired enemy system for our AAA hack and slash game using Unreal Engine 5. The system is designed to provide game designers with intuitive tools for creating, managing, and balancing enemies while maintaining high performance standards and visual quality.

The enemy system is built upon the Enemy Pattern Library's architectural foundation, with additional gameplay elements inspired by Stellar Blade's combat-focused design. This combination creates a robust framework that supports the creation of diverse, challenging, and memorable enemy encounters.

## Table of Contents

- [System Architecture](#system-architecture)
- [Enemy Classification System](#enemy-classification-system)
- [Enemy Data Structure](#enemy-data-structure)
- [AI System](#ai-system)
- [Combat System](#combat-system)
- [Animation System](#animation-system)
- [Visual Effects System](#visual-effects-system)
- [Designer Tools Implementation](#designer-tools-implementation)
- [Balancing & Testing Tools](#balancing--testing-tools)
- [Performance Optimization](#performance-optimization)
- [Blueprint Integration](#blueprint-integration)
- [Example Implementations](#example-implementations)
- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)
- [Future Extensions](#future-extensions)

## System Architecture

The enemy system architecture follows a component-based design that emphasizes modularity, extensibility, and performance. This approach allows for efficient reuse of code, clear separation of concerns, and simplified maintenance.

### Core Components

The system is built around the following core components:

#### 1. Base Enemy Actor

```cpp
// Base class for all enemy types
UCLASS(Abstract)
class GAME_API ABaseEnemy : public ACharacter
{
    GENERATED_BODY()
    
public:
    ABaseEnemy();
    
    // Core enemy interface
    virtual void InitializeEnemy(const FEnemyData& EnemyData);
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, 
                           AController* EventInstigator, AActor* DamageCauser) override;
    virtual void Die();
    
    // Component accessors
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    UEnemyCombatComponent* GetCombatComponent() const { return CombatComponent; }
    
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    UEnemyEvolutionComponent* GetEvolutionComponent() const { return EvolutionComponent; }
    
protected:
    // Core components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEnemyCombatComponent* CombatComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEnemyAIComponent* AIComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEnemyEvolutionComponent* EvolutionComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEnemyStatsComponent* StatsComponent;
    
    // Enemy data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyData EnemyData;
    
    // Network replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

#### 2. Component Hierarchy

The system uses a set of specialized components that handle different aspects of enemy functionality:

```
- UEnemyCombatComponent (Combat abilities and attacks)
  ├── UEnemyMeleeCombatComponent (Melee-specific combat)
  ├── UEnemyRangedCombatComponent (Ranged-specific combat)
  └── UEnemySupportCombatComponent (Support/buff abilities)

- UEnemyAIComponent (AI behavior and decision making)
  ├── UEnemyPatrolComponent (Movement patterns when not in combat)
  ├── UEnemyCombatAIComponent (Combat decision making)
  └── UEnemyGroupAIComponent (Coordination with other enemies)

- UEnemyEvolutionComponent (Evolution and progression)

- UEnemyStatsComponent (Stats and attributes)
```

#### 3. Manager Classes

Several manager classes coordinate system-wide functionality:

```cpp
// Manages enemy spawning and pooling
UCLASS()
class GAME_API UEnemySpawnManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // Initialize and spawn enemies
    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    ABaseEnemy* SpawnEnemy(TSubclassOf<ABaseEnemy> EnemyClass, const FTransform& SpawnTransform, 
                          const FEnemyData& EnemyData);
    
    // Enemy pooling
    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void ReturnEnemyToPool(ABaseEnemy* Enemy);
    
private:
    // Enemy pools by type
    UPROPERTY()
    TMap<TSubclassOf<ABaseEnemy>, TArray<ABaseEnemy*>> EnemyPools;
};

// Manages enemy encounters and difficulty scaling
UCLASS()
class GAME_API UEnemyEncounterManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // Create and manage encounters
    UFUNCTION(BlueprintCallable, Category = "Enemy Encounters")
    void InitiateEncounter(const FEnemyEncounterData& EncounterData);
    
    // Dynamic difficulty adjustment
    UFUNCTION(BlueprintCallable, Category = "Enemy Encounters")
    void AdjustDifficulty(float PlayerPerformanceRating);
    
private:
    // Active encounters
    UPROPERTY()
    TArray<FActiveEncounter> ActiveEncounters;
};
```

### Data Flow

The system's data flow follows this general pattern:

1. **Data Definition**: Enemy data is defined in data assets and tables
2. **Initialization**: Enemies are spawned and initialized with data
3. **Runtime Behavior**: Components handle specific aspects of behavior
4. **Inter-Component Communication**: Components communicate through delegates and interfaces
5. **Manager Coordination**: Manager classes handle system-wide concerns

### Integration Points

The enemy system integrates with other game systems through these primary interfaces:

1. **Combat System**: Damage handling, attack registration, combat effects
2. **Animation System**: Animation state machines, montages, and blending
3. **AI Perception**: Sight, hearing, and other sensory inputs
4. **Navigation System**: Pathfinding and movement
5. **VFX System**: Visual effects for attacks, states, and feedback
6. **Audio System**: Sound cues for enemy actions and states

### Networking Architecture

The enemy system is designed with multiplayer in mind, using these networking principles:

1. **Server Authority**: Server controls all enemy behavior and decisions
2. **State Replication**: Key state variables are replicated to clients
3. **Visual Prediction**: Clients predict visual aspects for responsiveness
4. **Bandwidth Optimization**: Prioritized replication based on relevance

```cpp
// Example of network-aware implementation
void ABaseEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // Replicate essential properties
    DOREPLIFETIME(ABaseEnemy, CurrentHealth);
    DOREPLIFETIME(ABaseEnemy, CurrentState);
    
    // Replicate with conditions for optimization
    DOREPLIFETIME_CONDITION(ABaseEnemy, DetailedState, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(ABaseEnemy, AttackData, COND_SimulatedOnly);
}
```

## Enemy Classification System

The enemy classification system provides a structured framework for categorizing enemies based on their role, threat level, and gameplay function. This classification system is inspired by Stellar Blade's clear enemy hierarchy while adding additional depth for our game's needs.

### Hierarchical Classification

Enemies are classified into five main hierarchical categories:

```cpp
// Enemy category enumeration
UENUM(BlueprintType)
enum class EEnemyCategory : uint8
{
    NaytibaMinion    UMETA(DisplayName = "Naytiba Minion"),
    NaytibaWarrior   UMETA(DisplayName = "Naytiba Warrior"),
    EliteNaytiba     UMETA(DisplayName = "Elite Naytiba"),
    AlphaNaytiba     UMETA(DisplayName = "Alpha Naytiba"),
    ElderNaytiba     UMETA(DisplayName = "Elder Naytiba")
};
```

#### 1. Naytiba Minions

Basic enemies that pose limited threat individually but can be dangerous in groups.

**Implementation Characteristics:**
- Simplified AI with fewer states
- Limited attack patterns (1-2 attacks)
- Lower health and damage values
- Often spawned in groups
- Optimized for performance (LOD, simplified physics)

**Example Types:**
- Thornhead: Basic melee attacker
- Creeper: Explosive suicide unit
- Mite: Small, fast swarming unit

#### 2. Naytiba Warriors

Standard combat enemies with defined attack patterns and moderate threat level.

**Implementation Characteristics:**
- More sophisticated AI with multiple states
- Multiple attack patterns (2-4 attacks)
- Moderate health and damage values
- Can function alone or in small groups
- Full animation and physics systems

**Example Types:**
- Beholder: Standard melee combatant
- Cricket Slasher: Agile multi-limb attacker
- Hydra: Stealth-capable ambusher

#### 3. Elite Naytiba

Specialized mini-boss enemies with unique abilities and substantial health pools.

**Implementation Characteristics:**
- Complex AI with multiple combat phases
- Diverse attack patterns (4-6 attacks)
- High health and damage values
- Usually encountered alone or with minion support
- Unique abilities and mechanics
- Full animation and VFX treatment

**Example Types:**
- Brute: Heavy explosive specialist
- Stalker: Fast, deadly assassin
- Abaddon: Electrical area-control enemy

#### 4. Alpha Naytiba

Major boss enemies with complex, multi-phase combat encounters.

**Implementation Characteristics:**
- Sophisticated multi-phase AI
- Extensive attack repertoire (6+ attacks)
- Very high health and damage values
- Unique arena mechanics
- Special entrance and death sequences
- Custom animation and VFX treatment

**Example Types:**
- Gigas: Massive brute force boss
- Tachy: Swift, technical combat boss
- Raven: Deceptive, trick-based boss

#### 5. Elder Naytiba

Ultimate boss enemies with story significance and exceptional combat challenges.

**Implementation Characteristics:**
- Most complex AI with multiple distinct phases
- Complete attack repertoire with unique mechanics
- Highest health and damage values
- Integrated cutscenes and story elements
- Custom arena with environmental mechanics
- Bespoke animation and VFX treatment

### Threat Level Classification

Within each category, enemies are further classified by threat level:

```cpp
// Enemy threat level enumeration
UENUM(BlueprintType)
enum class EEnemyThreatLevel : uint8
{
    Common      UMETA(DisplayName = "Common"),    // ⭐
    Uncommon    UMETA(DisplayName = "Uncommon"),  // ⭐⭐
    Rare        UMETA(DisplayName = "Rare"),      // ⭐⭐⭐
    Epic        UMETA(DisplayName = "Epic"),      // ⭐⭐⭐⭐
    Legendary   UMETA(DisplayName = "Legendary")  // ⭐⭐⭐⭐⭐
};
```

This threat level system affects:
- Base stat multipliers
- Attack damage and complexity
- AI sophistication
- Visual effects quality
- Reward values

### Combat Role Classification

Enemies are also classified by their primary combat role:

```cpp
// Enemy combat role enumeration
UENUM(BlueprintType)
enum class EEnemyCombatRole : uint8
{
    Melee       UMETA(DisplayName = "Melee"),
    Ranged      UMETA(DisplayName = "Ranged"),
    Tank        UMETA(DisplayName = "Tank"),
    Support     UMETA(DisplayName = "Support"),
    Assassin    UMETA(DisplayName = "Assassin"),
    Controller  UMETA(DisplayName = "Controller")
};
```

Each combat role has specific implementation characteristics:

#### Melee Role
- Close-range attack focus
- Direct approach patterns
- Higher health than ranged units
- Combo-based attack sequences

#### Ranged Role
- Distance attack focus
- Evasive movement patterns
- Lower health than melee units
- Area control abilities

#### Tank Role
- Defensive focus
- High health and armor
- Crowd control abilities
- Aggro management mechanics

#### Support Role
- Ally enhancement focus
- Healing or buffing abilities
- Summoning capabilities
- Priority targeting logic

#### Assassin Role
- Burst damage focus
- Stealth or ambush mechanics
- High mobility
- Target prioritization logic

#### Controller Role
- Crowd control focus
- Status effect abilities
- Area denial mechanics
- Tactical positioning logic

### Evolution Classification

The enemy evolution system allows enemies to transform and adapt:

```cpp
// Enemy evolution stage enumeration
UENUM(BlueprintType)
enum class EEnemyEvolutionStage : uint8
{
    Base        UMETA(DisplayName = "Base Form"),
    Evolved     UMETA(DisplayName = "Evolved Form"),
    Mutated     UMETA(DisplayName = "Mutated Form"),
    Ascended    UMETA(DisplayName = "Ascended Form")
};
```

Each evolution stage modifies:
- Visual appearance
- Attack capabilities
- Stat values
- AI behavior patterns
- Special abilities

### Classification Implementation

The classification system is implemented through a combination of inheritance and data-driven design:

```cpp
// Base enemy classes for each category
class GAME_API ANaytibaMinion : public ABaseEnemy { /* ... */ };
class GAME_API ANaytibaWarrior : public ABaseEnemy { /* ... */ };
class GAME_API AEliteNaytiba : public ABaseEnemy { /* ... */ };
class GAME_API AAlphaNaytiba : public ABaseEnemy { /* ... */ };
class GAME_API AElderNaytiba : public ABaseEnemy { /* ... */ };

// Data structure for classification
USTRUCT(BlueprintType)
struct GAME_API FEnemyClassification
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classification")
    EEnemyCategory Category = EEnemyCategory::NaytibaMinion;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classification")
    EEnemyThreatLevel ThreatLevel = EEnemyThreatLevel::Common;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classification")
    EEnemyCombatRole CombatRole = EEnemyCombatRole::Melee;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classification")
    EEnemyEvolutionStage EvolutionStage = EEnemyEvolutionStage::Base;
};
```

### Classification Utilities

The system includes utility functions for working with the classification system:

```cpp
// Classification utility functions
UCLASS()
class GAME_API UEnemyClassificationLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    // Get stat multiplier based on threat level
    UFUNCTION(BlueprintPure, Category = "Enemy Classification")
    static float GetThreatLevelStatMultiplier(EEnemyThreatLevel ThreatLevel);
    
    // Check if enemy is a boss type
    UFUNCTION(BlueprintPure, Category = "Enemy Classification")
    static bool IsBossEnemy(EEnemyCategory Category);
    
    // Get recommended encounter count based on category
    UFUNCTION(BlueprintPure, Category = "Enemy Classification")
    static int32 GetRecommendedEncounterCount(EEnemyCategory Category, EEnemyThreatLevel ThreatLevel);
    
    // Get visual feedback color for threat level
    UFUNCTION(BlueprintPure, Category = "Enemy Classification")
    static FLinearColor GetThreatLevelColor(EEnemyThreatLevel ThreatLevel);
};
```

## Enemy Data Structure

The enemy data structure provides a comprehensive framework for defining and managing enemy properties, abilities, and behaviors. This data-driven approach allows for easy modification and balancing without code changes.

### Core Data Structures

#### Base Enemy Data

```cpp
// Core enemy data structure
USTRUCT(BlueprintType)
struct GAME_API FEnemyData
{
    GENERATED_BODY()
    
    // Basic information
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Info")
    FText DisplayName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Info")
    FText Description;
    
    // Classification
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Info")
    FEnemyClassification Classification;
    
    // Core stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FEnemyStats BaseStats;
    
    // Combat abilities
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TArray<FEnemyAbilityData> Abilities;
    
    // AI behavior
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FEnemyBehaviorData BehaviorData;
    
    // Evolution data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evolution")
    FEnemyEvolutionData EvolutionData;
    
    // Visual and effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FEnemyVisualData VisualData;
};
```

#### Stats Structure

```cpp
// Enemy stats structure
USTRUCT(BlueprintType)
struct GAME_API FEnemyStats
{
    GENERATED_BODY()
    
    // Core stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseDamage = 10.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MoveSpeed = 300.0f;
    
    // Defense stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float PhysicalDefense = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float ElementalDefense = 0.0f;
    
    // Combat stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackSpeed = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CriticalChance = 0.05f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CriticalMultiplier = 1.5f;
    
    // AI stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AggressionLevel = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float DetectionRange = 1000.0f;
};
```

#### Ability Data

```cpp
// Enemy ability data structure
USTRUCT(BlueprintType)
struct GAME_API FEnemyAbilityData
{
    GENERATED_BODY()
    
    // Basic info
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FName AbilityID;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FText DisplayName;
    
    // Activation parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float CooldownTime = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float ActivationRange = 200.0f;
    
    // Combat properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DamageMultiplier = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    TArray<FStatusEffectData> StatusEffects;
    
    // Animation and VFX
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    UAnimMontage* AbilityMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    TArray<FAbilityVFXData> VisualEffects;
};
```

#### Behavior Data

```cpp
// Enemy behavior data structure
USTRUCT(BlueprintType)
struct GAME_API FEnemyBehaviorData
{
    GENERATED_BODY()
    
    // AI behavior tree
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* BehaviorTree;
    
    // Blackboard data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBlackboardData* BlackboardData;
    
    // Combat behavior
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float PreferredEngagementRange = 200.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float FlankingProbability = 0.3f;
    
    // Group behavior
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool bCanCoordinateWithAllies = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float GroupCoordinationRange = 500.0f;
};
```

#### Evolution Data

```cpp
// Enemy evolution data structure
USTRUCT(BlueprintType)
struct GAME_API FEnemyEvolutionData
{
    GENERATED_BODY()
    
    // Evolution requirements
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evolution")
    TArray<FEvolutionStageData> EvolutionStages;
    
    // Evolution effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evolution")
    TArray<FStatModifier> StatModifiers;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evolution")
    TArray<FEnemyAbilityData> UnlockedAbilities;
    
    // Visual evolution
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evolution")
    TArray<FEvolutionVisualData> VisualChanges;
};
```

#### Visual Data

```cpp
// Enemy visual data structure
USTRUCT(BlueprintType)
struct GAME_API FEnemyVisualData
{
    GENERATED_BODY()
    
    // Mesh and materials
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    USkeletalMesh* BaseMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TArray<UMaterialInterface*> Materials;
    
    // Animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TSubclassOf<UAnimInstance> AnimationClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TArray<UAnimMontage*> CombatMontages;
    
    // Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TArray<FEnemyVFXData> CombatEffects;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TArray<FEnemyVFXData> StatusEffects;
};
```

### Data Asset Implementation

The enemy data is managed through data assets for easy editing and organization:

```cpp
// Enemy data asset
UCLASS()
class GAME_API UEnemyDataAsset : public UDataAsset
{
    GENERATED_BODY()
    
public:
    // Core enemy data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Data")
    FEnemyData EnemyData;
    
    // Validation
    virtual void ValidateData();
    
    // Utility functions
    UFUNCTION(BlueprintPure, Category = "Enemy Data")
    float GetModifiedStat(FName StatName, float BaseValue) const;
    
    UFUNCTION(BlueprintPure, Category = "Enemy Data")
    bool CanEvolve(EEnemyEvolutionStage CurrentStage) const;
};
```

### Data Tables

Enemy data can also be managed through data tables for bulk editing and balancing:

```cpp
// Enemy data table row
USTRUCT(BlueprintType)
struct GAME_API FEnemyDataTableRow : public FTableRowBase
{
    GENERATED_BODY()
    
    // Core data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
    FEnemyData EnemyData;
    
    // Spawn settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Data")
    FEnemySpawnRules SpawnRules;
    
    // Reward data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
    FEnemyRewardData RewardData;
};
```

### Data Management Utilities

The system includes utilities for working with enemy data:

```cpp
// Enemy data management utilities
UCLASS()
class GAME_API UEnemyDataLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    // Data validation
    UFUNCTION(BlueprintCallable, Category = "Enemy Data")
    static bool ValidateEnemyData(const FEnemyData& EnemyData, FString& OutError);
    
    // Data modification
    UFUNCTION(BlueprintCallable, Category = "Enemy Data")
    static FEnemyData ApplyEvolutionModifiers(const FEnemyData& BaseData, 
                                            EEnemyEvolutionStage TargetStage);
    
    // Data queries
    UFUNCTION(BlueprintPure, Category = "Enemy Data")
    static TArray<FEnemyAbilityData> GetAvailableAbilities(const FEnemyData& EnemyData, 
                                                          EEnemyEvolutionStage Stage);
};
```

## AI System

The AI system provides sophisticated behavior control for enemies using Unreal Engine's Behavior Tree system, enhanced with custom nodes and services for our specific needs. The system is designed to create intelligent, responsive enemies that provide engaging combat encounters.

### Core AI Components

#### AI Controller

```cpp
// Base AI controller for all enemy types
UCLASS()
class GAME_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()
    
public:
    AEnemyAIController();
    
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    
    // Perception system
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* PerceptionComponent;
    
    // Combat interface
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void InitiateCombat(AActor* Target);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void EndCombat();
    
protected:
    // Behavior tree component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBehaviorTreeComponent* BehaviorTreeComponent;
    
    // Blackboard component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;
    
    // Event handlers
    virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
    virtual void OnCombatStateChanged(EEnemyCombatState NewState);
};
```

#### Perception Setup

```cpp
// Enemy perception configuration
UCLASS()
class GAME_API UEnemyPerceptionConfig : public UAISenseConfig
{
    GENERATED_BODY()
    
public:
    UEnemyPerceptionConfig();
    
    // Sight configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float SightRadius = 1500.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float LoseSightRadius = 2000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float PeripheralVisionAngleDegrees = 90.0f;
    
    // Hearing configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float HearingRange = 1000.0f;
    
    // Damage sensing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float DamageSensingRange = 500.0f;
};
```

### Behavior Tree Components

#### Custom Tasks

```cpp
// Example melee attack task
UCLASS()
class GAME_API UBTTask_MeleeAttack : public UBTTaskNode
{
    GENERATED_BODY()
    
public:
    UBTTask_MeleeAttack();
    
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, 
                                          uint8* NodeMemory) override;
    
protected:
    // Configuration
    UPROPERTY(EditAnywhere, Category = "Attack")
    float AttackRange = 200.0f;
    
    UPROPERTY(EditAnywhere, Category = "Attack")
    float AttackAngle = 45.0f;
    
    // Implementation
    virtual bool CanPerformAttack(ABaseEnemy* Enemy, AActor* Target) const;
    virtual void OnAttackFinished(ABaseEnemy* Enemy);
};

// Example ranged attack task
UCLASS()
class GAME_API UBTTask_RangedAttack : public UBTTaskNode
{
    GENERATED_BODY()
    
public:
    UBTTask_RangedAttack();
    
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, 
                                          uint8* NodeMemory) override;
    
protected:
    // Configuration
    UPROPERTY(EditAnywhere, Category = "Attack")
    float MinRange = 500.0f;
    
    UPROPERTY(EditAnywhere, Category = "Attack")
    float MaxRange = 1500.0f;
    
    // Implementation
    virtual bool FindAttackPosition(ABaseEnemy* Enemy, AActor* Target, 
                                  FVector& OutPosition);
};
```

#### Custom Services

```cpp
// Combat evaluation service
UCLASS()
class GAME_API UBTService_CombatEvaluation : public UBTService
{
    GENERATED_BODY()
    
public:
    UBTService_CombatEvaluation();
    
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, 
                         uint8* NodeMemory, 
                         float DeltaSeconds) override;
    
protected:
    // Configuration
    UPROPERTY(EditAnywhere, Category = "Combat")
    float EvaluationInterval = 0.5f;
    
    // Evaluation methods
    virtual void EvaluateCombatSituation(ABaseEnemy* Enemy, 
                                       AActor* Target, 
                                       float DeltaTime);
    
    virtual void UpdateTacticalPosition(ABaseEnemy* Enemy, 
                                      AActor* Target);
};

// Group coordination service
UCLASS()
class GAME_API UBTService_GroupCoordination : public UBTService
{
    GENERATED_BODY()
    
public:
    UBTService_GroupCoordination();
    
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, 
                         uint8* NodeMemory, 
                         float DeltaSeconds) override;
    
protected:
    // Configuration
    UPROPERTY(EditAnywhere, Category = "Coordination")
    float CoordinationRange = 500.0f;
    
    // Coordination methods
    virtual void CoordinateWithAllies(ABaseEnemy* Enemy, 
                                    const TArray<ABaseEnemy*>& NearbyAllies);
};
```

### AI Behaviors

#### Combat Behaviors

```cpp
// Combat behavior interface
UINTERFACE(MinimalAPI)
class UEnemyCombatBehavior : public UInterface
{
    GENERATED_BODY()
};

class IEnemyCombatBehavior
{
    GENERATED_BODY()
    
public:
    // Core combat methods
    virtual void InitiateCombat(AActor* Target) = 0;
    virtual void UpdateCombat(float DeltaTime) = 0;
    virtual void EndCombat() = 0;
    
    // Tactical methods
    virtual bool ShouldReposition() const = 0;
    virtual FVector GetPreferredCombatPosition() const = 0;
    virtual float GetPreferredEngagementRange() const = 0;
};
```

#### Group Behaviors

```cpp
// Group behavior interface
UINTERFACE(MinimalAPI)
class UEnemyGroupBehavior : public UInterface
{
    GENERATED_BODY()
};

class IEnemyGroupBehavior
{
    GENERATED_BODY()
    
public:
    // Group coordination
    virtual void RegisterWithGroup(AEnemyGroup* Group) = 0;
    virtual void UnregisterFromGroup() = 0;
    
    // Role management
    virtual EGroupCombatRole GetCurrentRole() const = 0;
    virtual void AssignRole(EGroupCombatRole NewRole) = 0;
    
    // Coordination
    virtual void CoordinateAttack(const FGroupAttackData& AttackData) = 0;
};
```

### Decision Making

#### Tactical Analysis

```cpp
// Tactical analysis system
UCLASS()
class GAME_API UEnemyTacticalSystem : public UObject
{
    GENERATED_BODY()
    
public:
    // Position evaluation
    UFUNCTION(BlueprintCallable, Category = "Tactical")
    float EvaluatePosition(const FVector& Position, 
                          ABaseEnemy* Enemy, 
                          AActor* Target);
    
    // Attack opportunity analysis
    UFUNCTION(BlueprintCallable, Category = "Tactical")
    bool AnalyzeAttackOpportunity(ABaseEnemy* Enemy, 
                                 AActor* Target, 
                                 FEnemyAbilityData& OutBestAbility);
    
protected:
    // Evaluation criteria
    virtual float EvaluateDistanceScore(float Distance, 
                                      float PreferredRange) const;
    
    virtual float EvaluateLineOfSightScore(const FVector& Position, 
                                         AActor* Target) const;
    
    virtual float EvaluateTerrainAdvantageScore(const FVector& Position) const;
};
```

#### Threat Assessment

```cpp
// Threat assessment system
UCLASS()
class GAME_API UEnemyThreatAssessment : public UObject
{
    GENERATED_BODY()
    
public:
    // Threat evaluation
    UFUNCTION(BlueprintCallable, Category = "Threat")
    float EvaluateTargetThreat(AActor* Target, ABaseEnemy* Enemy);
    
    // Priority targeting
    UFUNCTION(BlueprintCallable, Category = "Threat")
    AActor* SelectBestTarget(const TArray<AActor*>& PotentialTargets, 
                           ABaseEnemy* Enemy);
    
protected:
    // Assessment criteria
    virtual float CalculateDamageThreats(AActor* Target) const;
    virtual float CalculatePositionalThreat(AActor* Target, 
                                          ABaseEnemy* Enemy) const;
    virtual float CalculateStatusThreat(AActor* Target) const;
};
```

### AI Debugging

```cpp
// AI debugging system
UCLASS()
class GAME_API UEnemyAIDebugger : public UObject
{
    GENERATED_BODY()
    
public:
    // Visual debugging
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void DrawDebugInfo(ABaseEnemy* Enemy);
    
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void LogAIDecision(const FString& Decision, 
                      const FString& Reason);
    
protected:
    // Debug visualization
    virtual void DrawPerceptionInfo(ABaseEnemy* Enemy);
    virtual void DrawTacticalInfo(ABaseEnemy* Enemy);
    virtual void DrawGroupCoordinationInfo(ABaseEnemy* Enemy);
};
```

## Combat System

The combat system provides a robust framework for enemy combat mechanics, including attack patterns, damage calculation, status effects, and combat state management. The system is designed to create engaging and challenging combat encounters while maintaining flexibility and extensibility.

### Combat Components

#### Base Combat Component

```cpp
// Base combat component for all enemy types
UCLASS()
class GAME_API UEnemyCombatComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UEnemyCombatComponent();
    
    // Combat state management
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void EnterCombat(AActor* Target);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void ExitCombat();
    
    // Attack execution
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual bool ExecuteAttack(const FEnemyAbilityData& AbilityData);
    
    // Damage handling
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual float CalculateDamage(const FEnemyAbilityData& AbilityData, 
                                const FHitResult& HitResult);
    
protected:
    // Combat state
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
    EEnemyCombatState CurrentCombatState;
    
    // Active effects
    UPROPERTY(Replicated)
    TArray<FActiveStatusEffect> ActiveStatusEffects;
    
    // Combat events
    UFUNCTION()
    virtual void OnAttackLanded(const FHitResult& HitResult);
    
    UFUNCTION()
    virtual void OnStatusEffectApplied(const FStatusEffectData& EffectData);
};
```

#### Specialized Combat Components

```cpp
// Melee combat component
UCLASS()
class GAME_API UEnemyMeleeCombatComponent : public UEnemyCombatComponent
{
    GENERATED_BODY()
    
public:
    // Melee-specific attack execution
    virtual bool ExecuteAttack(const FEnemyAbilityData& AbilityData) override;
    
protected:
    // Melee combat configuration
    UPROPERTY(EditAnywhere, Category = "Melee Combat")
    float AttackRadius = 100.0f;
    
    UPROPERTY(EditAnywhere, Category = "Melee Combat")
    float AttackAngle = 45.0f;
    
    // Hit detection
    virtual bool PerformMeleeTrace(FHitResult& OutHit);
    virtual void ApplyMeleeDamage(const FHitResult& HitResult);
};

// Ranged combat component
UCLASS()
class GAME_API UEnemyRangedCombatComponent : public UEnemyCombatComponent
{
    GENERATED_BODY()
    
public:
    // Ranged-specific attack execution
    virtual bool ExecuteAttack(const FEnemyAbilityData& AbilityData) override;
    
protected:
    // Projectile configuration
    UPROPERTY(EditAnywhere, Category = "Ranged Combat")
    TSubclassOf<AEnemyProjectile> ProjectileClass;
    
    UPROPERTY(EditAnywhere, Category = "Ranged Combat")
    float ProjectileSpeed = 1000.0f;
    
    // Projectile spawning
    virtual AEnemyProjectile* SpawnProjectile(const FTransform& SpawnTransform);
};
```

### Combat States

```cpp
// Combat state enumeration
UENUM(BlueprintType)
enum class EEnemyCombatState : uint8
{
    Idle            UMETA(DisplayName = "Idle"),
    Approaching     UMETA(DisplayName = "Approaching"),
    Attacking       UMETA(DisplayName = "Attacking"),
    Recovering      UMETA(DisplayName = "Recovering"),
    Stunned         UMETA(DisplayName = "Stunned"),
    Evading         UMETA(DisplayName = "Evading"),
    Dead            UMETA(DisplayName = "Dead")
};

// Combat state manager
UCLASS()
class GAME_API UEnemyCombatStateManager : public UObject
{
    GENERATED_BODY()
    
public:
    // State management
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual bool CanTransitionTo(EEnemyCombatState NewState) const;
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void TransitionTo(EEnemyCombatState NewState);
    
protected:
    // State handlers
    virtual void OnStateEnter(EEnemyCombatState NewState);
    virtual void OnStateExit(EEnemyCombatState OldState);
    virtual void UpdateCurrentState(float DeltaTime);
};
```

### Attack System

#### Attack Data Structures

```cpp
// Attack configuration
USTRUCT(BlueprintType)
struct GAME_API FAttackConfiguration
{
    GENERATED_BODY()
    
    // Basic properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float BaseDamage = 10.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float DamageRadius = 100.0f;
    
    // Timing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float WindupTime = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float ActiveTime = 0.2f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float RecoveryTime = 0.3f;
    
    // Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TArray<FStatusEffectData> StatusEffects;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TArray<FAttackVFXData> VisualEffects;
};

// Attack phase tracking
USTRUCT(BlueprintType)
struct GAME_API FAttackPhase
{
    GENERATED_BODY()
    
    // Phase timing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
    float PhaseStartTime = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
    float PhaseDuration = 0.0f;
    
    // Phase properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
    bool bCanBeCancelled = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
    bool bIsInvulnerable = false;
};
```

#### Attack Execution

```cpp
// Attack execution manager
UCLASS()
class GAME_API UEnemyAttackManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Attack execution
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual bool StartAttack(const FAttackConfiguration& AttackConfig);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void UpdateAttack(float DeltaTime);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void CancelAttack();
    
protected:
    // Phase management
    virtual void EnterPhase(EAttackPhase NewPhase);
    virtual void UpdatePhase(float DeltaTime);
    virtual void ExitPhase(EAttackPhase OldPhase);
    
    // Hit detection
    virtual void CheckHits();
    virtual void ApplyHitEffects(const FHitResult& HitResult);
};
```

### Damage System

```cpp
// Damage type definition
USTRUCT(BlueprintType)
struct GAME_API FEnemyDamageData
{
    GENERATED_BODY()
    
    // Damage properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Amount = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EDamageType DamageType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    FVector HitDirection;
    
    // Additional effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float StaggerAmount = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    TArray<FStatusEffectData> StatusEffects;
};

// Damage calculation system
UCLASS()
class GAME_API UEnemyDamageCalculator : public UObject
{
    GENERATED_BODY()
    
public:
    // Damage calculation
    UFUNCTION(BlueprintCallable, Category = "Combat")
    float CalculateFinalDamage(const FEnemyDamageData& DamageData, 
                              const FEnemyStats& DefenderStats);
    
protected:
    // Calculation components
    virtual float ApplyDamageModifiers(float BaseDamage, 
                                     EDamageType DamageType, 
                                     const FEnemyStats& DefenderStats);
    
    virtual float CalculateCriticalHit(float Damage, 
                                     const FEnemyStats& AttackerStats);
};
```

### Status Effect System

```cpp
// Status effect definition
USTRUCT(BlueprintType)
struct GAME_API FStatusEffectData
{
    GENERATED_BODY()
    
    // Effect properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    EStatusEffectType EffectType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    float Duration = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    float Magnitude = 1.0f;
    
    // Visual feedback
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    UParticleSystem* EffectParticles;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
    USoundBase* EffectSound;
};

// Status effect manager
UCLASS()
class GAME_API UEnemyStatusEffectManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Effect management
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual bool ApplyStatusEffect(const FStatusEffectData& EffectData);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void UpdateStatusEffects(float DeltaTime);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void RemoveStatusEffect(EStatusEffectType EffectType);
    
protected:
    // Effect processing
    virtual void ProcessEffectStack(const FStatusEffectData& NewEffect);
    virtual void UpdateEffectDurations(float DeltaTime);
    virtual void ApplyEffectTick(const FActiveStatusEffect& Effect, float DeltaTime);
};
```

### Combat Feedback

```cpp
// Combat feedback manager
UCLASS()
class GAME_API UEnemyCombatFeedback : public UObject
{
    GENERATED_BODY()
    
public:
    // Visual feedback
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void PlayHitFeedback(const FHitResult& HitResult);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void PlayAttackFeedback(const FAttackConfiguration& AttackConfig);
    
    // Sound feedback
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void PlayCombatSound(USoundBase* Sound, 
                               const FVector& Location);
    
protected:
    // Feedback implementation
    virtual void SpawnHitParticles(const FHitResult& HitResult);
    virtual void PlayHitSound(const FHitResult& HitResult);
    virtual void ApplyHitCameraShake(const FHitResult& HitResult);
};
```

## Animation System

The animation system provides a sophisticated framework for managing enemy animations, including combat moves, state transitions, and procedural animation effects. The system is designed to create fluid, responsive animations while maintaining performance and network efficiency.

### Animation Components

#### Base Animation Component

```cpp
// Base animation component for all enemy types
UCLASS()
class GAME_API UEnemyAnimationComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UEnemyAnimationComponent();
    
    // Animation state management
    UFUNCTION(BlueprintCallable, Category = "Animation")
    virtual void PlayAnimationState(EEnemyAnimationState NewState);
    
    UFUNCTION(BlueprintCallable, Category = "Animation")
    virtual void PlayCombatMontage(const FEnemyAbilityData& AbilityData);
    
    // Animation events
    UFUNCTION()
    virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    
protected:
    // Animation instance
    UPROPERTY()
    UEnemyAnimInstance* AnimInstance;
    
    // Current state
    UPROPERTY(Replicated)
    EEnemyAnimationState CurrentAnimState;
    
    // Network handling
    virtual void OnRep_AnimationState();
};
```

### Animation States

```cpp
// Animation state enumeration
UENUM(BlueprintType)
enum class EEnemyAnimationState : uint8
{
    Idle                UMETA(DisplayName = "Idle"),
    Locomotion          UMETA(DisplayName = "Locomotion"),
    CombatIdle          UMETA(DisplayName = "Combat Idle"),
    CombatLocomotion    UMETA(DisplayName = "Combat Locomotion"),
    Attacking           UMETA(DisplayName = "Attacking"),
    Hit                 UMETA(DisplayName = "Hit"),
    Death               UMETA(DisplayName = "Death")
};

// Animation state data
USTRUCT(BlueprintType)
struct GAME_API FEnemyAnimationStateData
{
    GENERATED_BODY()
    
    // State properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    EEnemyAnimationState State;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimSequence* Animation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float BlendTime = 0.25f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    bool bLooping = true;
};
```

### Animation Instance

```cpp
// Enemy animation instance
UCLASS()
class GAME_API UEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
    
public:
    UEnemyAnimInstance();
    
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    
protected:
    // Animation properties
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    float Direction = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bInCombat = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsFalling = false;
    
    // State machine
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    UAnimStateMachine* MainStateMachine;
    
    // Update functions
    virtual void UpdateMovementProperties();
    virtual void UpdateCombatState();
};
```

### Combat Animation System

#### Combat Montages

```cpp
// Combat montage data
USTRUCT(BlueprintType)
struct GAME_API FEnemyCombatMontageData
{
    GENERATED_BODY()
    
    // Montage properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Animation")
    UAnimMontage* Montage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Animation")
    float PlayRate = 1.0f;
    
    // Section data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Animation")
    FName StartSection = NAME_None;
    
    // Blend settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Animation")
    float BlendInTime = 0.25f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Animation")
    float BlendOutTime = 0.25f;
};

// Combat animation manager
UCLASS()
class GAME_API UEnemyCombatAnimationManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Montage playback
    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    virtual bool PlayAttackMontage(const FEnemyAbilityData& AbilityData);
    
    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    virtual bool PlayHitReactionMontage(const FDamageEvent& DamageEvent);
    
protected:
    // Montage selection
    virtual UAnimMontage* SelectAttackMontage(const FEnemyAbilityData& AbilityData);
    virtual UAnimMontage* SelectHitReactionMontage(const FDamageEvent& DamageEvent);
};
```

### Procedural Animation

```cpp
// Procedural animation component
UCLASS()
class GAME_API UEnemyProceduralAnimationComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    // Procedural effects
    UFUNCTION(BlueprintCallable, Category = "Procedural Animation")
    virtual void ApplyHitReaction(const FVector& HitDirection, float Intensity);
    
    UFUNCTION(BlueprintCallable, Category = "Procedural Animation")
    virtual void ApplyWindEffect(const FVector& WindDirection, float Strength);
    
protected:
    // Bone modification
    virtual void ModifyBone(FName BoneName, const FTransform& Modification);
    virtual void ResetBoneModification(FName BoneName);
    
    // Physics simulation
    virtual void SimulatePhysics(float DeltaTime);
};

// Bone dynamics system
UCLASS()
class GAME_API UEnemyBoneDynamicsSystem : public UObject
{
    GENERATED_BODY()
    
public:
    // Dynamics setup
    UFUNCTION(BlueprintCallable, Category = "Bone Dynamics")
    virtual void SetupDynamicBone(FName BoneName, const FBoneDynamicsSettings& Settings);
    
    // Simulation
    virtual void SimulateDynamics(float DeltaTime);
    
protected:
    // Dynamics data
    UPROPERTY()
    TMap<FName, FBoneDynamicsState> BoneStates;
    
    // Physics calculations
    virtual void UpdateBonePhysics(FName BoneName, float DeltaTime);
    virtual void ApplyConstraints(FName BoneName);
};
```

### Animation Networking

```cpp
// Animation networking component
UCLASS()
class GAME_API UEnemyAnimationNetworking : public UActorComponent
{
    GENERATED_BODY()
    
public:
    // Network replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // Animation sync
    UFUNCTION(Server, Reliable)
    virtual void ServerPlayMontage(UAnimMontage* Montage, float PlayRate = 1.0f);
    
    UFUNCTION(NetMulticast, Reliable)
    virtual void MulticastPlayMontage(UAnimMontage* Montage, float PlayRate = 1.0f);
    
protected:
    // Sync data
    UPROPERTY(ReplicatedUsing = OnRep_CurrentMontage)
    FRepAnimMontageInfo RepMontageInfo;
    
    // Network handlers
    UFUNCTION()
    virtual void OnRep_CurrentMontage();
};
```

### Animation Debug Tools

```cpp
// Animation debug system
UCLASS()
class GAME_API UEnemyAnimationDebugger : public UObject
{
    GENERATED_BODY()
    
public:
    // Debug visualization
    UFUNCTION(BlueprintCallable, Category = "Animation Debug")
    virtual void DrawAnimationDebug();
    
    UFUNCTION(BlueprintCallable, Category = "Animation Debug")
    virtual void LogAnimationState(const FString& Context);
    
protected:
    // Debug helpers
    virtual void DrawSkeleton();
    virtual void DrawAnimationCurves();
    virtual void DrawStateMachine();
};
```

### Animation Optimization

```cpp
// Animation optimization system
UCLASS()
class GAME_API UEnemyAnimationOptimizer : public UObject
{
    GENERATED_BODY()
    
public:
    // LOD management
    UFUNCTION(BlueprintCallable, Category = "Animation Optimization")
    virtual void UpdateAnimationLOD(float DistanceToCamera);
    
    // Performance monitoring
    UFUNCTION(BlueprintCallable, Category = "Animation Optimization")
    virtual void MonitorAnimationPerformance();
    
protected:
    // Optimization strategies
    virtual void AdjustTickRate(float DistanceToCamera);
    virtual void SimplifyAnimationGraphs(int32 LODLevel);
    virtual void OptimizeBoneTransforms(int32 LODLevel);
};
```

## Visual Effects System

The visual effects system provides a comprehensive framework for managing enemy-related visual effects, including combat impacts, status effects, environmental interactions, and special abilities. The system is designed to create visually stunning and performance-optimized effects that enhance gameplay feedback.

### VFX Components

#### Base VFX Component

```cpp
// Base VFX component for all enemy types
UCLASS()
class GAME_API UEnemyVFXComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UEnemyVFXComponent();
    
    // Effect management
    UFUNCTION(BlueprintCallable, Category = "VFX")
    virtual void PlayEffect(const FEnemyVFXData& EffectData);
    
    UFUNCTION(BlueprintCallable, Category = "VFX")
    virtual void StopEffect(FName EffectID);
    
    // Effect events
    UFUNCTION()
    virtual void OnEffectComplete(UParticleSystemComponent* FinishedComponent);
    
protected:
    // Active effects
    UPROPERTY()
    TMap<FName, UParticleSystemComponent*> ActiveEffects;
    
    // Effect pooling
    UPROPERTY()
    TArray<UParticleSystemComponent*> EffectPool;
    
    // Effect management
    virtual UParticleSystemComponent* GetEffectFromPool();
    virtual void ReturnEffectToPool(UParticleSystemComponent* Effect);
};
```

### Effect Data Structures

```cpp
// VFX data structure
USTRUCT(BlueprintType)
struct GAME_API FEnemyVFXData
{
    GENERATED_BODY()
    
    // Effect identification
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    FName EffectID;
    
    // Visual components
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UParticleSystem* ParticleSystem;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* NiagaraSystem;
    
    // Material effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UMaterialInterface* DecalMaterial;
    
    // Effect parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    float Duration = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    FVector Scale = FVector(1.0f);
    
    // Attachment
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    FName AttachSocketName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    bool bAttachToSocket = false;
};
```

### Combat VFX System

```cpp
// Combat VFX manager
UCLASS()
class GAME_API UEnemyCombatVFXManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Combat effect playback
    UFUNCTION(BlueprintCallable, Category = "Combat VFX")
    virtual void PlayAttackEffect(const FEnemyAbilityData& AbilityData, 
                                const FVector& Location);
    
    UFUNCTION(BlueprintCallable, Category = "Combat VFX")
    virtual void PlayImpactEffect(const FHitResult& HitResult, 
                                const FEnemyDamageData& DamageData);
    
protected:
    // Effect selection
    virtual FEnemyVFXData SelectAttackEffect(const FEnemyAbilityData& AbilityData);
    virtual FEnemyVFXData SelectImpactEffect(const FEnemyDamageData& DamageData);
    
    // Effect placement
    virtual FTransform CalculateEffectTransform(const FVector& Location, 
                                              const FVector& Normal);
};
```

### Status Effect VFX

```cpp
// Status effect VFX manager
UCLASS()
class GAME_API UEnemyStatusVFXManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Status effect visualization
    UFUNCTION(BlueprintCallable, Category = "Status VFX")
    virtual void ApplyStatusEffect(const FStatusEffectData& EffectData);
    
    UFUNCTION(BlueprintCallable, Category = "Status VFX")
    virtual void UpdateStatusEffects(float DeltaTime);
    
    UFUNCTION(BlueprintCallable, Category = "Status VFX")
    virtual void RemoveStatusEffect(EStatusEffectType EffectType);
    
protected:
    // Effect management
    UPROPERTY()
    TMap<EStatusEffectType, FActiveVFXData> ActiveStatusEffects;
    
    // Effect updates
    virtual void UpdateEffectIntensity(EStatusEffectType EffectType, 
                                     float Intensity);
};
```

### Environmental Interaction VFX

```cpp
// Environmental VFX manager
UCLASS()
class GAME_API UEnemyEnvironmentVFXManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Environmental effects
    UFUNCTION(BlueprintCallable, Category = "Environment VFX")
    virtual void PlayFootstepEffect(EPhysicalSurface SurfaceType, 
                                  const FVector& Location);
    
    UFUNCTION(BlueprintCallable, Category = "Environment VFX")
    virtual void PlayWeatherEffect(const FWeatherEffectData& WeatherData);
    
protected:
    // Surface interaction
    virtual FEnemyVFXData GetSurfaceEffect(EPhysicalSurface SurfaceType);
    
    // Weather effects
    UPROPERTY()
    TArray<UParticleSystemComponent*> ActiveWeatherEffects;
};
```

### Special Ability VFX

```cpp
// Special ability VFX manager
UCLASS()
class GAME_API UEnemyAbilityVFXManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Ability visualization
    UFUNCTION(BlueprintCallable, Category = "Ability VFX")
    virtual void PlayAbilityStartEffect(const FEnemyAbilityData& AbilityData);
    
    UFUNCTION(BlueprintCallable, Category = "Ability VFX")
    virtual void UpdateAbilityEffect(const FEnemyAbilityData& AbilityData, 
                                   float Progress);
    
    UFUNCTION(BlueprintCallable, Category = "Ability VFX")
    virtual void PlayAbilityEndEffect(const FEnemyAbilityData& AbilityData);
    
protected:
    // Effect tracking
    UPROPERTY()
    TMap<FName, FActiveAbilityVFXData> ActiveAbilityEffects;
    
    // Effect chain management
    virtual void ChainEffects(const TArray<FEnemyVFXData>& EffectChain);
};
```

### VFX Optimization

```cpp
// VFX optimization system
UCLASS()
class GAME_API UEnemyVFXOptimizer : public UObject
{
    GENERATED_BODY()
    
public:
    // Performance management
    UFUNCTION(BlueprintCallable, Category = "VFX Optimization")
    virtual void UpdateEffectLOD(float DistanceToCamera);
    
    UFUNCTION(BlueprintCallable, Category = "VFX Optimization")
    virtual void ManageEffectBudget();
    
protected:
    // LOD settings
    UPROPERTY()
    TArray<FVFXLODSettings> LODLevels;
    
    // Budget management
    virtual void PrioritizeEffects();
    virtual void CullLowPriorityEffects();
};

// LOD settings structure
USTRUCT(BlueprintType)
struct GAME_API FVFXLODSettings
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX LOD")
    float MaxParticleCount;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX LOD")
    float SimulationRate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX LOD")
    bool bDisableCollision;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX LOD")
    bool bSimplifiedPhysics;
};
```

### VFX Networking

```cpp
// VFX networking component
UCLASS()
class GAME_API UEnemyVFXNetworking : public UActorComponent
{
    GENERATED_BODY()
    
public:
    // Network replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // Effect synchronization
    UFUNCTION(NetMulticast, Reliable)
    virtual void MulticastPlayEffect(const FEnemyVFXData& EffectData, 
                                   const FTransform& Transform);
    
protected:
    // Networked effect data
    UPROPERTY(Replicated)
    TArray<FNetworkedVFXData> NetworkedEffects;
    
    // Network optimization
    virtual bool ShouldReplicateEffect(const FEnemyVFXData& EffectData);
};
```

### VFX Debug Tools

```cpp
// VFX debug system
UCLASS()
class GAME_API UEnemyVFXDebugger : public UObject
{
    GENERATED_BODY()
    
public:
    // Debug visualization
    UFUNCTION(BlueprintCallable, Category = "VFX Debug")
    virtual void DrawVFXDebug();
    
    UFUNCTION(BlueprintCallable, Category = "VFX Debug")
    virtual void LogEffectStats();
    
protected:
    // Debug helpers
    virtual void DrawEffectBounds();
    virtual void DrawParticleCount();
    virtual void DrawPerformanceMetrics();
};
```

## Designer Tools Implementation

The Designer Tools Implementation provides a comprehensive suite of tools and interfaces for game designers to efficiently create, manage, and balance enemy content. These tools are designed to streamline the workflow while maintaining consistency and quality.

### Enemy Creation Dashboard

```cpp
// Main dashboard for enemy creation and management
UCLASS()
class GAME_API UEnemyDesignerDashboard : public UEditorUtilityWidget
{
    GENERATED_BODY()
    
public:
    // Creation interface
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void CreateNewEnemy(const FString& EnemyName, EEnemyCategory Category);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void DuplicateEnemy(const FString& SourceEnemyName, const FString& NewEnemyName);
    
    // Asset management
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void SaveEnemyData();
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void LoadEnemyData(const FString& EnemyName);
    
protected:
    // UI components
    UPROPERTY()
    UEnemyPropertyEditor* PropertyEditor;
    
    UPROPERTY()
    UEnemyPreviewViewport* PreviewViewport;
    
    // Data validation
    virtual bool ValidateEnemyData(const FEnemyData& Data, FString& OutError);
};
```

### Property Editors

```cpp
// Custom property editor for enemy data
UCLASS()
class GAME_API UEnemyPropertyEditor : public UObject
{
    GENERATED_BODY()
    
public:
    // Property editing
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void EditBaseProperties(FEnemyData& EnemyData);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void EditCombatProperties(FEnemyData& EnemyData);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void EditAIProperties(FEnemyData& EnemyData);
    
protected:
    // Property validation
    virtual bool ValidateProperty(const FProperty* Property, const void* PropertyValue);
    
    // UI helpers
    virtual void CreatePropertyWidget(const FProperty* Property);
};
```

### Preview System

```cpp
// Enemy preview viewport
UCLASS()
class GAME_API UEnemyPreviewViewport : public UEditorViewportClient
{
    GENERATED_BODY()
    
public:
    // Preview controls
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void SpawnPreviewEnemy(const FEnemyData& EnemyData);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void PlayAnimation(const FString& AnimationName);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void PreviewEffect(const FEnemyVFXData& EffectData);
    
protected:
    // Preview actor
    UPROPERTY()
    ABaseEnemy* PreviewActor;
    
    // Camera controls
    virtual void UpdateCamera(float DeltaTime);
};
```

### Balance Tools

```cpp
// Enemy balance analysis tool
UCLASS()
class GAME_API UEnemyBalanceAnalyzer : public UObject
{
    GENERATED_BODY()
    
public:
    // Analysis functions
    UFUNCTION(BlueprintCallable, Category = "Enemy Balance")
    virtual FBalanceReport AnalyzeEnemy(const FEnemyData& EnemyData);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Balance")
    virtual void CompareWithSimilarEnemies(const FEnemyData& EnemyData);
    
    // Balance suggestions
    UFUNCTION(BlueprintCallable, Category = "Enemy Balance")
    virtual TArray<FBalanceSuggestion> GenerateBalanceSuggestions(const FBalanceReport& Report);
    
protected:
    // Analysis metrics
    virtual float CalculateDPS(const FEnemyData& EnemyData);
    virtual float CalculateEffectiveHealth(const FEnemyData& EnemyData);
    virtual float CalculateThreatLevel(const FEnemyData& EnemyData);
};
```

### Encounter Designer

```cpp
// Enemy encounter design tool
UCLASS()
class GAME_API UEnemyEncounterDesigner : public UEditorUtilityWidget
{
    GENERATED_BODY()
    
public:
    // Encounter creation
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void CreateEncounter(const FString& EncounterName);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void AddEnemyToEncounter(const FString& EnemyName, int32 Count);
    
    // Encounter analysis
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual FEncounterDifficultyReport AnalyzeEncounterDifficulty();
    
protected:
    // Encounter data
    UPROPERTY()
    FEnemyEncounterData CurrentEncounter;
    
    // Difficulty calculation
    virtual float CalculateEncounterDifficulty();
};
```

### AI Behavior Editor

```cpp
// AI behavior editing tool
UCLASS()
class GAME_API UEnemyAIBehaviorEditor : public UEditorUtilityWidget
{
    GENERATED_BODY()
    
public:
    // Behavior editing
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void EditBehaviorTree(UBehaviorTree* BehaviorTree);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void EditBlackboardData(UBlackboardData* BlackboardData);
    
    // Behavior testing
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void TestBehavior(const FEnemyData& EnemyData);
    
protected:
    // Test environment
    UPROPERTY()
    UEnemyAITestEnvironment* TestEnvironment;
    
    // Behavior validation
    virtual bool ValidateBehaviorTree(const UBehaviorTree* BehaviorTree);
};
```

### Animation Tools

```cpp
// Animation editing and preview tools
UCLASS()
class GAME_API UEnemyAnimationTools : public UEditorUtilityWidget
{
    GENERATED_BODY()
    
public:
    // Animation editing
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void EditAnimationSequence(UAnimSequence* Animation);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void EditAnimMontage(UAnimMontage* Montage);
    
    // Animation preview
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void PreviewAnimation(UAnimSequence* Animation);
    
protected:
    // Preview controls
    UPROPERTY()
    UAnimationPreviewController* PreviewController;
    
    // Animation validation
    virtual bool ValidateAnimation(const UAnimSequence* Animation);
};
```

### VFX Editor

```cpp
// Visual effects editing tool
UCLASS()
class GAME_API UEnemyVFXEditor : public UEditorUtilityWidget
{
    GENERATED_BODY()
    
public:
    // Effect editing
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void EditEffect(FEnemyVFXData& EffectData);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void CreateEffectChain(TArray<FEnemyVFXData>& EffectChain);
    
    // Effect preview
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void PreviewEffect(const FEnemyVFXData& EffectData);
    
protected:
    // Preview system
    UPROPERTY()
    UVFXPreviewSystem* PreviewSystem;
    
    // Effect validation
    virtual bool ValidateEffect(const FEnemyVFXData& EffectData);
};
```

### Data Management

```cpp
// Enemy data management system
UCLASS()
class GAME_API UEnemyDataManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Data operations
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void ImportEnemyData(const FString& FilePath);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void ExportEnemyData(const FString& FilePath);
    
    // Data validation
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual bool ValidateAllEnemyData(TArray<FString>& OutErrors);
    
protected:
    // Data storage
    UPROPERTY()
    TMap<FString, FEnemyData> EnemyDataCache;
    
    // Version control
    virtual void TrackDataChanges(const FString& EnemyName);
};
```

### Best Practices Implementation

```cpp
// Best practices enforcement system
UCLASS()
class GAME_API UEnemyDesignBestPractices : public UObject
{
    GENERATED_BODY()
    
public:
    // Validation
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual bool ValidateAgainstBestPractices(const FEnemyData& EnemyData, 
                                            TArray<FString>& OutViolations);
    
    // Guidelines
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void ShowBestPracticesGuidelines();
    
protected:
    // Validation rules
    virtual bool CheckNamingConventions(const FString& Name);
    virtual bool CheckBalanceGuidelines(const FEnemyData& EnemyData);
    virtual bool CheckPerformanceGuidelines(const FEnemyData& EnemyData);
};
```

### Designer Workflow Automation

```cpp
// Workflow automation tools
UCLASS()
class GAME_API UEnemyDesignerWorkflow : public UObject
{
    GENERATED_BODY()
    
public:
    // Workflow automation
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void AutomateSetup(const FString& EnemyName);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Design")
    virtual void BatchUpdateEnemies(const TArray<FString>& EnemyNames);
    
protected:
    // Automation tasks
    virtual void CreateRequiredAssets(const FString& EnemyName);
    virtual void SetupDefaultProperties(FEnemyData& EnemyData);
    virtual void ConfigureReferences(FEnemyData& EnemyData);
};
```

## Balancing & Testing Tools

The balancing and testing tools provide a comprehensive suite of tools and interfaces for game designers to efficiently balance and test enemy content. These tools are designed to streamline the workflow while maintaining consistency and quality.

### Balance Analysis System

```cpp
UCLASS()
class GAME_API UEnemyBalanceAnalyzer : public UObject
{
    GENERATED_BODY()

public:
    // Core balance analysis functions
    UFUNCTION(BlueprintCallable, Category = "Enemy Balance")
    FBalanceReport AnalyzeEnemyBalance(const FEnemyData& EnemyData);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Balance")
    FDifficultyScore CalculateDifficultyScore(const FEnemyData& EnemyData);
    
    // Comparative analysis
    UFUNCTION(BlueprintCallable, Category = "Enemy Balance")
    FComparisonReport CompareWithSimilarEnemies(const FEnemyData& EnemyData);
    
    // Balance recommendations
    UFUNCTION(BlueprintCallable, Category = "Enemy Balance")
    TArray<FBalanceRecommendation> GenerateBalanceSuggestions(const FEnemyData& EnemyData);

protected:
    // Analysis metrics
    virtual float CalculateDPS(const FEnemyStats& Stats);
    virtual float CalculateEffectiveHealth(const FEnemyStats& Stats);
    virtual float EvaluateAIDifficulty(const FEnemyBehaviorData& BehaviorData);
};
```

### Testing Framework

```cpp
UCLASS()
class GAME_API UEnemyTestSuite : public UObject
{
    GENERATED_BODY()

public:
    // Combat testing
    UFUNCTION(BlueprintCallable, Category = "Enemy Testing")
    FTestResults RunCombatSimulation(const FEnemyData& EnemyData);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Testing")
    FTestResults ValidateAttackPatterns(const FEnemyData& EnemyData);
    
    // AI testing
    UFUNCTION(BlueprintCallable, Category = "Enemy Testing")
    FTestResults ValidateAIBehavior(const FEnemyData& EnemyData);
    
    // Performance testing
    UFUNCTION(BlueprintCallable, Category = "Enemy Testing")
    FPerformanceReport RunPerformanceTests(const FEnemyData& EnemyData);

protected:
    // Test utilities
    virtual void SimulateCombatScenario(const FEnemyData& EnemyData);
    virtual void ValidateAnimations(const FEnemyData& EnemyData);
    virtual void CheckResourceUsage(const FEnemyData& EnemyData);
};
```

### Encounter Balance Tools

```cpp
UCLASS()
class GAME_API UEncounterBalancer : public UObject
{
    GENERATED_BODY()

public:
    // Encounter analysis
    UFUNCTION(BlueprintCallable, Category = "Encounter Balance")
    FEncounterReport AnalyzeEncounter(const TArray<FEnemyData>& Enemies);
    
    UFUNCTION(BlueprintCallable, Category = "Encounter Balance")
    float CalculateEncounterDifficulty(const TArray<FEnemyData>& Enemies);
    
    // Group composition
    UFUNCTION(BlueprintCallable, Category = "Encounter Balance")
    TArray<FCompositionSuggestion> SuggestGroupComposition(const TArray<FEnemyData>& Enemies);

protected:
    // Analysis helpers
    virtual float EvaluateGroupSynergy(const TArray<FEnemyData>& Enemies);
    virtual float CalculateTotalThreat(const TArray<FEnemyData>& Enemies);
};
```

### Data Structures

```cpp
// Balance report structure
USTRUCT(BlueprintType)
struct FBalanceReport
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DPSScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SurvivabilityScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AIDifficultyScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Warnings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Recommendations;
};

// Test results structure
USTRUCT(BlueprintType)
struct FTestResults
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPassedAllTests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> FailedTests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Warnings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DetailedReport;
};
```

### Best Practices Implementation

```cpp
UCLASS()
class GAME_API UEnemyBalancingBestPractices : public UObject
{
    GENERATED_BODY()

public:
    // Validation functions
    UFUNCTION(BlueprintCallable, Category = "Balance Best Practices")
    bool ValidateAgainstBestPractices(const FEnemyData& EnemyData);
    
    UFUNCTION(BlueprintCallable, Category = "Balance Best Practices")
    TArray<FString> GetBestPracticeViolations(const FEnemyData& EnemyData);
    
    // Guidelines enforcement
    UFUNCTION(BlueprintCallable, Category = "Balance Best Practices")
    void EnforceBestPractices(UPARAM(ref) FEnemyData& EnemyData);

protected:
    // Validation helpers
    virtual bool CheckStatRanges(const FEnemyStats& Stats);
    virtual bool ValidateAbilityBalance(const TArray<FEnemyAbilityData>& Abilities);
    virtual bool CheckAIComplexity(const FEnemyBehaviorData& BehaviorData);
};
```

The Balancing & Testing Tools section provides a comprehensive framework for:

1. **Balance Analysis System**
   - Enemy power level evaluation
   - Comparative analysis with similar enemies
   - Balance recommendations generation

2. **Testing Framework**
   - Combat simulation and validation
   - AI behavior testing
   - Performance impact assessment

3. **Encounter Balance Tools**
   - Group composition analysis
   - Difficulty scaling evaluation
   - Synergy assessment

4. **Best Practices Implementation**
   - Automated validation against guidelines
   - Balance violation detection
   - Enforcement of design standards

These tools help ensure that enemies are well-balanced, properly tested, and adhere to the game's design standards.

## Performance Optimization

The performance optimization system provides a comprehensive framework for managing enemy performance across different scenarios and hardware configurations. This system ensures optimal performance while maintaining visual quality and gameplay responsiveness.

### LOD System

```cpp
// Enemy LOD manager
UCLASS()
class GAME_API UEnemyLODManager : public UObject
{
    GENERATED_BODY()
    
public:
    // LOD management
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void UpdateLODLevel(float DistanceToCamera);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void ConfigureLODSettings(const FEnemyLODSettings& Settings);
    
protected:
    // LOD data
    UPROPERTY()
    TArray<FEnemyLODLevel> LODLevels;
    
    // LOD transitions
    virtual void TransitionToLOD(int32 NewLODLevel);
    virtual void UpdateMeshLOD(int32 LODLevel);
    virtual void UpdateAnimationLOD(int32 LODLevel);
};

// LOD configuration structures
USTRUCT(BlueprintType)
struct FEnemyLODSettings
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    float LODDistanceFactors[MAX_LOD_LEVELS];
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    bool bUseDynamicLOD = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    float UpdateFrequency = 0.5f;
};
```

### Culling System

```cpp
// Enemy culling manager
UCLASS()
class GAME_API UEnemyCullingManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Culling management
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void UpdateCulling(const FVector& ViewLocation);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void ConfigureCullingSettings(const FEnemyCullingSettings& Settings);
    
protected:
    // Culling data
    UPROPERTY()
    TArray<FCullingGroup> CullingGroups;
    
    // Culling strategies
    virtual bool ShouldCullEnemy(ABaseEnemy* Enemy, const FVector& ViewLocation);
    virtual void OptimizeDistantEnemies(const FVector& ViewLocation);
};

// Culling configuration
USTRUCT(BlueprintType)
struct FEnemyCullingSettings
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Culling")
    float CullingDistance = 10000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Culling")
    bool bUseOcclusionCulling = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Culling")
    float CullingUpdateFrequency = 0.1f;
};
```

### Resource Management

```cpp
// Enemy resource manager
UCLASS()
class GAME_API UEnemyResourceManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Resource tracking
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void TrackResourceUsage(ABaseEnemy* Enemy);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual FResourceMetrics GetResourceMetrics();
    
protected:
    // Resource pools
    UPROPERTY()
    TArray<FEnemyResourcePool> ResourcePools;
    
    // Resource optimization
    virtual void OptimizeMemoryUsage();
    virtual void OptimizeCPUUsage();
    virtual void OptimizeGPUUsage();
};

// Resource metrics structure
USTRUCT(BlueprintType)
struct FResourceMetrics
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
    float MemoryUsage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
    float CPUUsage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
    float GPUUsage;
};
```

### Performance Profiling

```cpp
// Enemy performance profiler
UCLASS()
class GAME_API UEnemyPerformanceProfiler : public UObject
{
    GENERATED_BODY()
    
public:
    // Profiling functions
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void StartProfiling(ABaseEnemy* Enemy);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual FPerformanceReport GenerateReport();
    
protected:
    // Profiling metrics
    virtual void TrackAnimationPerformance();
    virtual void TrackAIPerformance();
    virtual void TrackVFXPerformance();
    
    // Report generation
    virtual void AnalyzeBottlenecks();
    virtual void GenerateOptimizationSuggestions();
};

// Performance report structure
USTRUCT(BlueprintType)
struct FPerformanceReport
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    TArray<FPerformanceMetric> Metrics;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    TArray<FString> Bottlenecks;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    TArray<FString> OptimizationSuggestions;
};
```

### Instance Management

```cpp
// Enemy instance manager
UCLASS()
class GAME_API UEnemyInstanceManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Instance management
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void ManageInstances(const TArray<ABaseEnemy*>& Enemies);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void OptimizeInstancing();
    
protected:
    // Instance groups
    UPROPERTY()
    TArray<FInstanceGroup> InstanceGroups;
    
    // Instancing strategies
    virtual void CreateInstanceGroup(const TArray<ABaseEnemy*>& SimilarEnemies);
    virtual void UpdateInstanceTransforms();
};

// Instance group structure
USTRUCT(BlueprintType)
struct FInstanceGroup
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instancing")
    UInstancedStaticMeshComponent* MeshComponent;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instancing")
    TArray<FTransform> InstanceTransforms;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instancing")
    TArray<FInstanceData> InstanceData;
};
```

### Memory Optimization

```cpp
// Enemy memory optimizer
UCLASS()
class GAME_API UEnemyMemoryOptimizer : public UObject
{
    GENERATED_BODY()
    
public:
    // Memory management
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual void OptimizeMemoryUsage(ABaseEnemy* Enemy);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    virtual FMemoryStats GetMemoryStats();
    
protected:
    // Optimization strategies
    virtual void CompressAssets();
    virtual void ManageTextureMemory();
    virtual void OptimizeMeshMemory();
    
    // Memory tracking
    virtual void TrackMemoryAllocation();
    virtual void MonitorMemoryLeaks();
};

// Memory statistics structure
USTRUCT(BlueprintType)
struct FMemoryStats
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float TextureMemory;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float MeshMemory;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float AnimationMemory;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float AIMemory;
};
```

The Performance Optimization section provides comprehensive systems for:

1. **LOD System**
   - Dynamic level of detail management
   - Distance-based optimization
   - Visual quality scaling

2. **Culling System**
   - Visibility culling
   - Distance culling
   - Occlusion optimization

3. **Resource Management**
   - Memory usage tracking
   - CPU optimization
   - GPU resource management

4. **Performance Profiling**
   - Real-time performance monitoring
   - Bottleneck detection
   - Optimization suggestions

5. **Instance Management**
   - Mesh instancing
   - Animation instancing
   - Transform optimization

6. **Memory Optimization**
   - Asset compression
   - Memory tracking
   - Resource cleanup

These systems work together to ensure optimal performance while maintaining visual quality and gameplay responsiveness.

## Blueprint Integration

The Blueprint Integration system provides a comprehensive framework for exposing enemy system functionality to Blueprints, enabling designers to create and modify enemy behavior without direct C++ programming.

### Core Blueprint Interface

```cpp
// Base enemy blueprint interface
UINTERFACE(MinimalAPI, Blueprintable)
class UEnemyBlueprintInterface : public UInterface
{
    GENERATED_BODY()
};

class IEnemyBlueprintInterface
{
    GENERATED_BODY()
    
public:
    // Core functionality
    UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
    void InitializeEnemy(const FEnemyData& EnemyData);
    
    UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
    void OnEnemySpawned();
    
    UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
    void OnEnemyDeath();
    
    // State management
    UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
    void SetEnemyState(EEnemyState NewState);
    
    UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
    EEnemyState GetCurrentState() const;
};
```

### Blueprint Function Library

```cpp
// Enemy blueprint function library
UCLASS()
class GAME_API UEnemyBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    // Enemy creation
    UFUNCTION(BlueprintCallable, Category = "Enemy Creation")
    static ABaseEnemy* SpawnEnemyFromData(const FEnemyData& EnemyData, 
                                        const FTransform& SpawnTransform);
    
    // Data manipulation
    UFUNCTION(BlueprintPure, Category = "Enemy Data")
    static FEnemyStats CalculateModifiedStats(const FEnemyStats& BaseStats, 
                                            const TArray<FStatModifier>& Modifiers);
    
    // Combat utilities
    UFUNCTION(BlueprintCallable, Category = "Enemy Combat")
    static float CalculateDamage(const FEnemyStats& AttackerStats, 
                               const FEnemyStats& DefenderStats, 
                               const FDamageData& DamageData);
};
```

### Blueprint Components

```cpp
// Blueprint-exposed enemy component
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class GAME_API UEnemyBlueprintComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    // Blueprint events
    UPROPERTY(BlueprintAssignable, Category = "Enemy Events")
    FOnEnemyStateChanged OnStateChanged;
    
    UPROPERTY(BlueprintAssignable, Category = "Enemy Events")
    FOnEnemyDamaged OnDamaged;
    
    // Blueprint functions
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    virtual void ApplyEffect(const FStatusEffectData& EffectData);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    virtual void TriggerAbility(const FName& AbilityID);
    
protected:
    // Blueprint properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    FEnemyCustomizationData CustomizationData;
};
```

### Blueprint Macros

```cpp
// Enemy blueprint macro library
UCLASS(meta=(ScriptName="EnemyMacros"))
class GAME_API UEnemyBlueprintMacros : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    // Combat macros
    UFUNCTION(BlueprintCallable, Category = "Enemy Macros", meta=(Macro))
    static void ExecuteAttackSequence(ABaseEnemy* Enemy, 
                                    const TArray<FAttackData>& Attacks);
    
    // AI macros
    UFUNCTION(BlueprintCallable, Category = "Enemy Macros", meta=(Macro))
    static void SetupPatrolBehavior(ABaseEnemy* Enemy, 
                                  const TArray<FVector>& PatrolPoints);
    
    // Animation macros
    UFUNCTION(BlueprintCallable, Category = "Enemy Macros", meta=(Macro))
    static void PlayAnimationSequence(ABaseEnemy* Enemy, 
                                    const TArray<FAnimationData>& Animations);
};
```

### Blueprint Nodes

```cpp
// Custom blueprint node for enemy behavior
USTRUCT(meta=(DisplayName="Enemy Behavior Node"))
struct GAME_API FEnemyBehaviorNode
{
    GENERATED_BODY()
    
    // Node configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    EBehaviorNodeType NodeType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    TArray<FBehaviorConnection> Connections;
    
    // Node functionality
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    virtual void ExecuteNode(ABaseEnemy* Enemy);
    
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    virtual bool EvaluateCondition(ABaseEnemy* Enemy);
};
```

### Blueprint Editor Integration

```cpp
// Enemy blueprint editor extension
UCLASS()
class GAME_API UEnemyBlueprintEditorExtension : public UObject
{
    GENERATED_BODY()
    
public:
    // Editor customization
    virtual void CustomizeBlueprint(UBlueprint* Blueprint);
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder);
    
    // Node creation
    UFUNCTION(BlueprintCallable, Category = "Editor")
    virtual void AddCustomBehaviorNode(UEdGraph* Graph, 
                                     const FVector2D& Location);
    
protected:
    // Editor utilities
    virtual void RegisterCustomPinTypes();
    virtual void RegisterCustomNodes();
};
```

### Blueprint Debugging

```cpp
// Enemy blueprint debugger
UCLASS()
class GAME_API UEnemyBlueprintDebugger : public UObject
{
    GENERATED_BODY()
    
public:
    // Debug visualization
    UFUNCTION(BlueprintCallable, Category = "Debug")
    virtual void VisualizeBlueprint(UBlueprint* Blueprint);
    
    UFUNCTION(BlueprintCallable, Category = "Debug")
    virtual void TraceExecution(UBlueprint* Blueprint);
    
protected:
    // Debug utilities
    virtual void HighlightActiveNodes();
    virtual void LogNodeExecution();
    virtual void DisplayVariableValues();
};
```

The Blueprint Integration system provides:

1. **Core Blueprint Interface**
   - Essential enemy functionality exposed to Blueprints
   - Event-driven architecture
   - State management

2. **Blueprint Function Library**
   - Utility functions for common operations
   - Data manipulation helpers
   - Combat calculations

3. **Blueprint Components**
   - Customizable enemy components
   - Event handling
   - Property exposure

4. **Blueprint Macros**
   - Common behavior patterns
   - Reusable functionality
   - Simplified workflow

5. **Blueprint Nodes**
   - Custom behavior nodes
   - Visual scripting support
   - Node customization

6. **Editor Integration**
   - Custom editor extensions
   - Node creation tools
   - Property customization

7. **Blueprint Debugging**
   - Visual debugging tools
   - Execution tracing
   - Variable inspection

This integration enables designers to:
- Create custom enemy behaviors
- Modify enemy properties
- Set up combat patterns
- Configure AI behaviors
- Implement special abilities
- Debug and iterate quickly

## Example Implementations

This section provides practical examples of implementing various enemy types using the enemy system. Each example demonstrates different aspects of the system and serves as a reference for creating new enemies.

### Basic Melee Enemy

```cpp
// Basic melee enemy implementation
UCLASS()
class GAME_API ABasicMeleeEnemy : public ABaseEnemy
{
    GENERATED_BODY()
    
public:
    ABasicMeleeEnemy();
    
    virtual void BeginPlay() override;
    
protected:
    // Configuration
    UPROPERTY(EditDefaultsOnly, Category = "Enemy Config")
    FBasicMeleeConfig MeleeConfig;
    
    // Combat implementation
    virtual void ExecuteMeleeAttack();
    virtual void HandleCombatRange();
    
    // Example implementation of basic melee attack pattern
    void PerformBasicCombo()
    {
        // Setup combo sequence
        TArray<FAttackData> ComboSequence = {
            FAttackData{EAttackType::Light, 1.0f, 0.5f},
            FAttackData{EAttackType::Heavy, 1.5f, 0.8f},
            FAttackData{EAttackType::Finisher, 2.0f, 1.0f}
        };
        
        // Execute combo
        for (const auto& Attack : ComboSequence)
        {
            if (IsInAttackRange())
            {
                ExecuteAttack(Attack);
                WaitForAnimationEnd();
            }
        }
    }
};
```

### Ranged Support Enemy

```cpp
// Ranged support enemy implementation
UCLASS()
class GAME_API ARangedSupportEnemy : public ABaseEnemy
{
    GENERATED_BODY()
    
public:
    ARangedSupportEnemy();
    
protected:
    // Support abilities
    UPROPERTY(EditDefaultsOnly, Category = "Support")
    TArray<FSupportAbility> SupportAbilities;
    
    // Example implementation of support behavior
    void ProvideSupportBehavior()
    {
        // Check for allies needing support
        TArray<ABaseEnemy*> NearbyAllies = FindNearbyAllies();
        for (auto* Ally : NearbyAllies)
        {
            if (ShouldSupportAlly(Ally))
            {
                // Choose and cast appropriate support ability
                FSupportAbility BestAbility = SelectBestSupportAbility(Ally);
                CastSupportAbility(BestAbility, Ally);
            }
        }
    }
    
    // Ranged attack implementation
    virtual void ExecuteRangedAttack();
    virtual void MaintainOptimalRange();
};
```

### Elite Boss Enemy

```cpp
// Elite boss enemy implementation
UCLASS()
class GAME_API AEliteBossEnemy : public ABaseEnemy
{
    GENERATED_BODY()
    
public:
    AEliteBossEnemy();
    
protected:
    // Phase management
    UPROPERTY(Replicated)
    EBossPhase CurrentPhase;
    
    // Example implementation of phase-based boss behavior
    void ManageBossPhases()
    {
        // Phase transition based on health
        float HealthPercentage = GetCurrentHealth() / GetMaxHealth();
        
        switch (CurrentPhase)
        {
            case EBossPhase::Phase1:
                ExecutePhase1Behavior();
                if (HealthPercentage <= 0.7f)
                    TransitionToPhase(EBossPhase::Phase2);
                break;
                
            case EBossPhase::Phase2:
                ExecutePhase2Behavior();
                if (HealthPercentage <= 0.3f)
                    TransitionToPhase(EBossPhase::Phase3);
                break;
                
            case EBossPhase::Phase3:
                ExecutePhase3Behavior();
                break;
        }
    }
    
    // Special abilities
    virtual void ExecuteSpecialAbility();
    virtual void TriggerPhaseTransition();
};
```

### Stealth Assassin Enemy

```cpp
// Stealth assassin enemy implementation
UCLASS()
class GAME_API AStealthAssassinEnemy : public ABaseEnemy
{
    GENERATED_BODY()
    
public:
    AStealthAssassinEnemy();
    
protected:
    // Stealth mechanics
    UPROPERTY(Replicated)
    bool bIsHidden;
    
    // Example implementation of stealth behavior
    void ExecuteStealthBehavior()
    {
        // Stealth movement pattern
        while (!IsDetected())
        {
            // Find optimal ambush position
            FVector AmbushPosition = CalculateAmbushPosition();
            MoveToPositionStealthily(AmbushPosition);
            
            // Check for assassination opportunity
            if (CanExecuteAssassination())
            {
                PerformAssassination();
                break;
            }
        }
        
        // Fallback behavior if detected
        if (IsDetected())
        {
            ExecuteEvasiveManeuvers();
            AttemptRestealth();
        }
    }
    
    // Assassination mechanics
    virtual bool CanExecuteAssassination();
    virtual void PerformAssassination();
};
```

### Example Blueprint Usage

Here's an example of how to set up enemy behavior using Blueprints:

```cpp
// Blueprint-based enemy configuration
UCLASS(Blueprintable)
class GAME_API ABlueprintEnemy : public ABaseEnemy
{
    GENERATED_BODY()
    
public:
    // Blueprint interface
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
    void OnCombatStart();
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
    void OnPhaseChange(EEnemyPhase NewPhase);
    
    // Example of Blueprint-exposed properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    TArray<FAttackPattern> AttackPatterns;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* CustomBehaviorTree;
};
```

### Implementation Best Practices

1. **Base Class Usage**
   ```cpp
   // Always inherit from ABaseEnemy
   class GAME_API ACustomEnemy : public ABaseEnemy
   {
       // Implement required interfaces
       virtual void InitializeEnemy(const FEnemyData& EnemyData) override;
       virtual void OnEnemySpawned() override;
   };
   ```

2. **Component Organization**
   ```cpp
   // Properly organize components
   UPROPERTY(VisibleAnywhere)
   UEnemyCombatComponent* CombatComponent;
   
   UPROPERTY(VisibleAnywhere)
   UEnemyAIComponent* AIComponent;
   
   UPROPERTY(VisibleAnywhere)
   UEnemyCustomizationComponent* CustomizationComponent;
   ```

3. **Network Considerations**
   ```cpp
   // Handle network replication
   virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
   {
       Super::GetLifetimeReplicatedProps(OutLifetimeProps);
       
       DOREPLIFETIME(ACustomEnemy, CurrentState);
       DOREPLIFETIME(ACustomEnemy, ActiveEffects);
   }
   ```

4. **Performance Optimization**
   ```cpp
   // Implement efficient updates
   virtual void Tick(float DeltaTime) override
   {
       Super::Tick(DeltaTime);
       
       // Use timer-based updates for non-critical functions
       if (UpdateTimer >= UpdateInterval)
       {
           UpdateNonCriticalBehavior();
           UpdateTimer = 0.0f;
       }
   }
   ```

5. **Data-Driven Configuration**
   ```cpp
   // Use data assets for configuration
   UPROPERTY(EditDefaultsOnly, Category = "Enemy Data")
   UEnemyDataAsset* EnemyDataAsset;
   ```

6. **Network Considerations**
   ```cpp
   // Implement network-aware behavior
   virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
   {
       Super::GetLifetimeReplicatedProps(OutLifetimeProps);
       
       DOREPLIFETIME(ACustomEnemy, CurrentHealth);
       DOREPLIFETIME(ACustomEnemy, CurrentState);
   }
   ```

7. **Performance Optimization**
   ```cpp
   // Implement efficient updates
   virtual void Tick(float DeltaTime) override
   {
       Super::Tick(DeltaTime);
       
       // Use timer-based updates for non-critical functions
       if (UpdateTimer >= UpdateInterval)
       {
           UpdateNonCriticalBehavior();
           UpdateTimer = 0.0f;
       }
   }
   ```

These examples demonstrate:
1. Different enemy type implementations
2. Integration with the core systems
3. Blueprint exposure and customization
4. Best practices for implementation
5. Data-driven configuration
6. Network considerations
7. Performance optimization strategies

## Best Practices

This section outlines comprehensive guidelines and best practices for working with the enemy system. Following these practices ensures consistency, maintainability, and optimal performance.

### Code Organization

1. **Class Hierarchy**
   ```cpp
   // Follow consistent inheritance patterns
   class ABaseEnemy : public ACharacter { ... }
   class ASpecializedEnemy : public ABaseEnemy { ... }
   class ACustomEnemy : public ASpecializedEnemy { ... }
   ```

2. **Component Structure**
   ```cpp
   // Organize components logically
   UPROPERTY(VisibleAnywhere, Category = "Components")
   UEnemyCombatComponent* CombatComponent;
   
   UPROPERTY(VisibleAnywhere, Category = "Components")
   UEnemyAIComponent* AIComponent;
   
   // Group related functionality
   UPROPERTY(VisibleAnywhere, Category = "Combat")
   TArray<UAbilityComponent*> CombatAbilities;
   ```

3. **Interface Implementation**
   ```cpp
   // Implement interfaces clearly
   class ACustomEnemy : public ABaseEnemy,
                       public IEnemyCombatInterface,
                       public IEnemyAIInterface
   {
       // Group interface implementations together
       virtual void OnCombatStart() override;
       virtual void OnCombatEnd() override;
       
       virtual void OnAIStateChange() override;
       virtual void OnAITargetChanged() override;
   };
   ```

### Data Management

1. **Data Asset Usage**
   ```cpp
   // Use data assets for configuration
   UCLASS()
   class UEnemyConfigData : public UDataAsset
   {
       GENERATED_BODY()
       
   public:
       UPROPERTY(EditDefaultsOnly, Category = "Stats")
       FEnemyStats BaseStats;
       
       UPROPERTY(EditDefaultsOnly, Category = "Combat")
       TArray<FAbilityData> Abilities;
       
       UPROPERTY(EditDefaultsOnly, Category = "AI")
       FAIConfig AIConfiguration;
   };
   ```

2. **State Management**
   ```cpp
   // Use clear state management
   UENUM(BlueprintType)
   enum class EEnemyState
   {
       Idle,
       Patrolling,
       Engaging,
       Attacking,
       Stunned,
       Dead
   };
   
   // Implement state transitions
   void ACustomEnemy::SetState(EEnemyState NewState)
   {
       if (CurrentState != NewState)
       {
           OnStateExit(CurrentState);
           CurrentState = NewState;
           OnStateEnter(CurrentState);
       }
   }
   ```

3. **Performance Optimization**
   ```cpp
   // Implement efficient resource handling
   class AOptimizedEnemy : public ABaseEnemy
   {
   protected:
       // Pool frequently used resources
       UPROPERTY()
       TArray<UParticleSystemComponent*> EffectPool;
       
       // Implement efficient resource allocation
       UParticleSystemComponent* GetEffectFromPool()
       {
           for (auto* Effect : EffectPool)
           {
               if (!Effect->IsActive())
                   return Effect;
           }
           return CreateNewEffect();
       }
   };
   ```

4. **Update Frequency**
   ```cpp
   // Optimize update cycles
   void ACustomEnemy::Tick(float DeltaTime)
   {
       Super::Tick(DeltaTime);
       
       // Use timer-based updates for non-critical functions
       if (UpdateTimer >= UpdateInterval)
       {
           UpdateNonCriticalSystems();
           UpdateTimer = 0.0f;
       }
       
       // Update critical systems every frame
       UpdateCriticalSystems(DeltaTime);
   }
   ```

5. **Network Optimization**
   ```cpp
   // Implement efficient replication
   void ACustomEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
   {
       Super::GetLifetimeReplicatedProps(OutLifetimeProps);
       
       // Replicate only necessary properties
       DOREPLIFETIME_CONDITION(ACustomEnemy, Health, COND_OwnerOnly);
       DOREPLIFETIME(ACustomEnemy, CurrentState);
   }
   ```

6. **RPC Usage**
   ```cpp
   // Use appropriate RPC calls
   UFUNCTION(Server, Reliable)
   void ServerExecuteAbility(FAbilityData AbilityData);
   
   UFUNCTION(NetMulticast, Unreliable)
   void MulticastPlayEffect(FVector Location);
   ```

7. **AI and Behavior**
   ```cpp
   // Organize behavior trees efficiently
   void ACustomEnemy::SetupBehaviorTree()
   {
       // Use clear task hierarchy
       UBTTask_Sequence* MainSequence = CreateSequence();
       UBTTask_Selector* CombatSelector = CreateSelector();
       
       // Add clear conditions
       AddCondition(CombatSelector, "IsInCombat");
       AddTask(CombatSelector, "ExecuteCombatBehavior");
   }
   ```

8. **AI Performance**
   ```cpp
   // Implement efficient AI updates
   void ACustomEnemy::UpdateAI()
   {
       // Use spatial awareness for optimization
       if (IsInPlayerRange())
       {
           UpdateDetailedBehavior();
       }
       else
       {
           UpdateSimplifiedBehavior();
       }
   }
   ```

9. **Visual Effects**
   ```cpp
   // Manage effects efficiently
   class UEnemyVFXManager : public UObject
   {
   public:
       // Pool and reuse effects
       void PlayEffect(FEffectData EffectData)
       {
           UParticleSystemComponent* Effect = GetEffectFromPool();
           ConfigureEffect(Effect, EffectData);
           Effect->Activate();
       }
   };
   ```

10. **LOD System**
    ```cpp
    // Implement proper LOD
    void ACustomEnemy::UpdateLOD(float Distance)
    {
        // Adjust effect quality based on distance
        int32 LODLevel = CalculateLODLevel(Distance);
        UpdateEffectQuality(LODLevel);
        UpdateAnimationQuality(LODLevel);
    }
    ```

11. **Debug and Testing**
    ```cpp
    // Implement clear debug tools
    void ACustomEnemy::DrawDebugInfo()
    {
        if (bDebugMode)
        {
            DrawDebugSphere(GetWorld(), GetActorLocation(), DetectionRadius, 12, FColor::Red);
            DrawDebugString(GetWorld(), GetActorLocation(), *StateToString(CurrentState));
        }
    }
    ```

12. **Testing Framework**
    ```cpp
    // Implement comprehensive tests
    void UEnemyTests::RunTests()
    {
        TestSpawning();
        TestCombat();
        TestAIBehavior();
        TestNetworking();
        
        LogTestResults();
    }
    ```

13. **Documentation**
    ```cpp
    /**
     * Executes the enemy's special ability
     * @param AbilityIndex - Index of the ability to execute
     * @param Target - Target for the ability
     * @return True if ability was successfully executed
     */
    bool ACustomEnemy::ExecuteSpecialAbility(int32 AbilityIndex, AActor* Target)
    {
        // Implementation
    }
    ```

14. **Blueprint Documentation**
    ```cpp
    // Provide clear Blueprint documentation
    UFUNCTION(BlueprintCallable, Category = "Enemy|Combat",
              meta=(ToolTip="Executes a special ability targeting the specified actor"))
    virtual bool ExecuteSpecialAbility(int32 AbilityIndex, AActor* Target);
    ```

These best practices ensure:
1. Consistent code organization
2. Efficient data management
3. Optimal performance
4. Proper network handling
5. Effective AI implementation
6. Quality visual effects
7. Comprehensive debugging
8. Clear documentation

Following these guidelines will help create maintainable, efficient, and robust enemy implementations.
