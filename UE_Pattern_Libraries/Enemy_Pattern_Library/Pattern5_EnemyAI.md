# Pattern 5: Enemy AI Integration

## Problem Statement
Creating a flexible and performant AI system that seamlessly integrates C++ functionality with Behavior Trees while providing reusable building blocks for enemy behaviors.

## Solution Overview
A comprehensive AI framework that combines modular Behavior Tree tasks, services, and decorators with C++ systems, allowing for complex enemy behaviors while maintaining performance.

## Core Components

### 1. AI Controller Base

```cpp
// Base AI controller for enemies
UCLASS()
class GAME_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()
public:
    AEnemyAIController();
    
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    
    // Behavior interface
    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual void InitializeBehavior();
    
    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual void UpdateBehaviorState();
    
protected:
    // Behavior tree component
    UPROPERTY()
    UBehaviorTreeComponent* BehaviorTreeComponent;
    
    // Blackboard component
    UPROPERTY()
    UBlackboardComponent* BlackboardComponent;
    
    // Perception component
    UPROPERTY()
    UAIPerceptionComponent* PerceptionComponent;
    
    // Helper functions
    virtual void SetupPerceptionSystem();
    virtual void ProcessPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
};
```

### 2. Behavior Tree Tasks

```cpp
// Base task for enemy actions
UCLASS()
class GAME_API UEnemyBTTaskBase : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UEnemyBTTaskBase();
    
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
    
protected:
    // Helper functions
    ABaseEnemy* GetEnemy(UBehaviorTreeComponent& OwnerComp) const;
    UEnemyAbilityComponent* GetAbilityComponent(UBehaviorTreeComponent& OwnerComp) const;
};

// Example combat task
UCLASS()
class GAME_API UBTTask_ActivateAbility : public UEnemyBTTaskBase
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Ability")
    FName AbilityName;
    
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    
protected:
    virtual bool ValidateAbility(UEnemyAbilityComponent* AbilityComp) const;
};
```

### 3. AI Services

```cpp
// Base service for enemy behavior updates
UCLASS()
class GAME_API UEnemyBTServiceBase : public UBTService
{
    GENERATED_BODY()
public:
    UEnemyBTServiceBase();
    
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    
protected:
    // Update frequency control
    UPROPERTY(EditAnywhere, Category = "Service")
    float UpdateInterval;
    
    // Helper functions
    virtual void UpdateState(UBehaviorTreeComponent& OwnerComp);
};

// Example target tracking service
UCLASS()
class GAME_API UBTService_UpdateTarget : public UEnemyBTServiceBase
{
    GENERATED_BODY()
protected:
    virtual void UpdateState(UBehaviorTreeComponent& OwnerComp) override;
    
    UPROPERTY(EditAnywhere, Category = "Targeting")
    FName TargetKey;
    
    UPROPERTY(EditAnywhere, Category = "Targeting")
    float MaxTargetAge;
};
```

## Usage Examples

### 1. Implementing AI Controller

```cpp
// Example of AI controller implementation
void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    // Setup perception
    SetupPerceptionSystem();
    
    // Initialize behavior
    if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(InPawn))
    {
        if (UBehaviorTree* BehaviorTree = Enemy->GetBehaviorTree())
        {
            // Initialize blackboard
            if (UBlackboardComponent* Blackboard = GetBlackboardComponent())
            {
                Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
                
                // Set initial values
                Blackboard->SetValueAsVector("HomeLocation", Enemy->GetActorLocation());
                Blackboard->SetValueAsFloat("AggressionLevel", Enemy->GetAggressionLevel());
            }
            
            // Run behavior tree
            RunBehaviorTree(BehaviorTree);
        }
    }
}
```

### 2. Creating Custom Tasks

```cpp
// Example of custom task implementation
EBTNodeResult::Type UBTTask_ActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (UEnemyAbilityComponent* AbilityComp = GetAbilityComponent(OwnerComp))
    {
        // Validate ability
        if (!ValidateAbility(AbilityComp))
        {
            return EBTNodeResult::Failed;
        }
        
        // Activate ability
        if (AbilityComp->ActivateAbility(AbilityName))
        {
            // Wait for completion if needed
            if (AbilityComp->IsAbilityInstant(AbilityName))
            {
                return EBTNodeResult::Succeeded;
            }
            else
            {
                WaitForAbilityEnd(OwnerComp, NodeMemory);
                return EBTNodeResult::InProgress;
            }
        }
    }
    
    return EBTNodeResult::Failed;
}
```

### 3. Implementing Services

```cpp
// Example of service implementation
void UBTService_UpdateTarget::UpdateState(UBehaviorTreeComponent& OwnerComp)
{
    if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
    {
        // Get current target
        AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey));
        
        // Validate target
        if (!IsValidTarget(CurrentTarget))
        {
            // Find new target
            if (AActor* NewTarget = FindBestTarget(OwnerComp))
            {
                Blackboard->SetValueAsObject(TargetKey, NewTarget);
                Blackboard->SetValueAsFloat("LastTargetUpdateTime", GetWorld()->GetTimeSeconds());
            }
        }
        
        // Update target information
        if (AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey)))
        {
            Blackboard->SetValueAsVector("TargetLocation", Target->GetActorLocation());
            Blackboard->SetValueAsFloat("DistanceToTarget", 
                FVector::Distance(GetEnemy(OwnerComp)->GetActorLocation(), Target->GetActorLocation()));
        }
    }
}
```

## Best Practices

1. **Performance Optimization**
   - Use appropriate update intervals
   - Implement efficient perception updates
   - Optimize behavior tree structure

2. **Behavior Design**
   - Create modular, reusable tasks
   - Implement clear state transitions
   - Use appropriate decorators

3. **Debug Support**
   - Add visual debugging
   - Implement logging
   - Create debug views

## Integration Guidelines

1. **With Combat System**
   ```cpp
   // Example of combat system integration
   void AEnemyAIController::OnCombatStateChanged(EEnemyCombatState NewState)
   {
       if (UBlackboardComponent* Blackboard = GetBlackboardComponent())
       {
           // Update combat state
           Blackboard->SetValueAsEnum("CombatState", (uint8)NewState);
           
           // Update behavior flags
           switch (NewState)
           {
               case EEnemyCombatState::Attacking:
                   Blackboard->SetValueAsBool("IsAggressive", true);
                   break;
                   
               case EEnemyCombatState::Retreating:
                   Blackboard->SetValueAsBool("IsRetreating", true);
                   break;
           }
       }
   }
   ```

2. **With Animation System**
   ```cpp
   // Example of animation integration
   void UBTTask_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
   {
       if (ABaseEnemy* Enemy = GetEnemy(OwnerComp))
       {
           if (UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance())
           {
               // Play animation
               float Duration = AnimInstance->Montage_Play(ActionMontage);
               
               // Setup completion callback
               FOnMontageEnded EndDelegate;
               EndDelegate.BindUObject(this, &UBTTask_PlayMontage::OnMontageEnded);
               AnimInstance->Montage_SetEndDelegate(EndDelegate, ActionMontage);
               
               return EBTNodeResult::InProgress;
           }
       }
       
       return EBTNodeResult::Failed;
   }
   ```

## Common Pitfalls

1. **Performance Issues**
   - Too frequent updates
   - Expensive calculations in services
   - Inefficient perception checks

2. **Behavior Problems**
   - Unclear state transitions
   - Missing error handling
   - Poor target selection

3. **Integration Issues**
   - Tight coupling with systems
   - Missing synchronization
   - Poor error handling

## Testing Strategy

1. **Behavior Testing**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyAITest, "Enemy.AI.Behavior",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyAITest::RunTest(const FString& Parameters)
   {
       // Test AI initialization
       AEnemyAIController* AIController = CreateTestController();
       TestNotNull("AI Controller should be created", AIController);
       
       // Test behavior tree execution
       TestTrue("Behavior tree should start", AIController->RunBehaviorTree(TestBehaviorTree));
       
       // Test state updates
       UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
       TestEqual("Combat state should be updated",
           Blackboard->GetValueAsEnum("CombatState"),
           (uint8)EEnemyCombatState::Idle);
       
       return true;
   }
   ```

2. **Performance Testing**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyAIPerformanceTest, "Enemy.AI.Performance",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyAIPerformanceTest::RunTest(const FString& Parameters)
   {
       // Setup test environment
       TArray<ABaseEnemy*> TestEnemies = SpawnTestEnemies(100);
       
       // Measure update time
       double StartTime = FPlatformTime::Seconds();
       
       // Run AI updates
       for (ABaseEnemy* Enemy : TestEnemies)
       {
           if (AEnemyAIController* AIController = Cast<AEnemyAIController>(Enemy->GetController()))
           {
               AIController->UpdateBehaviorState();
           }
       }
       
       double EndTime = FPlatformTime::Seconds();
       double UpdateTime = EndTime - StartTime;
       
       // Verify performance
       TestTrue("Update time should be within limits", UpdateTime < 0.016f);
       
       return true;
   }
   ```

## Debugging Tools

1. **Visual Debugging**
   ```cpp
   void AEnemyAIController::DrawDebugInfo()
   {
       if (CVarShowAIDebug.GetValueOnGameThread())
       {
           if (UBlackboardComponent* Blackboard = GetBlackboardComponent())
           {
               // Draw target information
               if (AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject("CurrentTarget")))
               {
                   DrawDebugLine(GetWorld(),
                       GetPawn()->GetActorLocation(),
                       Target->GetActorLocation(),
                       FColor::Red,
                       false,
                       -1.0f,
                       0,
                       2.0f);
               }
               
               // Draw state information
               FString StateInfo = FString::Printf(TEXT("State: %s\nAggression: %f"),
                   *UEnum::GetValueAsString(TEXT("EEnemyCombatState"), 
                       Blackboard->GetValueAsEnum("CombatState")),
                   Blackboard->GetValueAsFloat("AggressionLevel"));
               
               DrawDebugString(GetWorld(),
                   GetPawn()->GetActorLocation() + FVector(0, 0, 100),
                   StateInfo,
                   nullptr,
                   FColor::White,
                   0.0f);
           }
       }
   }
   ```

2. **Behavior Tree Debugging**
   ```cpp
   void UEnemyBTTaskBase::LogTaskExecution(UBehaviorTreeComponent& OwnerComp)
   {
       if (CVarLogAITasks.GetValueOnGameThread())
       {
           if (ABaseEnemy* Enemy = GetEnemy(OwnerComp))
           {
               UE_LOG(LogEnemyAI, Log, TEXT("[%s] Executing task: %s"),
                   *Enemy->GetName(),
                   *GetNodeName());
               
               if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
               {
                   UE_LOG(LogEnemyAI, Log, TEXT("  Combat State: %s"),
                       *UEnum::GetValueAsString(TEXT("EEnemyCombatState"),
                           Blackboard->GetValueAsEnum("CombatState")));
                   
                   UE_LOG(LogEnemyAI, Log, TEXT("  Target: %s"),
                       *GetNameSafe(Cast<AActor>(Blackboard->GetValueAsObject("CurrentTarget"))));
               }
           }
       }
   }
   ```

## Performance Considerations

1. **Update Optimization**
   ```cpp
   void UEnemyBTServiceBase::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
   {
       FBTAuxiliaryMemory* AuxMemory = GetNodeMemory<FBTAuxiliaryMemory>(NodeMemory);
       
       // Check update interval
       if (GetWorld()->GetTimeSeconds() - AuxMemory->LastUpdateTime < UpdateInterval)
       {
           return;
       }
       
       // Perform update
       UpdateState(OwnerComp);
       
       // Update timestamp
       AuxMemory->LastUpdateTime = GetWorld()->GetTimeSeconds();
   }
   ```

2. **Perception Optimization**
   ```cpp
   void AEnemyAIController::SetupPerceptionSystem()
   {
       // Configure sight sense
       if (UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config")))
       {
           SightConfig->SightRadius = 1500.0f;
           SightConfig->LoseSightRadius = 2000.0f;
           SightConfig->PeripheralVisionAngleDegrees = 90.0f;
           SightConfig->DetectionByAffiliation.bDetectEnemies = true;
           SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
           SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
           
           GetPerceptionComponent()->ConfigureSense(*SightConfig);
       }
       
       // Configure hearing sense
       if (UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config")))
       {
           HearingConfig->HearingRange = 1000.0f;
           HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
           HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
           HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
           
           GetPerceptionComponent()->ConfigureSense(*HearingConfig);
       }
   }
   ```

## Credits

This pattern is part of the Enemy Pattern Library, designed to provide a robust foundation for enemy AI systems in Unreal Engine 5. 