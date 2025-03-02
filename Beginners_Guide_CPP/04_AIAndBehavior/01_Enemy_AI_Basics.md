# Enemy AI Basics in UE5 C++

This guide covers implementing basic enemy AI systems for your hack and slash game using C++ in Unreal Engine 5.

## Table of Contents
1. [Basic AI Setup](#basic-ai-setup)
2. [AI Controller Implementation](#ai-controller-implementation)
3. [Perception System](#perception-system)
4. [Navigation System](#navigation-system)
5. [Basic Enemy Behaviors](#basic-enemy-behaviors)

## Basic AI Setup

### 1. Enemy Character Base Class
```cpp
// EnemyCharacter.h
UCLASS()
class MYGAME_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    class UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    class UPawnSensingComponent* PawnSensingComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float DetectionRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float LosePlayerRange;

    UFUNCTION()
    void OnPlayerSpotted(APawn* PlayerPawn);

    UFUNCTION()
    void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);
};

// EnemyCharacter.cpp
AEnemyCharacter::AEnemyCharacter()
{
    // Create and setup components
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

    // Set default values
    DetectionRange = 1000.0f;
    LosePlayerRange = 1500.0f;

    // Setup pawn sensing
    PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnPlayerSpotted);
    PawnSensingComponent->OnHearNoise.AddDynamic(this, &AEnemyCharacter::OnHearNoise);
    PawnSensingComponent->SightRadius = DetectionRange;
    PawnSensingComponent->SetPeripheralVisionAngle(60.0f);
}
```

### 2. Enemy State System
```cpp
// EnemyState.h
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Idle,
    Patrolling,
    Chasing,
    Attacking,
    Stunned,
    Dead
};

// EnemyStateComponent.h
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UEnemyStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetState(EEnemyState NewState);

    UFUNCTION(BlueprintPure, Category = "AI")
    EEnemyState GetCurrentState() const { return CurrentState; }

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
    EEnemyState CurrentState;

    UFUNCTION()
    void OnRep_CurrentState();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, EEnemyState, NewState, EEnemyState, OldState);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnStateChanged OnStateChanged;
};

// EnemyStateComponent.cpp
void UEnemyStateComponent::SetState(EEnemyState NewState)
{
    if (NewState != CurrentState)
    {
        EEnemyState OldState = CurrentState;
        CurrentState = NewState;
        OnStateChanged.Broadcast(NewState, OldState);
    }
}
```

## AI Controller Implementation

### 1. Basic AI Controller
```cpp
// EnemyAIController.h
UCLASS()
class MYGAME_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY()
    class AEnemyCharacter* EnemyCharacter;

    UPROPERTY()
    class ACharacter* PlayerCharacter;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float UpdateInterval;

    FTimerHandle DecisionTimerHandle;

    void MakeDecision();
    void UpdatePlayerLocation();
    bool IsPlayerInRange() const;
};

// EnemyAIController.cpp
void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    // Start decision making loop
    GetWorld()->GetTimerManager().SetTimer(
        DecisionTimerHandle,
        this,
        &AEnemyAIController::MakeDecision,
        UpdateInterval,
        true
    );
}

void AEnemyAIController::MakeDecision()
{
    if (!EnemyCharacter)
        return;

    UpdatePlayerLocation();

    if (UEnemyStateComponent* StateComp = EnemyCharacter->FindComponentByClass<UEnemyStateComponent>())
    {
        switch (StateComp->GetCurrentState())
        {
            case EEnemyState::Idle:
                HandleIdleState();
                break;
            case EEnemyState::Patrolling:
                HandlePatrolState();
                break;
            case EEnemyState::Chasing:
                HandleChaseState();
                break;
            case EEnemyState::Attacking:
                HandleAttackState();
                break;
            default:
                break;
        }
    }
}
```

## Perception System

### 1. Sight Configuration
```cpp
// EnemyPerceptionComponent.h
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UEnemyPerceptionComponent : public UAIPerceptionComponent
{
    GENERATED_BODY()

public:
    UEnemyPerceptionComponent();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    UAISenseConfig_Sight* SightConfig;

    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};

// EnemyPerceptionComponent.cpp
void UEnemyPerceptionComponent::BeginPlay()
{
    Super::BeginPlay();

    // Setup sight configuration
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    SightConfig->SightRadius = 1000.0f;
    SightConfig->LoseSightRadius = 1500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 60.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    // Add sight config to perception component
    ConfigureSense(*SightConfig);
}
```

## Navigation System

### 1. Path Following
```cpp
// EnemyNavigationComponent.h
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UEnemyNavigationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void MoveToLocation(const FVector& TargetLocation);

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void MoveToActor(AActor* TargetActor);

protected:
    UPROPERTY(EditAnywhere, Category = "Navigation")
    float AcceptanceRadius;

    UPROPERTY(EditAnywhere, Category = "Navigation")
    bool bStopOnOverlap;

    UFUNCTION()
    void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
};

// EnemyNavigationComponent.cpp
void UEnemyNavigationComponent::MoveToLocation(const FVector& TargetLocation)
{
    if (AAIController* AIController = Cast<AAIController>(GetOwner()->GetInstigatorController()))
    {
        AIController->MoveToLocation(
            TargetLocation,
            AcceptanceRadius,
            bStopOnOverlap,
            true,
            false,
            true,
            0,
            false
        );
    }
}
```

## Basic Enemy Behaviors

### 1. Patrol System
```cpp
// PatrolComponent.h
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UPatrolComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Patrol")
    void StartPatrol();

    UFUNCTION(BlueprintCallable, Category = "Patrol")
    void StopPatrol();

protected:
    UPROPERTY(EditAnywhere, Category = "Patrol")
    TArray<FVector> PatrolPoints;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    float WaitTimeAtPoint;

    int32 CurrentPatrolIndex;
    FTimerHandle PatrolTimer;

    void MoveToNextPoint();
    void OnReachedPatrolPoint();
};

// PatrolComponent.cpp
void UPatrolComponent::StartPatrol()
{
    if (PatrolPoints.Num() > 0)
    {
        CurrentPatrolIndex = 0;
        MoveToNextPoint();
    }
}

void UPatrolComponent::MoveToNextPoint()
{
    if (UEnemyNavigationComponent* NavComp = GetOwner()->FindComponentByClass<UEnemyNavigationComponent>())
    {
        NavComp->MoveToLocation(PatrolPoints[CurrentPatrolIndex]);
    }
}
```

### 2. Combat Behavior
```cpp
// EnemyCombatComponent.h
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UEnemyCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartCombat(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EndCombat();

protected:
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackCooldown;

    UPROPERTY(EditAnywhere, Category = "Combat")
    TArray<UAnimMontage*> AttackMontages;

    FTimerHandle AttackTimerHandle;
    AActor* CurrentTarget;

    void PerformAttack();
    bool IsInAttackRange() const;
};

// EnemyCombatComponent.cpp
void UEnemyCombatComponent::StartCombat(AActor* Target)
{
    if (!Target)
        return;

    CurrentTarget = Target;

    if (IsInAttackRange())
    {
        PerformAttack();
    }
    else if (UEnemyNavigationComponent* NavComp = GetOwner()->FindComponentByClass<UEnemyNavigationComponent>())
    {
        NavComp->MoveToActor(Target);
    }
}
```

## Best Practices and Tips

1. **Performance Optimization**
   - Use appropriate update intervals
   - Implement proper LOD for AI behavior
   - Optimize perception checks

2. **Code Organization**
   - Keep AI components modular
   - Use state machines for behavior
   - Implement proper interfaces

3. **Debugging**
   - Add visual debugging options
   - Log state changes and decisions
   - Create debug commands

4. **Extensibility**
   - Design for behavior variations
   - Use data tables for configurations
   - Implement proper interfaces

## Common Issues and Solutions

1. **Navigation Issues**
   - Ensure proper NavMesh coverage
   - Handle path finding failures
   - Implement path smoothing

2. **Perception Problems**
   - Configure proper sight ranges
   - Handle line of sight checks
   - Implement proper team handling

3. **Performance**
   - Optimize decision making
   - Implement proper tick intervals
   - Handle multiple AI agents

## Next Steps

1. Implement more complex behaviors
2. Add group coordination
3. Create different enemy types
4. Implement difficulty scaling
5. Add boss AI behaviors

Remember to thoroughly test your AI system in different scenarios and optimize its performance. The key is to create engaging and challenging enemies while maintaining good performance. 