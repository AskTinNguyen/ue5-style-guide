# Pattern 3: Character Progression

## Problem Definition

Character progression systems in games need to handle:
- Multiple character attributes and stats that change over time
- Experience points, levels, and advancement mechanics
- Skill trees and ability unlocks
- Temporary and permanent stat modifications
- Persistence across game sessions
- Balanced progression curves
- UI representation of character growth

These systems often become complex and tightly coupled, making them difficult to maintain, extend, or rebalance.

## Solution Approach

Implement a flexible, data-driven character progression system that:
1. Separates progression data from gameplay logic
2. Uses a modular attribute system with modifiers
3. Provides a flexible skill/ability unlock framework
4. Handles persistence through serialization
5. Supports runtime balancing and tuning

## Implementation

### Attribute System

```cpp
// AttributeSet.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttributeSet.generated.h"

// Forward declarations
class UAttributeModifier;

// Delegate for attribute changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FName, AttributeName, float, OldValue, float, NewValue);

UCLASS(BlueprintType, Blueprintable)
class YOURGAME_API UAttributeSet : public UObject
{
    GENERATED_BODY()
    
public:
    UAttributeSet();
    
    // Initialize attributes from data table
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void InitializeFromDataTable(class UDataTable* DataTable, int32 Level = 1);
    
    // Attribute access
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    float GetAttributeValue(FName AttributeName) const;
    
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    float GetAttributeBaseValue(FName AttributeName) const;
    
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void SetAttributeBaseValue(FName AttributeName, float NewValue);
    
    // Modifier management
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void AddModifier(UAttributeModifier* Modifier);
    
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void RemoveModifier(UAttributeModifier* Modifier);
    
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void RemoveModifiersWithSource(const UObject* Source);
    
    // Attribute change delegate
    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnAttributeChanged;
    
    // Serialization
    virtual void Serialize(FArchive& Ar) override;
    
protected:
    // Base attribute values
    UPROPERTY()
    TMap<FName, float> BaseAttributes;
    
    // Modifiers applied to attributes
    UPROPERTY()
    TArray<UAttributeModifier*> Modifiers;
    
    // Calculate final attribute value with all modifiers applied
    float CalculateAttributeFinalValue(FName AttributeName) const;
    
    // Notify attribute change
    void NotifyAttributeChanged(FName AttributeName, float OldValue, float NewValue);
};
```

### Attribute Modifier

```cpp
// AttributeModifier.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttributeModifier.generated.h"

UENUM(BlueprintType)
enum class EModifierOperation : uint8
{
    Additive,       // Add to base value
    Multiplicative, // Multiply base value
    Override        // Replace base value
};

UCLASS(BlueprintType, Blueprintable)
class YOURGAME_API UAttributeModifier : public UObject
{
    GENERATED_BODY()
    
public:
    UAttributeModifier();
    
    // Initialize the modifier
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void Initialize(FName InAttributeName, float InValue, EModifierOperation InOperation, UObject* InSource = nullptr, float InDuration = -1.0f);
    
    // Update modifier (for time-based modifiers)
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    bool Update(float DeltaTime);
    
    // Check if modifier is expired
    UFUNCTION(BlueprintPure, Category = "Attributes")
    bool IsExpired() const;
    
    // Apply modifier to a value
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float ApplyModifier(float BaseValue) const;
    
    // Getters
    UFUNCTION(BlueprintPure, Category = "Attributes")
    FName GetAttributeName() const { return AttributeName; }
    
    UFUNCTION(BlueprintPure, Category = "Attributes")
    UObject* GetSource() const { return Source; }
    
    UFUNCTION(BlueprintPure, Category = "Attributes")
    int32 GetPriority() const { return Priority; }
    
protected:
    // The attribute this modifier affects
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    FName AttributeName;
    
    // The value of the modifier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float Value;
    
    // How the modifier is applied
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    EModifierOperation Operation;
    
    // Source of the modifier (item, ability, etc.)
    UPROPERTY()
    UObject* Source;
    
    // Duration of the modifier (-1 for permanent)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float Duration;
    
    // Current time remaining
    UPROPERTY()
    float TimeRemaining;
    
    // Priority for stacking/overriding
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    int32 Priority;
};
```

### Experience and Level System

```cpp
// ProgressionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressionComponent.generated.h"

class UAttributeSet;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32, OldLevel, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExperienceGained, float, Amount, float, NewTotal);

UCLASS(ClassGroup=(Character), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UProgressionComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UProgressionComponent();
    
    virtual void BeginPlay() override;
    
    // Experience and level management
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void GainExperience(float Amount);
    
    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool SetLevel(int32 NewLevel);
    
    UFUNCTION(BlueprintPure, Category = "Progression")
    int32 GetLevel() const { return CurrentLevel; }
    
    UFUNCTION(BlueprintPure, Category = "Progression")
    float GetExperience() const { return CurrentExperience; }
    
    UFUNCTION(BlueprintPure, Category = "Progression")
    float GetExperienceForNextLevel() const;
    
    UFUNCTION(BlueprintPure, Category = "Progression")
    float GetExperienceProgress() const;
    
    // Attribute access
    UFUNCTION(BlueprintPure, Category = "Progression")
    UAttributeSet* GetAttributeSet() const { return AttributeSet; }
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Progression")
    FOnLevelChanged OnLevelChanged;
    
    UPROPERTY(BlueprintAssignable, Category = "Progression")
    FOnExperienceGained OnExperienceGained;
    
    // Serialization
    virtual void Serialize(FArchive& Ar) override;
    
protected:
    // Configuration
    UPROPERTY(EditDefaultsOnly, Category = "Progression")
    UDataTable* AttributeDataTable;
    
    UPROPERTY(EditDefaultsOnly, Category = "Progression")
    UDataTable* ExperienceCurveTable;
    
    UPROPERTY(EditDefaultsOnly, Category = "Progression")
    int32 MaxLevel;
    
    // State
    UPROPERTY(ReplicatedUsing = OnRep_CurrentLevel)
    int32 CurrentLevel;
    
    UPROPERTY(ReplicatedUsing = OnRep_CurrentExperience)
    float CurrentExperience;
    
    UPROPERTY()
    UAttributeSet* AttributeSet;
    
    // Replication
    UFUNCTION()
    void OnRep_CurrentLevel(int32 OldLevel);
    
    UFUNCTION()
    void OnRep_CurrentExperience(float OldExperience);
    
    // Level up logic
    void LevelUp();
    
    // Get experience required for a specific level
    float GetExperienceRequiredForLevel(int32 Level) const;
    
    // Network
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

### Skill System

```cpp
// SkillTreeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTreeComponent.generated.h"

class USkillData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillUnlocked, USkillData*, Skill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillPointsChanged, int32, NewPoints);

UCLASS(ClassGroup=(Character), meta=(BlueprintSpawnableComponent))
class YOURGAME_API USkillTreeComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    USkillTreeComponent();
    
    virtual void BeginPlay() override;
    
    // Skill point management
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void AddSkillPoints(int32 Points);
    
    UFUNCTION(BlueprintPure, Category = "Skills")
    int32 GetSkillPoints() const { return SkillPoints; }
    
    // Skill unlocking
    UFUNCTION(BlueprintCallable, Category = "Skills")
    bool UnlockSkill(USkillData* Skill);
    
    UFUNCTION(BlueprintPure, Category = "Skills")
    bool CanUnlockSkill(USkillData* Skill) const;
    
    UFUNCTION(BlueprintPure, Category = "Skills")
    bool IsSkillUnlocked(USkillData* Skill) const;
    
    UFUNCTION(BlueprintPure, Category = "Skills")
    TArray<USkillData*> GetUnlockedSkills() const;
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Skills")
    FOnSkillUnlocked OnSkillUnlocked;
    
    UPROPERTY(BlueprintAssignable, Category = "Skills")
    FOnSkillPointsChanged OnSkillPointsChanged;
    
    // Serialization
    virtual void Serialize(FArchive& Ar) override;
    
protected:
    // Configuration
    UPROPERTY(EditDefaultsOnly, Category = "Skills")
    TArray<USkillData*> AvailableSkills;
    
    // State
    UPROPERTY(ReplicatedUsing = OnRep_SkillPoints)
    int32 SkillPoints;
    
    UPROPERTY(ReplicatedUsing = OnRep_UnlockedSkills)
    TArray<USkillData*> UnlockedSkillsList;
    
    // Replication
    UFUNCTION()
    void OnRep_SkillPoints(int32 OldPoints);
    
    UFUNCTION()
    void OnRep_UnlockedSkills();
    
    // Network
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

### Skill Data

```cpp
// SkillData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillData.generated.h"

UCLASS(BlueprintType)
class YOURGAME_API USkillData : public UPrimaryDataAsset
{
    GENERATED_BODY()
    
public:
    USkillData();
    
    // Basic skill info
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    FText SkillName;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    FText Description;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    UTexture2D* Icon;
    
    // Skill requirements
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Requirements")
    int32 SkillPointCost;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Requirements")
    int32 LevelRequirement;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Requirements")
    TArray<USkillData*> PrerequisiteSkills;
    
    // Skill effects
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<TSubclassOf<class UGameplayEffect>> GrantedEffects;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<TSubclassOf<class UGameplayAbility>> GrantedAbilities;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TMap<FName, float> AttributeModifiers;
    
    // UI positioning
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    FVector2D UIPosition;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TArray<USkillData*> ConnectedSkills;
};
```

## Data-Driven Configuration

### Attribute Data Table

```cpp
// AttributeData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AttributeData.generated.h"

USTRUCT(BlueprintType)
struct YOURGAME_API FAttributeData : public FTableRowBase
{
    GENERATED_BODY()
    
    // Base value at level 1
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float BaseValue;
    
    // Value increase per level
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float ValuePerLevel;
    
    // Growth curve (linear, exponential, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    UCurveFloat* GrowthCurve;
    
    // Minimum and maximum values
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float MinValue;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float MaxValue;
    
    FAttributeData()
        : BaseValue(0.0f)
        , ValuePerLevel(0.0f)
        , GrowthCurve(nullptr)
        , MinValue(0.0f)
        , MaxValue(100.0f)
    {
    }
};
```

### Experience Curve Data

```cpp
// ExperienceData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ExperienceData.generated.h"

USTRUCT(BlueprintType)
struct YOURGAME_API FExperienceData : public FTableRowBase
{
    GENERATED_BODY()
    
    // Level number
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Experience")
    int32 Level;
    
    // Experience required to reach this level
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Experience")
    float ExperienceRequired;
    
    // Skill points awarded at this level
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Experience")
    int32 SkillPointsAwarded;
    
    FExperienceData()
        : Level(1)
        , ExperienceRequired(0.0f)
        , SkillPointsAwarded(0)
    {
    }
};
```

## Integration Tips

1. **Component Setup**:
   - Add ProgressionComponent and SkillTreeComponent to your character class
   - Initialize the AttributeSet with appropriate data tables

2. **Persistence**:
   - Implement Save/Load functionality using the Serialize methods
   - Consider using the Unreal SaveGame system for easy persistence

3. **UI Integration**:
   - Create UI widgets that bind to the component delegates
   - Display attribute values, experience bars, and skill trees

4. **Balancing**:
   - Use data tables for easy tuning of progression curves
   - Implement analytics to track player progression

## Example Usage Scenarios

### Character Setup with Progression

```cpp
// RPGCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGCharacter.generated.h"

class UProgressionComponent;
class USkillTreeComponent;

UCLASS()
class YOURGAME_API ARPGCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    ARPGCharacter();
    
    // Progression access
    UFUNCTION(BlueprintPure, Category = "Progression")
    UProgressionComponent* GetProgressionComponent() const { return ProgressionComponent; }
    
    UFUNCTION(BlueprintPure, Category = "Progression")
    USkillTreeComponent* GetSkillTreeComponent() const { return SkillTreeComponent; }
    
    // Attribute convenience functions
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetAttributeValue(FName AttributeName) const;
    
protected:
    virtual void BeginPlay() override;
    
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProgressionComponent* ProgressionComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkillTreeComponent* SkillTreeComponent;
    
    // Handle level up
    UFUNCTION()
    void OnLevelChanged(int32 OldLevel, int32 NewLevel);
    
    // Handle skill unlock
    UFUNCTION()
    void OnSkillUnlocked(USkillData* Skill);
};

// RPGCharacter.cpp
ARPGCharacter::ARPGCharacter()
{
    // Create components
    ProgressionComponent = CreateDefaultSubobject<UProgressionComponent>(TEXT("ProgressionComponent"));
    SkillTreeComponent = CreateDefaultSubobject<USkillTreeComponent>(TEXT("SkillTreeComponent"));
}

void ARPGCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Bind to delegates
    if (ProgressionComponent)
    {
        ProgressionComponent->OnLevelChanged.AddDynamic(this, &ARPGCharacter::OnLevelChanged);
    }
    
    if (SkillTreeComponent)
    {
        SkillTreeComponent->OnSkillUnlocked.AddDynamic(this, &ARPGCharacter::OnSkillUnlocked);
    }
}

float ARPGCharacter::GetAttributeValue(FName AttributeName) const
{
    if (ProgressionComponent && ProgressionComponent->GetAttributeSet())
    {
        return ProgressionComponent->GetAttributeSet()->GetAttributeValue(AttributeName);
    }
    return 0.0f;
}

void ARPGCharacter::OnLevelChanged(int32 OldLevel, int32 NewLevel)
{
    // Award skill points on level up
    if (SkillTreeComponent && NewLevel > OldLevel)
    {
        int32 PointsToAward = NewLevel - OldLevel;
        SkillTreeComponent->AddSkillPoints(PointsToAward);
    }
}

void ARPGCharacter::OnSkillUnlocked(USkillData* Skill)
{
    // Apply skill effects
    // ...
}
```

### Applying Temporary Buffs

```cpp
// BuffComponent.cpp
void UBuffComponent::ApplyTemporaryBuff(FName AttributeName, float Value, float Duration)
{
    // Get the attribute set
    UAttributeSet* AttributeSet = GetOwner()->FindComponentByClass<UProgressionComponent>()->GetAttributeSet();
    if (!AttributeSet)
    {
        return;
    }
    
    // Create a temporary modifier
    UAttributeModifier* Modifier = NewObject<UAttributeModifier>(this);
    Modifier->Initialize(AttributeName, Value, EModifierOperation::Additive, this, Duration);
    
    // Add the modifier to the attribute set
    AttributeSet->AddModifier(Modifier);
    
    // Store the modifier for cleanup
    ActiveModifiers.Add(Modifier);
}
```

## AI Prompt Templates

### Creating a Character Progression System

```
Create a character progression system for [GAME TYPE] with the following requirements:
- [NUMBER] primary attributes that affect [GAMEPLAY ELEMENTS]
- A leveling system with [PROGRESSION CURVE TYPE]
- A skill tree with [STRUCTURE DESCRIPTION]
- Temporary and permanent buffs/debuffs
- Persistence across game sessions

Include data structures, component implementations, and UI integration examples.
```

### Balancing a Progression System

```
Help me balance my character progression system for [GAME TYPE].
I need to create a progression curve that:
- Takes [TIME PERIOD] to reach max level
- Provides [REWARD FREQUENCY] to maintain player engagement
- Scales difficulty appropriately for [PLAYER SKILL LEVEL]
- Prevents power spikes at specific levels

Here's my current implementation: [CODE/DATA]
```

## Performance Considerations

1. **Attribute Calculations**:
   - Cache calculated attribute values where appropriate
   - Recalculate only when modifiers change
   - Consider using a dirty flag system for updates

2. **Serialization**:
   - Be mindful of save file size when persisting progression data
   - Consider delta compression for frequent saves

3. **UI Updates**:
   - Use efficient UI binding patterns
   - Update UI elements only when relevant data changes

## Common Pitfalls

1. **Progression Balance**:
   - Avoid exponential scaling that leads to number inflation
   - Test progression curves with different player skill levels
   - Ensure meaningful choices in skill trees

2. **Modifier Management**:
   - Properly handle expired modifiers to prevent memory leaks
   - Define clear stacking rules for similar modifiers
   - Consider the order of operations for different modifier types

3. **Save Data Compatibility**:
   - Design serialization to handle version changes
   - Include fallbacks for missing or changed attributes
   - Test save/load across game updates 