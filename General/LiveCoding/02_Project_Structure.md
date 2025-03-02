# Project Structure for Live Coding

## Module Organization

```plaintext
ProjectName/
├── Source/
│   ├── Core/           # Stable, rarely changed
│   │   ├── Interfaces/ # Core interfaces
│   │   └── Base/       # Base classes
│   │
│   ├── Gameplay/       # Live-coding friendly
│   │   ├── Characters/ # Character implementations
│   │   ├── Abilities/  # Gameplay abilities
│   │   └── Systems/    # Game systems
│   │
│   └── Tools/          # Development utilities
```

## Header Organization

### Good Header Example
```cpp
// MyCharacter.h
class AMyCharacter : public ACharacter {
    GENERATED_BODY()
public:
    // ✓ Interface declarations only
    virtual void ProcessInput();
    
    // ✓ Stable container properties
    UPROPERTY()
    TMap<FName, float> Stats;
    
    // ✓ Flexible data structures
    UPROPERTY()
    TArray<TSharedPtr<FAbilityData>> Abilities;
};
```

### Bad Header Example
```cpp
// ❌ Avoid implementation in headers
class AMyCharacter : public ACharacter {
    // ❌ Avoid fixed properties that might change
    UPROPERTY()
    float Health;
    float Mana;
    
    // ❌ Avoid implementation in headers
    void ProcessInput() {
        // Logic here will require rebuild
    }
};
```

## Implementation Organization

### Good Implementation Example
```cpp
// MyCharacter.cpp
void AMyCharacter::ProcessInput() {
    // ✓ Implementation details here
    // ✓ Can be modified with Live Coding
    float moveSpeed = Stats["Speed"];
    UpdateMovement(moveSpeed);
}
```

## Best Practices

1. Header Files:
   - Declare interfaces only
   - Use flexible containers
   - Minimize fixed properties

2. Implementation Files:
   - Keep all logic in .cpp
   - Use container-based approach
   - Implement modular functions

3. Module Structure:
   - Separate stable and dynamic code
   - Group related functionality
   - Use clear naming conventions

## Quick Tips

1. Place new features in Gameplay module
2. Keep Core module stable
3. Use Tools module for development utilities
4. Group related systems together
5. Follow consistent naming patterns 