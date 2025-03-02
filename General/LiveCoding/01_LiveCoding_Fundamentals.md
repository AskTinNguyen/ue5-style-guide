# Live Coding Fundamentals

## What is Live Coding?

- Runtime C++ code modification without editor restart
- Introduced: UE 4.21 (2018)
- Preserves object state during changes
- Enables rapid iteration

## Technical Implementation

```cpp
// Example of Live-Coding compatible function
void AGameCharacter::ProcessCombat() {
    // ✓ Can modify: Function logic
    // ✓ Can modify: Local variables
    // ✓ Can modify: Algorithm flow
    float damage = CalculateDamage();
    ApplyEffects();
}
```

## Capabilities

✓ Can Modify:
- Function implementations
- Algorithms and logic flow
- Local variables
- Non-property member variables
- Static variables
- Constants

❌ Cannot Modify:
- Class structures/definitions
- UPROPERTY declarations
- Virtual function declarations
- Memory layouts
- Class inheritance
- Interface definitions

## Best Practices

### Do's
```cpp
// ✓ Good: Implementation in CPP
void ACharacter::UpdateLogic() {
    // Freely modifiable logic
}

// ✓ Good: Using containers for flexibility
UPROPERTY()
TMap<FName, float> Stats;
```

### Don'ts
```cpp
// ❌ Bad: Changing class structure
class ACharacter {
    UPROPERTY()
    float NewProperty;  // Requires rebuild
};

// ❌ Bad: Adding virtual functions
virtual void NewFunction();  // Requires rebuild
```

## Quick Tips

1. Keep implementation details in .cpp files
2. Use containers for flexible data storage
3. Plan class structures before implementation
4. Test changes frequently
5. Use Live Coding for logic/algorithm changes only 