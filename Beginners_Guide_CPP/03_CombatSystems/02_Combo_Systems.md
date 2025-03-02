# Combo Systems in UE5 C++

This guide will teach you how to implement a robust combo system for your hack and slash game using C++ in Unreal Engine 5.

## Table of Contents
1. [Understanding Combo Mechanics](#understanding-combo-mechanics)
2. [Setting Up the Combo System](#setting-up-the-combo-system)
3. [Implementing Combo Logic](#implementing-combo-logic)
4. [Adding Visual Feedback](#adding-visual-feedback)
5. [Advanced Combo Features](#advanced-combo-features)

## Understanding Combo Mechanics

A combo system typically consists of:
- Sequential attack inputs
- Timing windows for combo continuation
- Different animations for each combo stage
- Hit detection and damage calculation
- Visual and audio feedback

### Key Components
- Combo Counter
- Input Buffer System
- Animation Montages
- Hit Detection
- Damage Multipliers

## Setting Up the Combo System

### 1. Header File Setup (MyCharacter.h)
```cpp
// Add to your character header file
protected:
    // Combo Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 ComboCounter;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ComboWindowTime;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TArray<UAnimMontage*> ComboMontages;
    
    // Combo State
    bool bIsInComboWindow;
    FTimerHandle ComboTimerHandle;
    
    // Functions
    void StartComboWindow();
    void EndComboWindow();
    void ProcessComboAttack();
```

### 2. Implementation (MyCharacter.cpp)
```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    ComboCounter = 0;
    ComboWindowTime = 1.5f;
    bIsInComboWindow = false;
}

void AMyCharacter::ProcessComboAttack()
{
    if (!bIsInComboWindow)
    {
        ComboCounter = 0;
    }
    
    if (ComboCounter < ComboMontages.Num())
    {
        if (UAnimMontage* CurrentMontage = ComboMontages[ComboCounter])
        {
            PlayAnimMontage(CurrentMontage);
            ComboCounter++;
            StartComboWindow();
        }
    }
}

void AMyCharacter::StartComboWindow()
{
    bIsInComboWindow = true;
    GetWorldTimerManager().SetTimer(
        ComboTimerHandle,
        this,
        &AMyCharacter::EndComboWindow,
        ComboWindowTime,
        false
    );
}

void AMyCharacter::EndComboWindow()
{
    bIsInComboWindow = false;
    ComboCounter = 0;
}
```

## Implementing Combo Logic

### 1. Input Buffer System
```cpp
// Add to MyCharacter.h
protected:
    TArray<float> InputBuffer;
    float InputBufferTimeThreshold;
    
    void ProcessInputBuffer();
    void ClearInputBuffer();

// Implementation in MyCharacter.cpp
void AMyCharacter::ProcessInputBuffer()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    // Remove old inputs
    InputBuffer.RemoveAll([=](float TimeStamp) {
        return CurrentTime - TimeStamp > InputBufferTimeThreshold;
    });
    
    // Process valid inputs
    if (InputBuffer.Num() > 0)
    {
        ProcessComboAttack();
    }
}
```

### 2. Animation Notifiers
```cpp
// Create a new class MyComboAnimNotify.h
UCLASS()
class MYGAME_API UMyComboAnimNotify : public UAnimNotify
{
    GENERATED_BODY()
    
public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    
    UPROPERTY(EditAnywhere)
    float DamageMultiplier;
};

// MyComboAnimNotify.cpp
void UMyComboAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (AMyCharacter* Character = Cast<AMyCharacter>(MeshComp->GetOwner()))
    {
        // Apply combo damage
        float FinalDamage = Character->GetBaseDamage() * DamageMultiplier;
        Character->ApplyComboDamage(FinalDamage);
    }
}
```

## Adding Visual Feedback

### 1. VFX System Integration
```cpp
// Add to MyCharacter.h
protected:
    UPROPERTY(EditAnywhere, Category = "Combat|VFX")
    TArray<UNiagaraSystem*> ComboVFX;
    
    UPROPERTY(EditAnywhere, Category = "Combat|VFX")
    TArray<USoundBase*> ComboSounds;
    
    void PlayComboVFX(int32 ComboIndex);

// Implementation in MyCharacter.cpp
void AMyCharacter::PlayComboVFX(int32 ComboIndex)
{
    if (ComboVFX.IsValidIndex(ComboIndex))
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ComboVFX[ComboIndex],
            GetActorLocation(),
            GetActorRotation()
        );
    }
    
    if (ComboSounds.IsValidIndex(ComboIndex))
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ComboSounds[ComboIndex],
            GetActorLocation()
        );
    }
}
```

## Advanced Combo Features

### 1. Branching Combos
```cpp
// Add to MyCharacter.h
struct FComboNode
{
    UAnimMontage* Montage;
    TArray<int32> NextValidCombos;
    float DamageMultiplier;
};

UPROPERTY(EditAnywhere, Category = "Combat")
TArray<FComboNode> ComboTree;

// Implementation example
void AMyCharacter::ProcessBranchingCombo(int32 InputID)
{
    if (ComboTree.IsValidIndex(CurrentComboIndex))
    {
        const FComboNode& CurrentNode = ComboTree[CurrentComboIndex];
        
        if (CurrentNode.NextValidCombos.Contains(InputID))
        {
            PlayAnimMontage(ComboTree[InputID].Montage);
            CurrentComboIndex = InputID;
            StartComboWindow();
        }
    }
}
```

### 2. Combo Momentum System
```cpp
// Add to MyCharacter.h
protected:
    float ComboMomentum;
    float MomentumDecayRate;
    float MaxMomentum;
    
    void UpdateComboMomentum();
    float CalculateMomentumBonus();

// Implementation
void AMyCharacter::UpdateComboMomentum()
{
    ComboMomentum = FMath::Clamp(
        ComboMomentum + (bIsInComboWindow ? 0.1f : -MomentumDecayRate),
        0.0f,
        MaxMomentum
    );
}

float AMyCharacter::CalculateMomentumBonus()
{
    return 1.0f + (ComboMomentum / MaxMomentum);
}
```

## Best Practices and Tips

1. **Performance Optimization**
   - Use object pooling for VFX
   - Optimize animation blueprint logic
   - Cache commonly used components

2. **Code Organization**
   - Keep combo logic in a separate component
   - Use interfaces for damage handling
   - Implement proper error handling

3. **Debugging**
   - Add visual debugging options
   - Log combo states and transitions
   - Create debug commands for testing

4. **Extensibility**
   - Design the system to be easily extended
   - Use data tables for combo configurations
   - Implement proper interfaces

## Common Issues and Solutions

1. **Timing Issues**
   - Use animation notifies for precise timing
   - Implement input buffering
   - Add animation blending

2. **Network Considerations**
   - Implement proper replication
   - Handle latency in combo timing
   - Validate combo sequences server-side

3. **Animation Transitions**
   - Use animation blending for smooth transitions
   - Handle interrupted combos gracefully
   - Implement proper state management

## Next Steps

1. Experiment with different combo patterns
2. Add particle effects and sound
3. Implement hit stop effects
4. Add camera shake and environmental reactions
5. Create combo challenges and tutorials

Remember to profile your combo system and optimize where necessary. The key to a good combo system is finding the right balance between responsiveness and visual appeal. 