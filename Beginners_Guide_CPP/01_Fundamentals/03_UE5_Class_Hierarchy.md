# Understanding UE5 Class Hierarchy

## Overview

This guide explains the Unreal Engine 5 class hierarchy, focusing on the most important classes you'll work with when developing hack and slash games. Understanding this hierarchy is crucial for knowing which base classes to inherit from and what functionality is available to you.

## Prerequisites

Before starting this guide, you should:
- Have completed the [C++ Basics for Unreal Engine](02_CPP_Basics_For_UE5.md) guide
- Understand basic object-oriented programming concepts
- Be familiar with inheritance and polymorphism

## Core Class Hierarchy

### UObject - The Root of All Things

At the top of Unreal Engine's class hierarchy is `UObject`. This base class provides fundamental functionality:

- Garbage Collection
- Reflection System
- Serialization
- Property System
- Asset Management

```cpp
// Example of a basic UObject class
UCLASS()
class MYGAME_API UMyObject : public UObject
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere)
    float MyValue;
    
    UFUNCTION(BlueprintCallable)
    void MyFunction();
};
```

### AActor - Interactive World Objects

`AActor` inherits from `UObject` and represents any object that can be placed in the game world:

- Has a transform (location, rotation, scale)
- Can be spawned and destroyed
- Can have components attached
- Receives tick events

Common `AActor` subclasses:
- `APawn` - Controllable characters
- `ACharacter` - Humanoid characters with movement
- `APlayerController` - Player input and control
- `AGameMode` - Game rules and state
- `AGameState` - Shared game state information

```cpp
// Example of a basic Actor class
UCLASS()
class MYGAME_API AMyActor : public AActor
{
    GENERATED_BODY()
    
public:
    // Constructor
    AMyActor();
    
protected:
    // Called when game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;
};
```

### UActorComponent - Building Blocks

`UActorComponent` is the base class for components that can be attached to Actors:

- Can be added/removed from Actors
- Can be enabled/disabled
- Can receive tick events
- Manages specific functionality

Common component types:
- `USceneComponent` - Components with transforms
- `UPrimitiveComponent` - Components with rendering/collision
- `USkeletalMeshComponent` - Animated character meshes
- `UStaticMeshComponent` - Static 3D models
- `UCharacterMovementComponent` - Character movement logic

```cpp
// Example of a custom component
UCLASS(ClassGroup="Combat", meta=(BlueprintSpawnableComponent))
class MYGAME_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UCombatComponent();
    
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackDamage;
    
    UFUNCTION(BlueprintCallable)
    void PerformAttack();
};
```

## Important Class Relationships

### Character Movement System

```
ACharacter
    ├── USkeletalMeshComponent (Mesh)
    ├── UCapsuleComponent (Collision)
    ├── UCharacterMovementComponent (Movement)
    └── USpringArmComponent (Optional, for camera)
        └── UCameraComponent (Optional)
```

### Player Control System

```
AGameMode
    └── APlayerController
        └── APawn/ACharacter (Possessed Pawn)
            └── UPlayerInput (Input Processing)
```

### Component Hierarchy

```
AActor
    ├── USceneComponent (Root)
    │   ├── UStaticMeshComponent
    │   ├── USkeletalMeshComponent
    │   └── Other Scene Components
    └── UActorComponent (Non-Scene Components)
```

## Common Base Classes for Hack and Slash Games

### Character Classes

```cpp
// Base character class for hack and slash games
UCLASS()
class MYGAME_API AHackAndSlashCharacter : public ACharacter
{
    GENERATED_BODY()
    
protected:
    // Combat component for attack logic
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCombatComponent* CombatComponent;
    
    // Stats component for health/stamina
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCharacterStatsComponent* StatsComponent;
    
    // Inventory component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UInventoryComponent* InventoryComponent;
};
```

### Enemy Classes

```cpp
// Base enemy class
UCLASS()
class MYGAME_API ABaseEnemy : public ACharacter
{
    GENERATED_BODY()
    
protected:
    // AI controller reference
    UPROPERTY()
    class AEnemyAIController* EnemyAIController;
    
    // Behavior tree component
    UPROPERTY(VisibleAnywhere)
    class UBehaviorTreeComponent* BehaviorTreeComponent;
    
    // Perception component for detecting players
    UPROPERTY(VisibleAnywhere)
    class UAIPerceptionComponent* AIPerceptionComponent;
};
```

### Weapon Classes

```cpp
// Base weapon class
UCLASS()
class MYGAME_API ABaseWeapon : public AActor
{
    GENERATED_BODY()
    
protected:
    // Weapon mesh
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* WeaponMesh;
    
    // Collision component for hit detection
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* HitBox;
    
    // Stats for the weapon
    UPROPERTY(EditAnywhere, Category = "Weapon Stats")
    FWeaponStats WeaponStats;
};
```

## Best Practices

1. **Choose the Right Base Class**
   - Use `AActor` for objects that need world placement
   - Use `UActorComponent` for modular functionality
   - Use `ACharacter` for player and enemy characters
   - Use `UObject` for pure data/logic objects

2. **Component-Based Design**
   - Break functionality into components
   - Make components reusable across different actors
   - Use composition over inheritance when possible

3. **Proper Initialization Order**
   - Constructor: Set default values only
   - BeginPlay: Initialize gameplay elements
   - PostInitializeComponents: Set up component interactions

4. **Clean Inheritance Hierarchies**
   - Don't create deep inheritance chains
   - Use interfaces for shared behavior
   - Consider composition for varied functionality

## Common Pitfalls

### Issue: Deep Inheritance Chains
```cpp
// Bad: Deep inheritance chain
class ASpecialMagicSwordWarrior : public AMagicSwordWarrior // Avoid this

// Better: Composition with components
class AWarrior : public ACharacter
{
    UPROPERTY()
    UMagicComponent* MagicComponent;
    
    UPROPERTY()
    UWeaponComponent* WeaponComponent;
};
```

### Issue: Tight Coupling
```cpp
// Bad: Direct references to specific classes
class AEnemy : public ACharacter
{
    UPROPERTY()
    ASpecificPlayerClass* Player; // Avoid this
};

// Better: Use interfaces
class AEnemy : public ACharacter
{
    UPROPERTY()
    TScriptInterface<ITargetable> CurrentTarget;
};
```

## Next Steps

Now that you understand UE5's class hierarchy, you can explore:

1. [Blueprint vs C++: When to Use Each](04_Blueprint_vs_CPP.md) - Learn when to use each approach
2. [Using Cursor IDE Effectively](05_Using_Cursor_IDE.md) - Master your development environment
3. [Creating Your First Character](../02_GameplayBasics/01_Creating_First_Character.md) - Apply your knowledge