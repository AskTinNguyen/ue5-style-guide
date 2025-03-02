# Perception System Pattern

## Problem Definition
AI characters need a robust and efficient way to perceive and react to their environment, including sight, hearing, and other custom senses, while maintaining performance and scalability.

## Solution Approach
Implement a modular perception system that handles multiple sense types, with proper optimization for large numbers of AI agents and stimuli.

## Core Components

### 1. Custom Sense Configuration
```cpp
UCLASS()
class GAME_API UAISenseConfig_Custom : public UAISenseConfig
{
    GENERATED_BODY()

public:
    UAISenseConfig_Custom();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear)
    TSubclassOf<UAISense_Custom> Implementation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense")
    float DetectionRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense")
    float LoseSightRadius;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense")
    float PeripheralVisionAngleDegrees;

    virtual TSubclassOf<UAISense> GetSenseImplementation() const override;
};
```

### 2. Custom Sense Implementation
```cpp
UCLASS()
class GAME_API UAISense_Custom : public UAISense
{
    GENERATED_BODY()

public:
    UAISense_Custom();

    virtual void OnNewPawn(APawn* NewPawn) override;
    virtual void OnRegister() override;

protected:
    virtual float Update() override;

    UPROPERTY()
    TArray<FAISenseCustomInfo> SenseInfos;

    void UpdateSenseInfo(const FAISenseCustomInfo& Info);
};
```

### 3. Perception Component Extension
```cpp
UCLASS()
class GAME_API UCustomAIPerceptionComponent : public UAIPerceptionComponent
{
    GENERATED_BODY()

public:
    UCustomAIPerceptionComponent();

    UFUNCTION(BlueprintCallable, Category = "AI|Perception")
    void ConfigureCustomSense(TSubclassOf<UAISense_Custom> SenseClass);

    UFUNCTION(BlueprintCallable, Category = "AI|Perception")
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

protected:
    virtual void BeginPlay() override;
    
    UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
    TArray<FAICustomSenseConfig> CustomSenseConfigs;
};
```

## Implementation Guidelines

1. **Sense Configuration**
   - Configure detection parameters
   - Set up perception ranges
   - Define detection conditions
   - Implement LOD system

2. **Performance Optimization**
   - Use spatial partitioning
   - Implement perception caching
   - Optimize update frequency
   - Use efficient data structures

3. **Integration**
   - Connect with behavior system
   - Handle multiple senses
   - Implement priority system
   - Add debugging support

## Usage Example

```cpp
// Example custom sight sense
UCLASS()
class UAISense_CustomSight : public UAISense_Custom
{
    GENERATED_BODY()

public:
    virtual float Update() override
    {
        float MinUpdateInterval = 0.2f;

        for (const auto& SightInfo : SenseInfos)
        {
            // Update sight perception
            UpdateSightPerception(SightInfo);
        }

        return MinUpdateInterval;
    }

protected:
    void UpdateSightPerception(const FAISenseCustomInfo& Info)
    {
        AAIController* AIController = Cast<AAIController>(Info.Observer);
        if (!AIController) return;

        // Perform sight checks
        FVector ViewPoint;
        FRotator ViewRotation;
        AIController->GetActorEyesViewPoint(ViewPoint, ViewRotation);

        // Check visible actors
        TArray<FHitResult> HitResults;
        PerformVisionCone(ViewPoint, ViewRotation, HitResults);

        // Process results
        ProcessPerceptionResults(AIController, HitResults);
    }
};
```

## Integration Tips

1. **Component Setup**
```cpp
// In your AI Controller
void AAIControllerBase::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // Setup perception component
    if (UCustomAIPerceptionComponent* PerceptionComp = GetPerceptionComponent())
    {
        PerceptionComp->ConfigureCustomSense(UAISense_CustomSight::StaticClass());
        PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnPerceptionUpdate);
    }
}
```

2. **Perception Event Handling**
```cpp
void AAIControllerBase::OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        if (UCustomAIPerceptionComponent* PerceptionComp = GetPerceptionComponent())
        {
            FActorPerceptionBlueprintInfo Info;
            PerceptionComp->GetActorInfo(Actor, Info);

            // Process perception info
            ProcessPerceptionInfo(Info);
        }
    }
}
```

## Debugging Support

1. **Visual Debugging**
```cpp
void UCustomAIPerceptionComponent::DrawDebugPerception()
{
    #if ENABLE_VISUAL_LOG
        AAIController* AIController = Cast<AAIController>(GetOwner());
        if (!AIController) return;

        // Draw perception ranges
        FVector Location = AIController->GetPawn()->GetActorLocation();
        
        // Draw sight cone
        if (const UAISenseConfig_Sight* SightConfig = GetSenseConfig<UAISenseConfig_Sight>())
        {
            DrawDebugCone(
                GetWorld(),
                Location,
                AIController->GetControlRotation().Vector(),
                SightConfig->SightRadius,
                FMath::DegreesToRadians(SightConfig->PeripheralVisionAngleDegrees),
                16,
                FColor::Yellow,
                false,
                0.0f
            );
        }
    #endif
}
```

2. **Performance Monitoring**
```cpp
void UCustomAIPerceptionComponent::MonitorPerformance()
{
    #if WITH_EDITOR
        float UpdateTime = FPlatformTime::Seconds();
        
        // Log performance metrics
        UE_LOG(LogAIPerception, Log, TEXT("Perception Update Time: %f ms"), 
            (FPlatformTime::Seconds() - UpdateTime) * 1000.0f);
    #endif
}
```

## Common Pitfalls and Solutions

1. **Performance Issues**
   - Implement proper LOD system
   - Use efficient spatial queries
   - Optimize update frequencies
   - Cache perception results

2. **Integration Problems**
   - Handle multiple sense types
   - Manage sense priorities
   - Handle perception conflicts
   - Implement proper cleanup

3. **Debugging**
   - Add visual debugging
   - Implement logging system
   - Monitor performance
   - Add validation checks

## Testing

1. **Unit Tests**
```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPerceptionTest, "AI.Perception.BasicTests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPerceptionTest::RunTest(const FString& Parameters)
{
    // Create test component
    UCustomAIPerceptionComponent* PerceptionComp = NewObject<UCustomAIPerceptionComponent>();
    
    // Test sight configuration
    UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>();
    PerceptionComp->ConfigureSense(SightConfig);
    
    // Verify configuration
    TestTrue("Sight Sense Configured", PerceptionComp->GetSenseConfig<UAISenseConfig_Sight>() != nullptr);
    
    return true;
}
```

## Performance Optimization

1. **Spatial Partitioning**
```cpp
UPROPERTY()
class FPerceptionGrid
{
public:
    void UpdateGrid(const TArray<FAIPerceptionGridCell>& Cells);
    void QueryPerceivers(const FVector& Location, float Radius, TArray<AAIController*>& OutPerceivers);

private:
    TArray<FAIPerceptionGridCell> Grid;
    float CellSize;
};
```

2. **Update Frequency Management**
```cpp
void UCustomAIPerceptionComponent::ManageUpdateFrequency()
{
    float DistanceToPlayer = GetDistanceToPlayer();
    
    // Adjust update frequency based on distance
    float NewUpdateInterval = FMath::Lerp(
        MinUpdateInterval,
        MaxUpdateInterval,
        FMath::Clamp(DistanceToPlayer / MaxPerceptionDistance, 0.0f, 1.0f)
    );
    
    SetUpdateInterval(NewUpdateInterval);
}
```

## References

- [Unreal Engine Perception System Documentation](https://docs.unrealengine.com/5.0/en-US/perception-system-in-unreal-engine/)
- [AI Perception Best Practices](https://docs.unrealengine.com/5.0/en-US/ai-perception-in-unreal-engine/) 