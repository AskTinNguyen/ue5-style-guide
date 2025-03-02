# AI Director and Spawning Pattern

## Problem Definition
Games need a dynamic system to manage AI populations, control difficulty scaling, and trigger events based on player performance and game state, while maintaining performance and creating engaging gameplay.

## Solution Approach
Implement an AI Director system that manages NPC spawning, difficulty adjustment, and event triggering through a data-driven approach with proper performance optimization.

## Core Components

### 1. AI Director
```cpp
UCLASS()
class GAME_API AAIDirector : public AActor
{
    GENERATED_BODY()

public:
    AAIDirector();

    virtual void Tick(float DeltaTime) override;
    
    UFUNCTION(BlueprintCallable, Category = "AI|Director")
    void UpdateDifficulty(float PlayerPerformanceScore);
    
    UFUNCTION(BlueprintCallable, Category = "AI|Director")
    void TriggerEvent(const FName& EventName);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "AI|Director")
    class UAIDirectorConfig* Config;

    UPROPERTY()
    class UAIPopulationManager* PopulationManager;

    UPROPERTY()
    class UAIEventManager* EventManager;

    virtual void BeginPlay() override;
    virtual void AssessGameState();
};
```

### 2. Population Manager
```cpp
UCLASS()
class GAME_API UAIPopulationManager : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "AI|Population")
    AActor* SpawnAI(TSubclassOf<AActor> AIClass, const FTransform& SpawnTransform);
    
    UFUNCTION(BlueprintCallable, Category = "AI|Population")
    void ManagePopulation(float DeltaTime);

protected:
    UPROPERTY()
    TArray<FAISpawnGroup> SpawnGroups;

    UPROPERTY()
    TArray<FAIPopulationZone> PopulationZones;

    void UpdateSpawnPoints();
    void CleanupInactiveAI();
};
```

### 3. Event Manager
```cpp
UCLASS()
class GAME_API UAIEventManager : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "AI|Events")
    void QueueEvent(const FAIEvent& Event);
    
    UFUNCTION(BlueprintCallable, Category = "AI|Events")
    void ProcessEvents(float DeltaTime);

protected:
    UPROPERTY()
    TArray<FAIEvent> EventQueue;

    UPROPERTY()
    TMap<FName, FAIEventType> RegisteredEvents;

    void EvaluateEventConditions();
    void ExecuteEvent(const FAIEvent& Event);
};
```

## Implementation Guidelines

1. **Director Implementation**
   - Implement performance monitoring
   - Add difficulty scaling
   - Handle event triggering
   - Manage AI populations

2. **Spawning System**
   - Implement spawn point management
   - Handle group spawning
   - Add spawn validation
   - Optimize spawn locations

3. **Event System**
   - Implement event queuing
   - Handle event priorities
   - Add event conditions
   - Manage event timing

## Usage Example

```cpp
// Example AI Director implementation
UCLASS()
class UAIDirectorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
    {
        Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

        // Update game state assessment
        float PlayerScore = AssessPlayerPerformance();
        UpdateDifficulty(PlayerScore);

        // Manage AI population
        if (UAIPopulationManager* PopManager = GetPopulationManager())
        {
            PopManager->ManagePopulation(DeltaTime);
        }

        // Process events
        if (UAIEventManager* EventManager = GetEventManager())
        {
            EventManager->ProcessEvents(DeltaTime);
        }
    }

protected:
    float AssessPlayerPerformance()
    {
        float Score = 0.0f;
        
        // Calculate player performance metrics
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
            {
                // Assess various performance metrics
                Score += CalculateKillDeathRatio(PS);
                Score += AssessObjectiveCompletion(PS);
                Score += EvaluateResourceManagement(PS);
            }
        }
        
        return Score;
    }
};
```

## Integration Tips

1. **Director Setup**
```cpp
// In your GameMode
void ACustomGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Setup AI Director
    if (AAIDirector* Director = GetAIDirector())
    {
        // Configure initial settings
        FAIDirectorSettings Settings;
        Settings.BaseSpawnRate = 1.0f;
        Settings.MaxConcurrentAI = 50;
        Settings.DifficultyMultiplier = 1.0f;
        
        Director->Initialize(Settings);
    }
}
```

2. **Event System Integration**
```cpp
// Example event registration
void UAIEventManager::RegisterGameEvents()
{
    // Register common events
    FAIEventType AmbushEvent;
    AmbushEvent.MinCooldown = 300.0f;  // 5 minutes
    AmbushEvent.Conditions.Add(MakeShared<FPlayerHealthCondition>(0.5f));
    RegisteredEvents.Add("Ambush", AmbushEvent);

    // Register wave events
    FAIEventType WaveEvent;
    WaveEvent.MinCooldown = 600.0f;    // 10 minutes
    WaveEvent.Conditions.Add(MakeShared<FTimedIntervalCondition>(600.0f));
    RegisteredEvents.Add("Wave", WaveEvent);
}
```

## Debugging Support

1. **Director Visualization**
```cpp
void AAIDirector::DrawDebugInfo()
{
    #if ENABLE_VISUAL_LOG
        // Draw population zones
        for (const FAIPopulationZone& Zone : PopulationManager->GetPopulationZones())
        {
            DrawDebugBox(
                GetWorld(),
                Zone.Center,
                Zone.Extent,
                FColor::Yellow,
                false,
                -1.0f,
                0,
                2.0f
            );
            
            // Draw AI count
            FString DebugText = FString::Printf(TEXT("AI Count: %d"), Zone.CurrentPopulation);
            DrawDebugString(
                GetWorld(),
                Zone.Center,
                DebugText,
                nullptr,
                FColor::White,
                0.0f
            );
        }
    #endif
}
```

2. **Event Debugging**
```cpp
void UAIEventManager::LogEventStatus()
{
    #if WITH_EDITOR
        for (const auto& Event : EventQueue)
        {
            UE_LOG(LogAIDirector, Log, TEXT("Event: %s, Priority: %f, Cooldown: %f"),
                *Event.Name.ToString(),
                Event.Priority,
                Event.CooldownRemaining);
        }
    #endif
}
```

## Common Pitfalls and Solutions

1. **Spawning Issues**
   - Validate spawn points
   - Handle spawn collisions
   - Implement spawn queuing
   - Add spawn rate limiting

2. **Performance Problems**
   - Optimize AI updates
   - Implement AI pooling
   - Use spatial partitioning
   - Add LOD system

3. **Event Management**
   - Handle event priorities
   - Manage event timing
   - Handle event conflicts
   - Implement cooldowns

## Testing

1. **Unit Tests**
```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAIDirectorTest, "AI.Director.BasicTests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FAIDirectorTest::RunTest(const FString& Parameters)
{
    // Create test director
    AAIDirector* Director = NewObject<AAIDirector>();
    
    // Test difficulty scaling
    float InitialDifficulty = Director->GetCurrentDifficulty();
    Director->UpdateDifficulty(2.0f);  // High performance
    TestTrue("Difficulty Increased", Director->GetCurrentDifficulty() > InitialDifficulty);
    
    // Test event system
    UAIEventManager* EventManager = Director->GetEventManager();
    FAIEvent TestEvent;
    TestEvent.Name = "TestEvent";
    EventManager->QueueEvent(TestEvent);
    TestTrue("Event Queued", EventManager->HasPendingEvents());
    
    return true;
}
```

## Performance Optimization

1. **Population Management**
```cpp
USTRUCT()
struct FAIPool
{
    GENERATED_BODY()

    void Initialize(TSubclassOf<AActor> InClass, int32 PoolSize)
    {
        AIClass = InClass;
        
        // Pre-spawn AI actors
        for (int32 i = 0; i < PoolSize; ++i)
        {
            AActor* NewAI = SpawnAIActor(AIClass);
            if (NewAI)
            {
                NewAI->SetActorHiddenInGame(true);
                NewAI->SetActorEnableCollision(false);
                InactivePool.Add(NewAI);
            }
        }
    }

    AActor* AcquireAI()
    {
        if (InactivePool.Num() > 0)
        {
            AActor* AI = InactivePool.Pop();
            AI->SetActorHiddenInGame(false);
            AI->SetActorEnableCollision(true);
            return AI;
        }
        return nullptr;
    }

    void ReleaseAI(AActor* AI)
    {
        if (AI)
        {
            AI->SetActorHiddenInGame(true);
            AI->SetActorEnableCollision(false);
            InactivePool.Add(AI);
        }
    }

private:
    TSubclassOf<AActor> AIClass;
    TArray<AActor*> InactivePool;
};
```

2. **Spatial Management**
```cpp
void UAIPopulationManager::OptimizePopulationZones()
{
    // Build zone quadtree
    FQuadTree ZoneTree(GetWorldBounds());
    
    // Add zones to tree
    for (const FAIPopulationZone& Zone : PopulationZones)
    {
        ZoneTree.AddElement(Zone);
    }
    
    // Query optimization
    ZoneTree.OptimizeTree();
}
```

## References

- [Unreal Engine AI Spawning Documentation](https://docs.unrealengine.com/5.0/en-US/spawning-actors-in-unreal-engine/)
- [Game AI Pro - AI Director Systems](http://www.gameaipro.com/) 