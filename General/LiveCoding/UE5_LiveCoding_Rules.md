# UE5 Live Coding Rules

## 1. Header File Rules

### 1.1 Class Declarations
- ✓ Declare interfaces and virtual functions only
- ✓ Use UPROPERTY() for reflection
- ❌ NO implementation in headers
- ❌ NO FORCEINLINE functions with logic

### 1.2 Property Declarations
- ✓ Use container-based properties (TMap, TArray)
- ✓ Use smart pointers (TSharedPtr, TWeakObjectPtr)
- ❌ NO fixed property declarations unless absolutely necessary
- ❌ NO direct member variables that might need future expansion

### 1.3 Interface Design
- ✓ Design stable, generic interfaces
- ✓ Use FName-based action systems
- ❌ NO frequent interface changes
- ❌ NO tightly coupled interfaces

## 2. Implementation Rules

### 2.1 CPP File Organization
- ✓ Keep all logic in .cpp files
- ✓ Use container-based data access
- ✓ Implement modular functions
- ✓ Use delegate systems for extensibility

### 2.2 State Management
```cpp
// ✓ CORRECT: Flexible state system
UPROPERTY()
TMap<FName, FStateData> States;

// ❌ INCORRECT: Fixed state variables
float Health;
float Mana;
```

### 2.3 Memory Management
```cpp
// ✓ CORRECT: Smart pointer usage
UPROPERTY()
TArray<TWeakObjectPtr<AActor>> TrackedActors;

// ❌ INCORRECT: Raw pointers
TArray<AActor*> TrackedActors;
```

## 3. Container Usage Rules

### 3.1 Data Storage
```cpp
// ✓ CORRECT: Flexible storage
TMap<FName, float> Stats;
TMap<FName, FStateData> States;
TMap<FName, TSharedPtr<FGameData>> DataSystem;

// ❌ INCORRECT: Fixed structures
struct FPlayerStats {
    float Health;
    float Mana;
};
```

### 3.2 Performance Optimization
```cpp
// ✓ CORRECT: Pre-allocation
TArray<FItem> Items;
Items.Reserve(ExpectedSize);

// ❌ INCORRECT: Dynamic allocation
TArray<FItem> Items;
for(auto& Item : WorldItems) {
    Items.Add(Item); // Potential reallocation
}
```

## 4. System Design Rules

### 4.1 Module Organization
```plaintext
✓ CORRECT Structure:
Source/
├── Core/           # Stable systems
├── Gameplay/       # Live-coding friendly
└── Tools/          # Development utilities
```

### 4.2 Feature Implementation
```cpp
// ✓ CORRECT: Data-driven approach
UPROPERTY()
TMap<FName, FFeatureData> Features;

// ❌ INCORRECT: Hard-coded features
void ProcessFeatureA();
void ProcessFeatureB();
```

## 5. Error Handling Rules

### 5.1 Null Checking
```cpp
// ✓ CORRECT: Robust error handling
if (!ensureMsgf(IsValid(GameState), TEXT("Invalid state"))) {
    RecoverState();
    return;
}

// ❌ INCORRECT: No error handling
GameState->Process();
```

### 5.2 State Recovery
```cpp
// ✓ CORRECT: State preservation
UPROPERTY()
TMap<FName, FStateData> StateBackup;

// Implement state recovery
void RecoverState() {
    CurrentState = StateBackup.FindRef(LastValidState);
}
```

## 6. Documentation Rules

### 6.1 Code Comments
```cpp
/**
 * ✓ CORRECT: Clear documentation
 * @brief Live-coding friendly system
 * @note Modify implementation in .cpp only
 * @note Use containers for flexibility
 */
```

### 6.2 Implementation Notes
```cpp
// ✓ CORRECT: Implementation notes
void ProcessLogic() {
    // Live-coding friendly implementation
    // Modify this section for new features
    // Use containers for data access
}
```

## 7. Testing Rules

### 7.1 Test Implementation
```cpp
// ✓ CORRECT: Modular testing
void TestSystem() {
    SetupTestData();
    ExecuteTest();
    ValidateResults();
    CleanupTest();
}
```

### 7.2 Validation
- ✓ Test after each live code change
- ✓ Verify state preservation
- ✓ Check error handling
- ✓ Validate performance