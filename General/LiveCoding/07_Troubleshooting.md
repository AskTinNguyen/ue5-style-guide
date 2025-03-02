# Live Coding Troubleshooting Guide

## Common Issues and Solutions

### 1. Property Changes Not Applying

#### Problem
```cpp
// ❌ Problem: Property changes requiring rebuild
UPROPERTY()
float Health;  // Adding new property
```

#### Solution
```cpp
// ✓ Solution: Use flexible container
UPROPERTY()
TMap<FName, float> Stats;  // Add health to existing container

// Access in implementation
float GetHealth() {
    return Stats["Health"];
}
```

### 2. Memory Layout Issues

#### Problem
```cpp
// ❌ Problem: Changing class structure
class ACharacter {
    // Adding new member variables
    float NewVariable;  // Requires rebuild
};
```

#### Solution
```cpp
// ✓ Solution: Use data container
class ACharacter {
    UPROPERTY()
    TMap<FName, FVariableContainer> DynamicVariables;
};

// Access in implementation
void UpdateVariable() {
    DynamicVariables["NewVar"].Update();
}
```

### 3. Performance Problems

#### Problem
```cpp
// ❌ Problem: Inefficient container usage
void ProcessItems() {
    // Constant reallocation
    TArray<FItem> Items;
    for (auto& Item : WorldItems) {
        Items.Add(Item);  // May cause reallocation
    }
}
```

#### Solution
```cpp
// ✓ Solution: Pre-allocated containers
void ProcessItems() {
    // Pre-allocate space
    TArray<FItem> Items;
    Items.Reserve(WorldItems.Num());
    
    // No reallocation needed
    for (auto& Item : WorldItems) {
        Items.Add(Item);
    }
}
```

### 4. Virtual Function Changes

#### Problem
```cpp
// ❌ Problem: Modifying virtual functions
class ABaseCharacter {
    virtual void NewFunction();  // Requires rebuild
};
```

#### Solution
```cpp
// ✓ Solution: Use delegate system
class ABaseCharacter {
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionDelegate);
    
    UPROPERTY()
    FOnActionDelegate OnAction;
};

// Implementation can be modified
void HandleAction() {
    OnAction.Broadcast();
}
```

### 5. Interface Changes

#### Problem
```cpp
// ❌ Problem: Modifying interfaces
class IGameInterface {
    virtual void NewMethod() = 0;  // Requires rebuild
};
```

#### Solution
```cpp
// ✓ Solution: Use dynamic dispatch
class IGameInterface {
    // Stable interface
    virtual void ProcessAction(const FName& ActionName) = 0;
};

// Implementation can be modified
void ProcessAction(const FName& ActionName) {
    if (ActionName == "NewAction") {
        HandleNewAction();
    }
}
```

## Quick Fixes

### 1. Live Coding Not Working
```plaintext
1. Check Hot Reload is enabled
2. Verify file is saved
3. Clear derived data cache
4. Restart editor if needed
```

### 2. Compilation Errors
```plaintext
1. Check error messages
2. Verify includes
3. Check class declarations
4. Review property macros
```

### 3. State Loss
```plaintext
1. Use proper containers
2. Implement state saving
3. Use weak pointers
4. Handle null cases
```

## Best Practices for Prevention

### 1. Code Structure
```cpp
// ✓ Good: Modular design
class UGameSystem {
    // Stable interface
    virtual void ProcessAction(const FName& ActionName);
    
    // Flexible storage
    UPROPERTY()
    TMap<FName, FActionData> Actions;
};
```

### 2. State Management
```cpp
// ✓ Good: State preservation
class UStateManager {
    // Persistent state
    UPROPERTY()
    TMap<FName, FStateData> StateMap;
    
    // State backup
    void BackupState() {
        // Implementation
    }
};
```

### 3. Error Handling
```cpp
// ✓ Good: Robust error handling
void ProcessGameLogic() {
    if (!ensureMsgf(IsValid(GameState), TEXT("Invalid game state"))) {
        RecoverGameState();
        return;
    }
    // Continue processing
}
```