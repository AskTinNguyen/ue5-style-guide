# Unreal Engine 5 C++ Style Guide
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