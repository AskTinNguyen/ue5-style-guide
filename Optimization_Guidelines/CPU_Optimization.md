# CPU Optimization Guidelines

This document provides detailed guidelines for CPU optimization in AAA game development using Unreal Engine 5. Efficient CPU usage is crucial for maintaining high frame rates, responsive gameplay, and smooth performance across different platforms.

## Table of Contents
- [Performance Budgets](#performance-budgets)
- [Threading and Parallelization](#threading-and-parallelization)
- [Game Logic Optimization](#game-logic-optimization)
- [Physics and Simulation](#physics-and-simulation)
- [AI and Behavior Systems](#ai-and-behavior-systems)
- [Best Practices](#best-practices)

## Performance Budgets

### Frame Time Budgets by Platform

| Platform | Target FPS | Frame Budget | Game Thread | Render Thread | GPU Thread |
|----------|------------|--------------|-------------|---------------|------------|
| High-end PC | 60-144 | 16.67-6.94ms | 4-8ms | 4-8ms | 8-12ms |
| PS5/XSX | 60 | 16.67ms | 6ms | 6ms | 12ms |
| PS4/XB1 | 30 | 33.33ms | 12ms | 12ms | 24ms |
| Mobile | 30-60 | 33.33-16.67ms | 8-12ms | 8-12ms | 16-24ms |

### System-Level Budgets

1. **Game Logic**
   - Core gameplay: 2-3ms
   - UI/HUD: 0.5-1ms
   - Input handling: 0.2-0.5ms
   - State management: 0.2-0.5ms

2. **Physics**
   - Collision detection: 1-2ms
   - Physics simulation: 1-2ms
   - Ragdoll physics: 0.5-1ms per active ragdoll
   - Physics-based animation: 0.5-1ms

3. **AI Systems**
   - Pathfinding: 0.5-1ms per active AI
   - Behavior trees: 0.2-0.5ms per active AI
   - Perception systems: 0.5-1ms total
   - Group coordination: 0.2-0.5ms total

## Threading and Parallelization

### Task System Usage

```cpp
// Task graph example
void UCustomGameSystem::ProcessActorsAsync()
{
    // Create task graph
    FGraphEventArray Tasks;
    Tasks.Reserve(ActorsToProcess.Num());
    
    // Split work into tasks
    for (AActor* Actor : ActorsToProcess)
    {
        Tasks.Add(FFunctionGraphTask::CreateAndDispatchWhenReady([Actor]()
        {
            // Process individual actor
            Actor->ProcessData();
        }, TStatId(), nullptr, ENamedThreads::AnyBackgroundThreadNormalTask));
    }
    
    // Wait for all tasks to complete
    FTaskGraphInterface::Get().WaitUntilTasksComplete(Tasks);
}
```

### Parallel For Loops

```cpp
// ParallelFor example
void UCustomSystem::ProcessDataParallel(TArray<FCustomData>& Data)
{
    ParallelFor(Data.Num(), [&](int32 Index)
    {
        // Process individual data element
        ProcessDataElement(Data[Index]);
    });
}
```

### Thread-Safe Operations

```cpp
// Thread-safe counter example
class FThreadSafeCounter
{
private:
    FThreadSafeCounter64 Counter;
    
public:
    int64 Increment()
    {
        return Counter.Increment();
    }
    
    int64 Add(int64 Amount)
    {
        return Counter.Add(Amount);
    }
    
    int64 GetValue() const
    {
        return Counter.GetValue();
    }
};
```

## Game Logic Optimization

### Event-Driven Architecture

```cpp
// Event manager example
UCLASS()
class UOptimizedEventManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Event delegate
    DECLARE_MULTICAST_DELEGATE_OneParam(FGameEvent, const FEventData&);
    
    // Register event handler
    template<typename UserClass>
    void RegisterEventHandler(UserClass* Object, void (UserClass::*Handler)(const FEventData&))
    {
        EventDelegate.AddUObject(Object, Handler);
    }
    
    // Trigger event
    void TriggerEvent(const FEventData& EventData)
    {
        EventDelegate.Broadcast(EventData);
    }
    
private:
    FGameEvent EventDelegate;
};
```

### Tick Function Optimization

```cpp
// Optimized tick function
UCLASS()
class AOptimizedActor : public AActor
{
    GENERATED_BODY()
    
public:
    AOptimizedActor()
    {
        PrimaryActorTick.bCanEverTick = false;  // Disable tick by default
        PrimaryActorTick.TickInterval = 0.1f;   // Tick every 0.1 seconds when enabled
    }
    
    // Custom tick function
    UFUNCTION()
    void CustomTick()
    {
        // Only process if necessary
        if (ShouldProcess())
        {
            ProcessLogic();
        }
    }
    
protected:
    // Timer handle for custom tick
    FTimerHandle TickTimerHandle;
    
    // Enable custom tick when needed
    void EnableCustomTick()
    {
        GetWorld()->GetTimerManager().SetTimer(
            TickTimerHandle,
            this,
            &AOptimizedActor::CustomTick,
            0.1f,
            true
        );
    }
};
```

## Physics and Simulation

### Physics Scene Configuration

```cpp
// Physics scene settings
void UCustomPhysicsSettings::ConfigurePhysicsScene()
{
    if (UWorld* World = GetWorld())
    {
        if (FPhysScene* PhysScene = World->GetPhysicsScene())
        {
            // Configure solver iterations
            PhysScene->SetSolverIterations(8);  // Default iterations
            
            // Set up substeps
            PhysScene->SetMaxPhysicsDeltaTime(1.0f / 60.0f);  // 60 Hz physics
            PhysScene->SetMaxSubsteps(2);  // Maximum substeps
        }
    }
}
```

### Collision Optimization

```cpp
// Collision manager
UCLASS()
class UCollisionOptimizer : public UObject
{
    GENERATED_BODY()
    
public:
    // Configure collision settings
    void ConfigureCollision(UPrimitiveComponent* Component)
    {
        if (Component)
        {
            // Set collision profile
            Component->SetCollisionProfileName(TEXT("OptimizedProfile"));
            
            // Configure collision responses
            Component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            Component->SetCollisionResponseToAllChannels(ECR_Ignore);
            Component->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
        }
    }
    
    // Optimize trace channels
    void SetupTraceChannels()
    {
        // Configure custom trace channels
        UCollisionProfile::Get()->AddCustomChannels();
    }
};
```

## AI and Behavior Systems

### AI Manager Optimization

```cpp
// Optimized AI manager
UCLASS()
class UAIOptimizationManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Initialize AI systems
    void InitializeAISystems()
    {
        // Configure update frequencies
        ConfigureUpdateFrequencies();
        
        // Set up AI pools
        InitializeAIPools();
    }
    
protected:
    // Update frequency settings
    UPROPERTY(EditDefaultsOnly, Category = "AI|Performance")
    float BaseUpdateInterval = 0.1f;
    
    UPROPERTY(EditDefaultsOnly, Category = "AI|Performance")
    float DistanceBasedUpdateScale = 1.0f;
    
    // Configure different update frequencies based on distance/importance
    void ConfigureUpdateFrequencies()
    {
        // Near players: frequent updates
        NearUpdateInterval = BaseUpdateInterval;
        
        // Medium distance: reduced frequency
        MediumUpdateInterval = BaseUpdateInterval * 2.0f;
        
        // Far distance: minimal updates
        FarUpdateInterval = BaseUpdateInterval * 4.0f;
    }
};
```

### Behavior Tree Optimization

```cpp
// Optimized behavior tree task
UCLASS()
class UOptimizedBTTask : public UBTTaskNode
{
    GENERATED_BODY()
    
public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
    {
        // Cache frequently accessed data
        if (AController* Controller = OwnerComp.GetAIOwner())
        {
            if (APawn* Pawn = Controller->GetPawn())
            {
                // Use cached values for calculations
                ProcessBehavior(Pawn);
            }
        }
        
        return EBTNodeResult::Succeeded;
    }
    
protected:
    // Cached data structure
    UPROPERTY()
    FBehaviorCacheData CachedData;
    
    // Update cache when needed
    void UpdateCache();
};
```

## Best Practices

### General Guidelines

1. **Threading Best Practices**
   - Use async tasks for heavy operations
   - Implement proper thread synchronization
   - Avoid thread contention
   - Profile thread usage

2. **Game Logic**
   - Optimize tick functions
   - Use event-driven approaches
   - Implement proper caching
   - Profile critical paths

3. **Physics and Simulation**
   - Configure appropriate solver settings
   - Optimize collision detection
   - Use physics LOD systems
   - Monitor physics performance

4. **AI and Behavior**
   - Implement update frequency scaling
   - Use behavior LOD systems
   - Optimize pathfinding
   - Cache AI calculations

### Common Pitfalls

1. **Threading Issues**
   - Race conditions
   - Deadlocks
   - Thread starvation
   - Excessive synchronization

2. **Performance Problems**
   - Unoptimized loops
   - Excessive garbage collection
   - Poor cache utilization
   - Blocking operations

### Optimization Checklist

- [ ] Profile CPU usage across threads
- [ ] Implement async operations
- [ ] Optimize critical paths
- [ ] Configure physics settings
- [ ] Set up AI LOD systems
- [ ] Monitor thread utilization
- [ ] Implement proper caching
- [ ] Test on target platforms
- [ ] Document optimizations
- [ ] Review performance metrics 