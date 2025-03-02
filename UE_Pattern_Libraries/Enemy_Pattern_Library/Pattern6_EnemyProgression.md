# Pattern 6: Enemy Progression & Scaling

## Problem Statement
Creating a flexible and balanced system for managing enemy difficulty, level scaling, and dynamic adjustments based on game progression and player performance.

## Solution Overview
A comprehensive progression system that combines data-driven difficulty curves, dynamic stat scaling, and adaptive behavior modification to create challenging but fair enemy encounters.

## Core Components

### 1. Progression Manager

```cpp
// Global progression management
UCLASS()
class GAME_API UEnemyProgressionManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Initialization
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // Progression interface
    UFUNCTION(BlueprintCallable, Category = "Progression")
    float GetDifficultyMultiplier(const FName& StatName, float BaseValue) const;
    
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void UpdateGlobalDifficulty(float DeltaTime);
    
protected:
    // Difficulty state
    UPROPERTY()
    float GlobalDifficultyLevel;
    
    UPROPERTY()
    FEnemyDifficultySettings DifficultySettings;
    
    // Progression curves
    UPROPERTY()
    UCurveFloat* DifficultyScalingCurve;
    
    // Helper functions
    float CalculatePlayerPerformance() const;
    void AdjustDifficultyBasedOnPerformance();
};
```

### 2. Scaling System

```cpp
// Stat scaling configuration
USTRUCT(BlueprintType)
struct FEnemyStatScaling
{
    GENERATED_BODY()
    
    // Base scaling
    UPROPERTY(EditDefaultsOnly)
    float BaseMultiplier;
    
    // Level-based scaling
    UPROPERTY(EditDefaultsOnly)
    UCurveFloat* LevelScalingCurve;
    
    // Difficulty-based scaling
    UPROPERTY(EditDefaultsOnly)
    UCurveFloat* DifficultyScalingCurve;
    
    // Performance-based scaling
    UPROPERTY(EditDefaultsOnly)
    float PerformanceScalingFactor;
};

// Enemy scaling component
UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class GAME_API UEnemyScalingComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    // Scaling interface
    virtual void InitializeScaling();
    virtual void UpdateScaling();
    
    // Stat modification
    UFUNCTION(BlueprintCallable, Category = "Scaling")
    float GetScaledStatValue(const FName& StatName, float BaseValue) const;
    
protected:
    // Scaling configuration
    UPROPERTY(EditDefaultsOnly, Category = "Scaling")
    TMap<FName, FEnemyStatScaling> StatScalingMap;
    
    // Current scaling state
    UPROPERTY(Replicated)
    float CurrentScalingLevel;
    
    // Helper functions
    float CalculateStatMultiplier(const FEnemyStatScaling& Scaling) const;
    void UpdateScalingLevel();
};
```

### 3. Difficulty Adjustment

```cpp
// Difficulty adjustment settings
USTRUCT(BlueprintType)
struct FEnemyDifficultySettings
{
    GENERATED_BODY()
    
    // Adjustment thresholds
    UPROPERTY(EditDefaultsOnly)
    float MinDifficulty;
    
    UPROPERTY(EditDefaultsOnly)
    float MaxDifficulty;
    
    // Adjustment rates
    UPROPERTY(EditDefaultsOnly)
    float DifficultyAdjustmentRate;
    
    UPROPERTY(EditDefaultsOnly)
    float PerformanceWeight;
    
    // Behavior modification
    UPROPERTY(EditDefaultsOnly)
    TMap<FName, FCurveTableRowHandle> BehaviorAdjustments;
};

// Difficulty adjustment component
UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class GAME_API UEnemyDifficultyComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    // Difficulty interface
    virtual void InitializeDifficulty();
    virtual void UpdateDifficulty();
    
    // Behavior modification
    UFUNCTION(BlueprintCallable, Category = "Difficulty")
    float GetModifiedBehaviorValue(const FName& BehaviorName, float BaseValue) const;
    
protected:
    // Difficulty configuration
    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    FEnemyDifficultySettings Settings;
    
    // Current difficulty state
    UPROPERTY(Replicated)
    float CurrentDifficultyLevel;
    
    // Helper functions
    void AdjustDifficultyBasedOnPerformance();
    float CalculateBehaviorMultiplier(const FName& BehaviorName) const;
};
```

## Usage Examples

### 1. Implementing Stat Scaling

```cpp
// Example of stat scaling implementation
float UEnemyScalingComponent::GetScaledStatValue(const FName& StatName, float BaseValue) const
{
    const FEnemyStatScaling* Scaling = StatScalingMap.Find(StatName);
    if (!Scaling)
    {
        return BaseValue;
    }
    
    // Calculate final multiplier
    float Multiplier = Scaling->BaseMultiplier;
    
    // Apply level scaling
    if (Scaling->LevelScalingCurve)
    {
        Multiplier *= Scaling->LevelScalingCurve->GetFloatValue(CurrentScalingLevel);
    }
    
    // Apply difficulty scaling
    if (Scaling->DifficultyScalingCurve)
    {
        if (UEnemyProgressionManager* ProgressionManager = GetProgressionManager())
        {
            float DifficultyLevel = ProgressionManager->GetGlobalDifficultyLevel();
            Multiplier *= Scaling->DifficultyScalingCurve->GetFloatValue(DifficultyLevel);
        }
    }
    
    // Apply performance scaling
    if (Scaling->PerformanceScalingFactor != 0.0f)
    {
        float PlayerPerformance = CalculatePlayerPerformance();
        Multiplier *= (1.0f + PlayerPerformance * Scaling->PerformanceScalingFactor);
    }
    
    return BaseValue * Multiplier;
}
```

### 2. Managing Difficulty

```cpp
// Example of difficulty management
void UEnemyProgressionManager::UpdateGlobalDifficulty(float DeltaTime)
{
    // Calculate base difficulty progression
    float TimeDifficulty = DifficultyScalingCurve->GetFloatValue(GetGameTime());
    
    // Calculate performance-based adjustment
    float PerformanceModifier = CalculatePlayerPerformance() * DifficultySettings.PerformanceWeight;
    
    // Update global difficulty
    GlobalDifficultyLevel = FMath::Clamp(
        TimeDifficulty + PerformanceModifier,
        DifficultySettings.MinDifficulty,
        DifficultySettings.MaxDifficulty
    );
    
    // Broadcast difficulty update
    OnDifficultyUpdated.Broadcast(GlobalDifficultyLevel);
}
```

### 3. Behavior Modification

```cpp
// Example of behavior modification
float UEnemyDifficultyComponent::GetModifiedBehaviorValue(const FName& BehaviorName, float BaseValue) const
{
    // Get behavior adjustment curve
    const FCurveTableRowHandle* AdjustmentCurve = Settings.BehaviorAdjustments.Find(BehaviorName);
    if (!AdjustmentCurve || !AdjustmentCurve->IsValid())
    {
        return BaseValue;
    }
    
    // Calculate difficulty multiplier
    float DifficultyMultiplier = AdjustmentCurve->Curve->GetFloatValue(CurrentDifficultyLevel);
    
    // Apply modification
    return BaseValue * DifficultyMultiplier;
}
```

## Best Practices

1. **Scaling Design**
   - Use smooth progression curves
   - Implement proper value clamping
   - Consider multiple scaling factors

2. **Difficulty Balance**
   - Implement gradual difficulty increases
   - Provide clear feedback
   - Allow for player adaptation

3. **Performance Monitoring**
   - Track relevant metrics
   - Implement proper averaging
   - Consider multiple factors

## Integration Guidelines

1. **With Combat System**
   ```cpp
   // Example of combat system integration
   void ABaseEnemy::CalculateDamageOutput(float& OutDamage)
   {
       if (UEnemyScalingComponent* ScalingComp = GetScalingComponent())
       {
           // Apply damage scaling
           OutDamage = ScalingComp->GetScaledStatValue("Damage", OutDamage);
       }
       
       if (UEnemyDifficultyComponent* DifficultyComp = GetDifficultyComponent())
       {
           // Apply difficulty-based modification
           OutDamage = DifficultyComp->GetModifiedBehaviorValue("DamageOutput", OutDamage);
       }
   }
   ```

2. **With AI System**
   ```cpp
   // Example of AI integration
   void AEnemyAIController::UpdateBehaviorParameters()
   {
       if (UEnemyDifficultyComponent* DifficultyComp = GetPawn()->FindComponentByClass<UEnemyDifficultyComponent>())
       {
           if (UBlackboardComponent* Blackboard = GetBlackboardComponent())
           {
               // Update AI parameters based on difficulty
               float AggressionBase = Blackboard->GetValueAsFloat("BaseAggression");
               float ModifiedAggression = DifficultyComp->GetModifiedBehaviorValue("Aggression", AggressionBase);
               Blackboard->SetValueAsFloat("Aggression", ModifiedAggression);
               
               float ReactionBase = Blackboard->GetValueAsFloat("BaseReactionTime");
               float ModifiedReaction = DifficultyComp->GetModifiedBehaviorValue("ReactionTime", ReactionBase);
               Blackboard->SetValueAsFloat("ReactionTime", ModifiedReaction);
           }
       }
   }
   ```

## Common Pitfalls

1. **Scaling Issues**
   - Exponential scaling problems
   - Unbalanced stat interactions
   - Poor progression curves

2. **Difficulty Spikes**
   - Sudden difficulty increases
   - Poor player feedback
   - Unfair challenges

3. **Performance Impact**
   - Expensive calculations
   - Frequent updates
   - Poor optimization

## Testing Strategy

1. **Scaling Tests**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyScalingTest, "Enemy.Progression.Scaling",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyScalingTest::RunTest(const FString& Parameters)
   {
       // Test base scaling
       UEnemyScalingComponent* ScalingComp = CreateTestScalingComponent();
       float BaseValue = 100.0f;
       float ScaledValue = ScalingComp->GetScaledStatValue("Health", BaseValue);
       TestTrue("Scaled value should be higher than base", ScaledValue > BaseValue);
       
       // Test level scaling
       ScalingComp->SetScalingLevel(10.0f);
       float HighLevelValue = ScalingComp->GetScaledStatValue("Health", BaseValue);
       TestTrue("Higher level should increase scaling", HighLevelValue > ScaledValue);
       
       return true;
   }
   ```

2. **Difficulty Tests**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyDifficultyTest, "Enemy.Progression.Difficulty",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyDifficultyTest::RunTest(const FString& Parameters)
   {
       // Test difficulty adjustment
       UEnemyDifficultyComponent* DifficultyComp = CreateTestDifficultyComponent();
       
       // Test behavior modification
       float BaseAggression = 1.0f;
       float ModifiedAggression = DifficultyComp->GetModifiedBehaviorValue("Aggression", BaseAggression);
       TestTrue("Difficulty should affect behavior", ModifiedAggression != BaseAggression);
       
       // Test difficulty limits
       DifficultyComp->SetDifficultyLevel(100.0f);
       TestEqual("Difficulty should be clamped",
           DifficultyComp->GetDifficultyLevel(),
           DifficultyComp->GetSettings().MaxDifficulty);
       
       return true;
   }
   ```

## Debugging Tools

1. **Progression Visualization**
   ```cpp
   void UEnemyProgressionManager::DrawDebugInfo()
   {
       if (CVarShowProgressionDebug.GetValueOnGameThread())
       {
           // Draw global difficulty
           GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow,
               FString::Printf(TEXT("Global Difficulty: %.2f"), GlobalDifficultyLevel));
           
           // Draw performance metrics
           float Performance = CalculatePlayerPerformance();
           GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan,
               FString::Printf(TEXT("Player Performance: %.2f"), Performance));
           
           // Draw scaling info
           TArray<FString> ScalingInfo;
           GetActiveScalingInfo(ScalingInfo);
           for (const FString& Info : ScalingInfo)
           {
               GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, Info);
           }
       }
   }
   ```

2. **Stat Monitoring**
   ```cpp
   void UEnemyScalingComponent::LogScalingStats()
   {
       if (CVarLogScalingStats.GetValueOnGameThread())
       {
           UE_LOG(LogEnemyProgression, Log, TEXT("Scaling Stats for %s:"), *GetOwner()->GetName());
           
           for (const auto& StatPair : StatScalingMap)
           {
               float BaseValue = 100.0f; // Example base value
               float ScaledValue = GetScaledStatValue(StatPair.Key, BaseValue);
               
               UE_LOG(LogEnemyProgression, Log, TEXT("  %s: Base=%.1f, Scaled=%.1f (x%.2f)"),
                   *StatPair.Key.ToString(),
                   BaseValue,
                   ScaledValue,
                   ScaledValue / BaseValue);
           }
       }
   }
   ```

## Performance Considerations

1. **Calculation Optimization**
   ```cpp
   // Example of optimized scaling calculation
   float UEnemyScalingComponent::CalculateStatMultiplier(const FEnemyStatScaling& Scaling) const
   {
       // Cache frequently used values
       static float CachedDifficultyLevel = -1.0f;
       static float CachedDifficultyMultiplier = 1.0f;
       
       // Update cache if needed
       if (UEnemyProgressionManager* ProgressionManager = GetProgressionManager())
       {
           float CurrentDifficulty = ProgressionManager->GetGlobalDifficultyLevel();
           if (CurrentDifficulty != CachedDifficultyLevel)
           {
               CachedDifficultyLevel = CurrentDifficulty;
               CachedDifficultyMultiplier = Scaling.DifficultyScalingCurve->GetFloatValue(CurrentDifficulty);
           }
       }
       
       return Scaling.BaseMultiplier * CachedDifficultyMultiplier;
   }
   ```

2. **Update Frequency**
   ```cpp
   void UEnemyDifficultyComponent::UpdateDifficulty()
   {
       // Only update at appropriate intervals
       static float LastUpdateTime = 0.0f;
       float CurrentTime = GetWorld()->GetTimeSeconds();
       
       if (CurrentTime - LastUpdateTime < Settings.UpdateInterval)
       {
           return;
       }
       
       // Perform update
       AdjustDifficultyBasedOnPerformance();
       LastUpdateTime = CurrentTime;
   }
   ```

## Credits

This pattern is part of the Enemy Pattern Library, designed to provide a robust foundation for enemy progression and scaling systems in Unreal Engine 5. 