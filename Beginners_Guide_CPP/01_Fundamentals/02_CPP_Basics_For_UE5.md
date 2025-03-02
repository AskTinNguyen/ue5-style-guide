# C++ Basics for Unreal Engine

## Overview

This guide introduces the fundamental C++ concepts specifically relevant to Unreal Engine 5 development. We'll focus on the core C++ features you'll use most frequently when developing hack and slash games.

## Prerequisites

Before starting this guide, you should:
- Have completed the [Setting Up Your Development Environment](01_Setting_Up_Environment.md) guide
- Have basic programming knowledge (variables, functions, conditions, loops)
- Have your UE5 project created and ready

## Unreal Engine's C++ Environment

Unreal Engine uses a specialized version of C++ with additional macros, helpers, and systems that extend the standard language. The key differences include:

1. **Reflection System** - Uses macros like `UCLASS()`, `UPROPERTY()`, and `UFUNCTION()` to expose C++ to the engine
2. **Memory Management** - Utilizes smart pointers and garbage collection 
3. **Type System** - Adds Unreal-specific types beyond standard C++ types
4. **Preprocessor Macros** - Extensive use of macros to simplify common operations

## Essential C++ Concepts for UE5

### 1. Header and Implementation Files

In UE5, C++ code is typically split into two files:

- **Header Files (.h)** - Contain class declarations, properties, and function signatures
- **Implementation Files (.cpp)** - Contain the actual implementation of functions

Example header file:
```cpp
// MyActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h" // Always include this last!

UCLASS()
class MYGAME_API AMyActor : public AActor
{
    GENERATED_BODY()
    
public:    
    // Constructor
    AMyActor();
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
public:    
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};
```

Example implementation file:
```cpp
// MyActor.cpp
#include "MyActor.h"

// Constructor
AMyActor::AMyActor()
{
    // Set default values
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
    Super::BeginPlay(); // Always call parent implementation
    
    UE_LOG(LogTemp, Warning, TEXT("Actor has started!"));
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); // Always call parent implementation
    
    // Update logic here
}
```

### 2. Unreal Engine Class Types

UE5 has several base class types you'll frequently use:

| Prefix | Base Class | Purpose | Common Use |
|--------|------------|---------|------------|
| `A` | `AActor` | Objects that can be placed/spawned in the world | Characters, projectiles, pickups |
| `U` | `UObject` | Engine core object system | Components, widgets, subsystems |
| `F` | (struct) | Lightweight data structures | Stat values, configuration data |
| `E` | (enum) | Enumeration types | State machines, option lists |
| `I` | (interface) | Interface classes | Interaction systems, damage handlers |
| `T` | (template) | Template classes | Containers, arrays, maps |

### 3. UE5 Reflection System

The reflection system is what allows C++ classes to be visible to the editor and Blueprint system:

#### UCLASS Macro
```cpp
UCLASS(Blueprintable, BlueprintType)
class MYGAME_API AMyCharacter : public ACharacter
{
    GENERATED_BODY() // Required in every UCLASS
    
    // Class contents
};
```

Common UCLASS specifiers:
- `Blueprintable` - Can be subclassed in Blueprints
- `BlueprintType` - Can be used as a variable type in Blueprints
- `Abstract` - Cannot be instantiated directly
- `Config=GameName` - Loads values from config files

#### UPROPERTY Macro
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
float Damage = 10.0f;
```

Common UPROPERTY specifiers:
- `EditAnywhere` - Can be edited in the editor and on instances
- `EditDefaultsOnly` - Can only be edited in the class defaults
- `VisibleAnywhere` - Visible but not editable in the editor
- `BlueprintReadWrite` - Can be read and written from Blueprints
- `BlueprintReadOnly` - Can only be read from Blueprints
- `Category = "CategoryName"` - Organizes properties in the editor

#### UFUNCTION Macro
```cpp
UFUNCTION(BlueprintCallable, Category = "Combat")
void ApplyDamage(AActor* Target, float DamageAmount);
```

Common UFUNCTION specifiers:
- `BlueprintCallable` - Can be called from Blueprints
- `BlueprintPure` - No side effects, can be used in Blueprint expressions
- `BlueprintImplementableEvent` - Declared in C++, implemented in Blueprint
- `BlueprintNativeEvent` - Can be implemented in C++ and overridden in Blueprint

### 4. Unreal Engine Common Types

UE5 provides its own types that you'll use frequently:

| Unreal Type | Description | Common Use |
|-------------|-------------|------------|
| `FString` | Dynamic string | Text manipulation, messages |
| `FName` | Lightweight immutable string | IDs, tags, optimization |
| `FText` | Localizable text | UI text, localization |
| `FVector` | 3D vector (X, Y, Z) | Positions, directions |
| `FRotator` | Rotation (Pitch, Yaw, Roll) | Object rotations |
| `FTransform` | Combined location, rotation, scale | Object transforms |
| `TArray<T>` | Dynamic array | Collections of objects |
| `TMap<K, V>` | Dictionary/hashtable | Key-value pairs |
| `TSet<T>` | Unique collection | Collections without duplicates |

### 5. Memory Management

UE5 uses a garbage collection system for memory management:

```cpp
// Creating objects
UWorld* World = GetWorld();
if (World)
{
    // Spawn an actor (automatically managed)
    AMyActor* MyActor = World->SpawnActor<AMyActor>(AMyActor::StaticClass(), Location, Rotation);
    
    // Create a component
    UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(this);
    MeshComp->RegisterComponent(); // Register for it to function
    
    // Create a UObject
    UMyObject* MyObj = NewObject<UMyObject>();
}
```

Smart pointers:
```cpp
// Weak pointer (doesn't prevent garbage collection)
TWeakObjectPtr<AActor> WeakActorRef = SomeActor;

// Shared pointer (reference counted)
TSharedPtr<FMyStruct> SharedData = MakeShared<FMyStruct>();

// Unique pointer (single owner)
TUniquePtr<FMyStruct> UniqueData = MakeUnique<FMyStruct>();
```

### 6. Logging and Debugging

UE5 provides several ways to log information:

```cpp
// Basic logging
UE_LOG(LogTemp, Warning, TEXT("This is a warning message"));

// Logging with variables
FString PlayerName = "Hero";
int32 PlayerLevel = 5;
UE_LOG(LogTemp, Display, TEXT("Player %s is level %d"), *PlayerName, PlayerLevel);

// Screen messages (visible during gameplay)
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("On-screen debug message"));

// Draw debug lines and shapes (visible in the game world)
DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.f, 0, 1.f);
DrawDebugSphere(GetWorld(), Location, 50.f, 16, FColor::Green, false, 5.f);
```

## Common UE5 C++ Patterns

### Timers

```cpp
// Declare a timer handle in your header
FTimerHandle AttackTimerHandle;

// Set a timer for a function call after delay
GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AMyCharacter::PerformAttack, 2.0f, false);

// Set a repeating timer
GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AMyCharacter::PerformAttack, 1.0f, true);

// Clear a timer
GetWorldTimerManager().ClearTimer(AttackTimerHandle);
```

### Delegates and Events

```cpp
// Declare a delegate in your header
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, float, DamageAmount);

// Expose it as a property
UPROPERTY(BlueprintAssignable, Category = "Events")
FOnDamageTaken OnDamageTaken;

// Trigger the delegate
void AMyCharacter::TakeDamage(float Amount)
{
    Health -= Amount;
    OnDamageTaken.Broadcast(Amount); // Notify all listeners
}

// Bind to a delegate
OtherActor->OnDamageTaken.AddDynamic(this, &AMyClass::HandleDamageTaken);

// Delegate callback
void AMyClass::HandleDamageTaken(float DamageAmount)
{
    UE_LOG(LogTemp, Warning, TEXT("Damage taken: %f"), DamageAmount);
}
```

### Collision Detection

```cpp
// Simple collision check
FHitResult HitResult;
FCollisionQueryParams QueryParams;
QueryParams.AddIgnoredActor(this);

if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, QueryParams))
{
    // We hit something
    AActor* HitActor = HitResult.GetActor();
    if (HitActor)
    {
        // Process the hit
    }
}

// Set up collision response
UBoxComponent* TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
TriggerBox->SetCollisionProfileName("Trigger");
TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

// Set up collision events
TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMyActor::OnOverlapBegin);
```

## Best Practices for UE5 C++ Development

1. **Follow the UE5 Naming Conventions**
   - Classes: PascalCase with appropriate prefix (A, U, F, etc.)
   - Functions: PascalCase
   - Variables: PascalCase with appropriate prefix for member variables
   - Consider using our [C++ Style Guide](../../UE_Pattern_Libraries/UE5-CPP-Style-Guide.md)

2. **Minimize C++ Recompilation**
   - Use Blueprints for quick iteration
   - Use data assets instead of hardcoded values
   - Consider using the `BlueprintImplementableEvent` for functions you want to test quickly

3. **Optimize Property Access**
   - Use `const` references for function parameters to avoid copying
   - Cache frequently accessed components and values
   - Use appropriate access specifiers (private, protected, public)

4. **Learn to Read Unreal's Error Messages**
   - UE5 error messages can be verbose but contain valuable information
   - Look for the actual error amid the macro expansions
   - Search for specific error codes if you're stuck

## Common Pitfalls and Solutions

### Issue: Null Pointer Access
```cpp
// Problem: Potential crash if GetWorld() returns nullptr
GetWorld()->SpawnActor<AActor>(ActorClass, Location, Rotation);

// Solution: Always check pointers before using them
UWorld* World = GetWorld();
if (World)
{
    World->SpawnActor<AActor>(ActorClass, Location, Rotation);
}
```

### Issue: Forgetting to Call Super Implementation
```cpp
// Problem: Missing parent class functionality
void AMyCharacter::BeginPlay()
{
    // Missing Super::BeginPlay() call
    // Your code here
}

// Solution: Always call parent implementation first
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    // Your code here
}
```

### Issue: Improper Include Order
```cpp
// Problem: generated.h not included last
#include "MyActor.generated.h"
#include "SomeOtherHeader.h" // Error!

// Solution: generated.h must be included last
#include "SomeOtherHeader.h"
#include "MyActor.generated.h"
```

## Next Steps

Now that you understand the basics of C++ in Unreal Engine 5, you're ready to explore:

1. [Understanding UE5 Class Hierarchy](03_UE5_Class_Hierarchy.md) - Learn about the class inheritance system
2. [Blueprint vs C++: When to Use Each](04_Blueprint_vs_CPP.md) - Understand when to use which approach
3. [Using Cursor IDE Effectively](05_Using_Cursor_IDE.md) - Maximize your productivity with Cursor
