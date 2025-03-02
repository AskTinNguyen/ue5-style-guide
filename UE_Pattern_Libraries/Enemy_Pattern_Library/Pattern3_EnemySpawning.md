# Pattern 3: Enemy Spawning & Management

## Problem Statement
Creating an efficient and scalable system for spawning, managing, and recycling large numbers of enemies while maintaining performance and providing tools for encounter design.

## Solution Overview
A comprehensive spawning system using object pooling, encounter management, and territory control to efficiently handle large numbers of enemies in a performant way.

## Core Components

### 1. Enemy Pool Manager

```cpp
// Enemy pool manager singleton
UCLASS()
class GAME_API UEnemyPoolManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Pool interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // Spawn interface
    UFUNCTION(BlueprintCallable, Category = "Enemy|Spawning")
    ABaseEnemy* SpawnEnemy(TSubclassOf<ABaseEnemy> EnemyClass, const FTransform& SpawnTransform);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy|Spawning")
    void ReturnToPool(ABaseEnemy* Enemy);
    
protected:
    // Pool containers
    UPROPERTY()
    TMap<TSubclassOf<ABaseEnemy>, TArray<ABaseEnemy*>> EnemyPools;
    
    // Pool configuration
    UPROPERTY(Config)
    int32 DefaultPoolSize;
    
    UPROPERTY(Config)
    int32 MaxPoolSize;
    
    // Helper functions
    void PreloadPool(TSubclassOf<ABaseEnemy> EnemyClass, int32 Count);
    ABaseEnemy* CreatePooledEnemy(TSubclassOf<ABaseEnemy> EnemyClass);
};
```

### 2. Encounter System

```cpp
// Encounter definition
USTRUCT(BlueprintType)
struct FEnemyEncounterWave
{
    GENERATED_BODY()
    
    // Wave composition
    UPROPERTY(EditDefaultsOnly)
    TArray<FEnemySpawnInfo> EnemyTypes;
    
    // Wave timing
    UPROPERTY(EditDefaultsOnly)
    float WaveDelay;
    
    // Spawn conditions
    UPROPERTY(EditDefaultsOnly)
    TArray<FSpawnCondition> SpawnConditions;
};

// Encounter manager
UCLASS(BlueprintType)
class GAME_API AEnemyEncounterManager : public AActor
{
    GENERATED_BODY()
public:
    // Encounter interface
    UFUNCTION(BlueprintCallable)
    void StartEncounter();
    
    UFUNCTION(BlueprintCallable)
    void EndEncounter();
    
    // Wave management
    UFUNCTION(BlueprintCallable)
    void TriggerNextWave();
    
protected:
    // Encounter configuration
    UPROPERTY(EditDefaultsOnly, Category = "Encounter")
    TArray<FEnemyEncounterWave> Waves;
    
    UPROPERTY(EditDefaultsOnly, Category = "Encounter")
    FEncounterSettings Settings;
    
    // State tracking
    UPROPERTY(Replicated)
    int32 CurrentWaveIndex;
    
    UPROPERTY(Replicated)
    int32 ActiveEnemyCount;
};
```

### 3. Territory System

```cpp
// Territory definition
UCLASS(BlueprintType)
class GAME_API UEnemyTerritory : public UObject
{
    GENERATED_BODY()
public:
    // Territory interface
    UFUNCTION(BlueprintCallable)
    FVector GetSpawnPoint() const;
    
    UFUNCTION(BlueprintCallable)
    bool IsInTerritory(const FVector& Location) const;
    
protected:
    // Territory bounds
    UPROPERTY(EditDefaultsOnly)
    FBox TerritoryBounds;
    
    // Spawn points
    UPROPERTY(EditDefaultsOnly)
    TArray<FSpawnPointData> SpawnPoints;
    
    // Navigation data
    UPROPERTY()
    class UNavigationSystemV1* NavSystem;
};
```

## Usage Examples

### 1. Spawning Enemies

```cpp
// Example of spawning an enemy using the pool
void AEnemyEncounterManager::SpawnWaveEnemy(const FEnemySpawnInfo& SpawnInfo)
{
    if (UEnemyPoolManager* PoolManager = GetPoolManager())
    {
        // Get spawn location
        FVector SpawnLocation = Territory->GetSpawnPoint();
        FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
        
        // Spawn enemy from pool
        if (ABaseEnemy* Enemy = PoolManager->SpawnEnemy(SpawnInfo.EnemyClass, SpawnTransform))
        {
            // Initialize enemy
            Enemy->InitializeEnemy();
            
            // Apply spawn modifiers
            ApplySpawnModifiers(Enemy, SpawnInfo.Modifiers);
            
            // Track active enemies
            ActiveEnemyCount++;
        }
    }
}
```

### 2. Managing Encounters

```cpp
// Example of encounter wave management
void AEnemyEncounterManager::TriggerNextWave()
{
    if (!HasAuthority())
    {
        return;
    }
    
    if (CurrentWaveIndex >= Waves.Num())
    {
        EndEncounter();
        return;
    }
    
    const FEnemyEncounterWave& Wave = Waves[CurrentWaveIndex];
    
    // Check wave conditions
    if (!AreWaveConditionsMet(Wave))
    {
        return;
    }
    
    // Spawn wave enemies
    for (const FEnemySpawnInfo& SpawnInfo : Wave.EnemyTypes)
    {
        SpawnWaveEnemy(SpawnInfo);
    }
    
    // Setup next wave timer
    FTimerHandle NextWaveTimer;
    GetWorldTimerManager().SetTimer(NextWaveTimer, this,
        &AEnemyEncounterManager::CheckWaveCompletion, 1.0f, true);
    
    CurrentWaveIndex++;
}
```

### 3. Territory Management

```cpp
// Example of territory management
FVector UEnemyTerritory::GetSpawnPoint() const
{
    // Try to find valid spawn point
    for (const FSpawnPointData& SpawnPoint : SpawnPoints)
    {
        if (IsSpawnPointValid(SpawnPoint))
        {
            return SpawnPoint.Location;
        }
    }
    
    // Fallback to random point in territory
    return GetRandomPointInTerritory();
}

bool UEnemyTerritory::IsSpawnPointValid(const FSpawnPointData& SpawnPoint) const
{
    if (!NavSystem)
    {
        return false;
    }
    
    // Check navigation
    FNavLocation NavLocation;
    if (!NavSystem->ProjectPointToNavigation(SpawnPoint.Location, NavLocation))
    {
        return false;
    }
    
    // Check visibility
    TArray<AActor*> OverlappingActors;
    UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SpawnPoint.Location,
        100.0f, TArray<TEnumAsByte<EObjectTypeQuery>>(), AActor::StaticClass(),
        TArray<AActor*>(), OverlappingActors);
    
    return OverlappingActors.Num() == 0;
}
```

## Best Practices

1. **Pool Management**
   - Pre-allocate pools based on expected usage
   - Implement smart pool sizing
   - Handle pool exhaustion gracefully

2. **Encounter Design**
   - Balance wave composition
   - Consider player skill and progression
   - Implement proper difficulty scaling

3. **Territory Control**
   - Validate spawn points
   - Consider navigation constraints
   - Implement proper bounds checking

## Integration Guidelines

1. **With Level Streaming**
   ```cpp
   // Example of level streaming integration
   void UEnemyPoolManager::OnLevelChanged(ULevel* NewLevel)
   {
       // Clear pools for unloaded enemy types
       TArray<TSubclassOf<ABaseEnemy>> UnusedTypes;
       EnemyPools.GetKeys(UnusedTypes);
       
       for (TSubclassOf<ABaseEnemy> EnemyClass : UnusedTypes)
       {
           if (!IsEnemyTypeNeeded(EnemyClass, NewLevel))
           {
               ClearPool(EnemyClass);
           }
       }
       
       // Preload pools for new enemy types
       TArray<TSubclassOf<ABaseEnemy>> NewTypes = GetLevelEnemyTypes(NewLevel);
       for (TSubclassOf<ABaseEnemy> EnemyClass : NewTypes)
       {
           PreloadPool(EnemyClass, DefaultPoolSize);
       }
   }
   ```

2. **With AI System**
   ```cpp
   // Example of AI integration
   void AEnemyEncounterManager::OnEnemySpawned(ABaseEnemy* Enemy)
   {
       if (UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld()))
       {
           // Register with perception system
           PerceptionSystem->RegisterSource(*Enemy);
       }
       
       // Setup AI controller
       if (AAIController* AIController = Cast<AAIController>(Enemy->GetController()))
       {
           // Initialize AI
           AIController->RunBehaviorTree(Enemy->GetDefaultBehaviorTree());
       }
   }
   ```

## Common Pitfalls

1. **Pool Management**
   - Memory leaks from unclaimed pool objects
   - Pool fragmentation
   - Inefficient pool sizing

2. **Encounter Balance**
   - Overwhelming wave composition
   - Poor spawn timing
   - Unfair difficulty spikes

3. **Performance Issues**
   - Too many active enemies
   - Inefficient spawn point validation
   - Poor territory management

## Testing Strategy

1. **Pool Testing**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyPoolTest, "Enemy.Spawning.Pool",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyPoolTest::RunTest(const FString& Parameters)
   {
       // Test pool creation
       UEnemyPoolManager* PoolManager = CreateTestPoolManager();
       TestNotNull("Pool manager should be created", PoolManager);
       
       // Test enemy spawning
       ABaseEnemy* Enemy = PoolManager->SpawnEnemy(TestEnemyClass, FTransform::Identity);
       TestNotNull("Enemy should be spawned from pool", Enemy);
       
       // Test pool recycling
       PoolManager->ReturnToPool(Enemy);
       TestEqual("Pool should contain returned enemy", 
           PoolManager->GetPoolSize(TestEnemyClass), 1);
       
       return true;
   }
   ```

2. **Encounter Testing**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncounterTest, "Enemy.Spawning.Encounter",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEncounterTest::RunTest(const FString& Parameters)
   {
       // Test encounter initialization
       AEnemyEncounterManager* EncounterManager = CreateTestEncounter();
       TestNotNull("Encounter manager should be created", EncounterManager);
       
       // Test wave progression
       EncounterManager->StartEncounter();
       TestEqual("Should be on first wave", EncounterManager->GetCurrentWaveIndex(), 0);
       
       // Test enemy spawning
       TestEqual("Should spawn correct number of enemies",
           EncounterManager->GetActiveEnemyCount(),
           TestWave.EnemyTypes.Num());
       
       return true;
   }
   ```

## Debugging Tools

1. **Visual Debugging**
   ```cpp
   void AEnemyEncounterManager::DrawDebugInfo()
   {
       if (CVarShowEncounterDebug.GetValueOnGameThread())
       {
           // Draw territory bounds
           if (Territory)
           {
               DrawDebugBox(GetWorld(), Territory->GetTerritoryBounds().GetCenter(),
                   Territory->GetTerritoryBounds().GetExtent(),
                   FColor::Green, false, -1.0f, 0, 2.0f);
           }
           
           // Draw spawn points
           for (const FSpawnPointData& SpawnPoint : Territory->GetSpawnPoints())
           {
               DrawDebugSphere(GetWorld(), SpawnPoint.Location,
                   25.0f, 12, FColor::Blue, false, -1.0f, 0, 2.0f);
           }
           
           // Draw encounter info
           FString EncounterInfo = FString::Printf(TEXT("Wave: %d/%d\nActive Enemies: %d"),
               CurrentWaveIndex + 1, Waves.Num(), ActiveEnemyCount);
           DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 100),
               EncounterInfo, nullptr, FColor::White, 0.0f);
       }
   }
   ```

2. **Performance Monitoring**
   ```cpp
   void UEnemyPoolManager::LogPoolStats()
   {
       if (CVarLogPoolStats.GetValueOnGameThread())
       {
           for (const auto& PoolPair : EnemyPools)
           {
               UE_LOG(LogEnemyPool, Log, TEXT("Pool for %s: Active=%d, Available=%d"),
                   *PoolPair.Key->GetName(),
                   GetActiveCount(PoolPair.Key),
                   PoolPair.Value.Num());
           }
       }
   }
   ```

## Network Considerations

1. **Spawn Synchronization**
   ```cpp
   UFUNCTION(NetMulticast, Reliable)
   void AEnemyEncounterManager::MulticastOnEnemySpawned(ABaseEnemy* Enemy)
   {
       if (!IsRunningDedicatedServer())
       {
           // Setup client-side effects
           if (Enemy)
           {
               Enemy->PlaySpawnEffects();
           }
       }
   }
   ```

2. **Territory Replication**
   ```cpp
   void AEnemyEncounterManager::GetLifetimeReplicatedProps(
       TArray<FLifetimeProperty>& OutLifetimeProps) const
   {
       Super::GetLifetimeReplicatedProps(OutLifetimeProps);
       
       DOREPLIFETIME(AEnemyEncounterManager, CurrentWaveIndex);
       DOREPLIFETIME(AEnemyEncounterManager, ActiveEnemyCount);
   }
   ```

## Credits

This pattern is part of the Enemy Pattern Library, designed to provide a robust foundation for AAA game enemy spawning and management systems in Unreal Engine 5. 