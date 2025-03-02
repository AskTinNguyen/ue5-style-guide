# Unreal Engine 5 C++ Style Guide

*A clear and concise approach to C++ coding in Unreal Engine 5*

## Introduction

This style guide provides a set of best practices for writing C++ code in Unreal Engine 5 projects. It is designed to help both experienced engineers and non-engineers who need to work with C++ code in Unreal Engine.

> **Note**: This guide focuses on core C++ style guidelines. For domain-specific patterns and best practices, please refer to our specialized pattern libraries:
> - [Combat Patterns](Combat_Pattern_Library/CombatPatternLibrary.md)
> - [UI/UX Patterns](UI_Pattern_Library/UIPatternLibrary.md)
> - [AI Patterns](AI_Pattern_Library/AIPatternLibrary.md)
> - [Character Patterns](Character_Pattern_Library/CharacterPatternLibrary.md)
> - [Inventory Patterns](Inventory_Pattern_Library/InventoryPatternLibrary.md)
> - [Save/Load Patterns](Save_Load_Pattern_Library/SaveLoadPatternLibrary.md)

### Who Should Use This Guide

- Engineers working on UE5 projects
- Designers who need to read or modify C++ code
- Technical artists implementing game features
- Team leads establishing project standards
- Anyone learning C++ in the context of Unreal Engine

### How to Use This Guide

This document is structured to serve as both a reference and learning resource. New team members should read this guide in its entirety to understand the expected coding style, while experienced developers can use the table of contents to quickly reference specific guidelines.

The guide focuses on practical advice rather than exhaustive rules. It presents common patterns, best practices, and solutions to problems you're likely to encounter in real-world game development with Unreal Engine 5.

Remember that consistency within a project is the most important rule. If your team has existing conventions that differ from this guide, follow your team's established patterns.

## Table of Contents
- [1. Core Principles](#1-core-principles)
- [2. C++ Naming Conventions](#2-c-naming-conventions)
- [3. Code Structure and Organization](#3-code-structure-and-organization)
- [4. Unreal Specific C++ Practices](#4-unreal-specific-c-practices)
- [5. Blueprint and C++ Integration](#5-blueprint-and-c-integration)
- [6. Performance Considerations](#6-performance-considerations)
- [7. Common Pitfalls and How to Avoid Them](#7-common-pitfalls-and-how-to-avoid-them)
- [8. Appendix](#8-appendix)

## 1. Core Principles

### 1.1 Consistency Above All
All code should look like a single person wrote it, regardless of how many contributors there are. Code consistency allows team members to understand and modify any part of the codebase without learning different styles.

### 1.2 Readability Over Cleverness
Write code that's easy to read and understand. Avoid overly clever tricks that might confuse other developers or your future self.

### 1.3 Document and Comment Thoughtfully
Write meaningful comments that explain "why" rather than "what." The code itself should clearly show what it's doing.

### 1.4 Follow Unreal's Conventions
When in doubt, follow Epic Games' coding standards. They've established patterns that work well with the engine.

## 2. C++ Naming Conventions

### 2.1 Class Naming
- Classes should be prefixed according to their type:
  - `A` for Actors (e.g., `APlayerCharacter`)
  - `U` for Objects (e.g., `UHealthComponent`)
  - `F` for structs (e.g., `FHitResult`)
  - `E` for enums (e.g., `EMovementState`)
  - `I` for interfaces (e.g., `IInteractable`)
  - `T` for templates (e.g., `TArray`)

### 2.2 Variable Naming
- Use PascalCase for all variables
- Prefix member variables with appropriate type indicators:
  - `b` for booleans (e.g., `bIsDead`)
  - `f` for floats (e.g., `fMaxSpeed`)
  - `i` for integers (e.g., `iPlayerCount`)
  - `v` for vectors (e.g., `vStartLocation`)
  - `r` for rotators (e.g., `rInitialRotation`)
  - None for complex types/objects (e.g., `PlayerController`)
- Private variables should have an underscore prefix (e.g., `_Health`)

### 2.3 Function Naming
- Use PascalCase for all functions
- Begin with a verb that describes the action (e.g., `CalculateDamage`, `FindNearestEnemy`)
- Property getters should not use a "Get" prefix unless they perform significant work
- Boolean functions should be phrased as questions (e.g., `IsAlive()`, `CanAttack()`)
- Event handlers should begin with "On" (e.g., `OnDamageReceived()`)
- RPC functions should be prefixed with their execution context:
  - `Server_` for functions called on server
  - `Client_` for functions called on client
  - `Multicast_` for functions called on all clients

## 3. Code Structure and Organization

### 3.1 Header Organization
Headers should be organized in the following order:
1. Class declaration with appropriate macros
2. Public methods and properties
3. Protected methods and properties
4. Private methods and properties
5. UPROPERTY and UFUNCTION macros should be on separate lines from the variables/functions they modify

Example:
```cpp
UCLASS()
class MYGAME_API AMyActor : public AActor
{
    GENERATED_BODY()
    
public:
    // Constructor and virtual functions
    AMyActor();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    // Public functions
    void ApplyDamage(float DamageAmount);
    
    // Public properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MaxHealth;
    
protected:
    // Protected functions
    void HandleDeath();
    
    // Protected properties
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    float CurrentHealth;
    
private:
    // Private functions
    void UpdateHealthUI();
    
    // Private properties
    UPROPERTY()
    bool _bIsDying;
};
```

### 3.2 Source File Organization
1. Include necessary headers
2. Implement constructor and destructor first
3. Implement overridden functions next
4. Implement public methods
5. Implement protected methods
6. Implement private methods

### 3.3 Keep Functions Small and Focused
- Functions should do one thing and do it well
- If a function exceeds 50 lines, consider breaking it into smaller functions
- Never exceed 100 lines for a single function

## 4. Unreal Specific C++ Practices

### 4.1 Memory Management
- Use Unreal's memory management system
- Avoid manual `new` and `delete` operations
- Use `CreateDefaultSubobject<>` for components in actor constructors
- Use `NewObject<>` for runtime object creation
- Use weak pointers (`TWeakObjectPtr`) for optional references to UObjects
- Use `TSharedPtr` and `TSharedRef` for non-UObject resources

### 4.2 UPROPERTY and UFUNCTION Macros
- Always use appropriate specifiers for properties and functions
- For UPROPERTY:
  - `EditAnywhere` - Property can be edited in editor for all instances
  - `EditDefaultsOnly` - Property editable only on archetypes (defaults)
  - `EditInstanceOnly` - Property editable only on instances, not archetypes
  - `VisibleAnywhere` - Property is visible but not editable
  - `BlueprintReadWrite` - Property can be read and written from Blueprints
  - `BlueprintReadOnly` - Property can be read but not written from Blueprints
  - Use appropriate categories to organize properties in the editor

- For UFUNCTION:
  - `BlueprintCallable` - Function can be called from Blueprint
  - `BlueprintPure` - Function doesn't modify state and can be called from Blueprint
  - `BlueprintImplementableEvent` - Function declared in C++ but implemented in Blueprint
  - `BlueprintNativeEvent` - Function has C++ default implementation but can be overridden in Blueprint

> **Note**: For specific implementation patterns using these macros, refer to:
> - [Character Components](Character_Pattern_Library/Pattern1_CharacterComponents.md)
> - [Item Data Management](Inventory_Pattern_Library/Pattern1_ItemDataManagement.md)
> - [Save Data Architecture](Save_Load_Pattern_Library/Pattern1_SaveDataArchitecture.md)

### 4.3 Garbage Collection
- Always use `TWeakObjectPtr` for cross-references between objects to avoid reference cycles
- Mark all UObject references with UPROPERTY()
- Be careful with lambdas and delegates that can cause memory leaks

## 5. Blueprint and C++ Integration

### 5.1 Blueprint Exposure
- Design your C++ classes with Blueprint extension in mind
- Expose variables with clear, descriptive tooltips using the `ToolTip` specifier
- Provide reasonable default values and validation ranges for numeric properties
- Use `Category` specifiers to organize properties and functions in logical groups

### 5.2 Creating Blueprint-Friendly APIs
- Create high-level functions that Blueprint users can easily understand
- Provide Blueprint-native alternatives for complex C++ operations
- Document your API with both comments and tooltips
- Consider adding validation with `Exec` pins in your Blueprint nodes for better user feedback

> **Note**: For AI-specific Blueprint integration patterns, see:
> - [Behavior Tree Framework](AI_Pattern_Library/Pattern1_BehaviorTreeFramework.md)
> - [State Machine System](AI_Pattern_Library/Pattern2_StateMachineSystem.md)

## 6. Performance Considerations

### 6.1 General Optimization
- Avoid unnecessary calculations in the Tick function
- Use timers or custom tick groups for operations that don't need to happen every frame
- Cache frequently accessed values rather than recalculating them
- Use profiling tools to identify bottlenecks

### 6.2 Memory Efficiency
- Pre-allocate memory for containers when size is known (e.g., `TArray::Reserve()`)
- Consider using TArray for small collections and TSet/TMap for larger ones where lookup performance matters
- Use appropriate container types (TSet for unique items, TMap for key-value pairs)
- Be mindful of struct sizes, especially for components that will have many instances

### 6.3 Threading
- Use Unreal's task graph system for parallelization
- Be careful with shared state in threaded code
- Use atomic operations for thread-safe counters
- Consider using AsyncTasks for heavy operations that can be done off the game thread

> **Note**: For async operation patterns, see:
> - [Async Save System](Save_Load_Pattern_Library/Pattern3_AsyncSaveSystem.md)

## 7. Common Pitfalls and How to Avoid Them

### 7.1 Null Pointers
- Always check pointers before dereferencing them
- Use `IsValid()` for UObjects to check both null and pending kill status
- Consider using TOptional for values that might not exist

### 7.2 Circular Dependencies
- Avoid circular includes by using forward declarations
- Design your class hierarchy to minimize dependencies
- Be careful with complex delegate setups that can cause reference cycles

### 7.3 Replication Issues
- Mark replicated properties with the `Replicated` specifier
- Implement `GetLifetimeReplicatedProps` for classes with replicated properties
- Use appropriate replication conditions for efficient networking
- Create proper `OnRep_*` functions for replicated properties that need to trigger visual updates

### 7.4 Hot Reload Failure
- Avoid complex static initialization
- Be careful with template code that can cause hot reload issues
- Keep constructors simple and consider moving complex initialization to BeginPlay

## 8. Appendix

### 8.1 Additional Resources

- [Epic Games Coding Standard](https://docs.unrealengine.com/5.0/en-US/epic-cplusplus-coding-standard-for-unreal-engine/)
- [Unreal Engine 5 Documentation](https://docs.unrealengine.com/5.0/en-US/)
- [Unreal Engine API Reference](https://docs.unrealengine.com/5.0/en-US/API/)
- [Unreal Engine C++ Dev Community](https://forums.unrealengine.com/c/development-discussion/c-gameplay-programming/146)

> **Note**: For domain-specific examples and patterns, refer to our pattern libraries listed at the top of this document.