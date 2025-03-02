// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EnemyTemplateTypes.generated.h"

/** Base stats for enemy types */
USTRUCT(BlueprintType)
struct ENEMYCREATOR_API FEnemyBaseStats
{
    GENERATED_BODY()
    
    /** Base health value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Health = 100.0f;
    
    /** Base damage value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Damage = 20.0f;
    
    /** Movement speed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Speed = 300.0f;
    
    /** Attack speed multiplier */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackSpeed = 1.0f;
    
    /** Defense value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Defense = 10.0f;
    
    /** Critical hit chance (0-1) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CriticalChance = 0.05f;
    
    /** Critical hit multiplier */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "1.0"))
    float CriticalMultiplier = 2.0f;
};

/** Stat scaling configuration */
USTRUCT(BlueprintType)
struct ENEMYCREATOR_API FEnemyStatScaling
{
    GENERATED_BODY()
    
    /** Level-based stat multipliers */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
    TMap<FName, UCurveFloat*> StatScalingCurves;
    
    /** Difficulty-based stat multipliers */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
    TMap<FName, float> DifficultyMultipliers;
    
    /** Elite/Champion stat bonuses */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
    TMap<FName, float> EliteMultipliers;
};

/** Enemy ability definition */
USTRUCT(BlueprintType)
struct ENEMYCREATOR_API FEnemyAbilityDefinition
{
    GENERATED_BODY()
    
    /** Unique name for this ability */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FName AbilityName;
    
    /** Display name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FText DisplayName;
    
    /** Ability description */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FText Description;
    
    /** Ability tags */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FGameplayTagContainer AbilityTags;
    
    /** Cooldown time in seconds */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float CooldownTime = 0.0f;
    
    /** Range of the ability */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Range = 100.0f;
    
    /** Cost to use the ability */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Cost = 0.0f;
    
    /** Whether this ability is passive */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    bool bIsPassive = false;
    
    /** Animation montage for this ability */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    TSoftObjectPtr<UAnimMontage> AbilityMontage;
    
    /** Effects to apply */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    TArray<TSoftClassPtr<UGameplayEffect>> AbilityEffects;
};

/** AI behavior configuration */
USTRUCT(BlueprintType)
struct ENEMYCREATOR_API FEnemyAIConfig
{
    GENERATED_BODY()
    
    /** Behavior tree asset */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BehaviorTree;
    
    /** Blackboard asset */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TSoftObjectPtr<UBlackboardData> Blackboard;
    
    /** Aggression level (0-1) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AggressionLevel = 0.5f;
    
    /** Preferred engagement range */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float PreferredRange = 300.0f;
    
    /** Whether to use cover */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool bUseCover = false;
    
    /** Whether to coordinate with allies */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool bCoordinateWithAllies = false;
    
    /** Behavior tree parameters */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TMap<FName, float> BehaviorParameters;
    
    /** AI personality tags */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FGameplayTagContainer PersonalityTags;
};

/** Visual customization options */
USTRUCT(BlueprintType)
struct ENEMYCREATOR_API FEnemyVisualCustomization
{
    GENERATED_BODY()
    
    /** Base skeletal mesh */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
    
    /** Animation blueprint */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TSoftObjectPtr<UAnimBlueprint> AnimationBlueprint;
    
    /** Scale multiplier */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FVector Scale = FVector(1.0f);
    
    /** Color tint */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FLinearColor ColorTint = FLinearColor::White;
    
    /** Material parameter overrides */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TMap<FName, float> ScalarParameters;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TMap<FName, FLinearColor> VectorParameters;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TMap<FName, TSoftObjectPtr<UTexture>> TextureParameters;
    
    /** Material slot overrides */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TMap<int32, TSoftObjectPtr<UMaterialInterface>> MaterialOverrides;
};

/** Template modification for creating variants */
USTRUCT(BlueprintType)
struct ENEMYCREATOR_API FEnemyTemplateModification
{
    GENERATED_BODY()
    
    /** Stat multipliers */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification")
    TMap<FName, float> StatMultipliers;
    
    /** Ability modifications */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification")
    TMap<FName, FEnemyAbilityDefinition> ModifiedAbilities;
    
    /** AI behavior modifications */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification")
    FEnemyAIConfig AIModifications;
    
    /** Visual modifications */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification")
    FEnemyVisualCustomization VisualModifications;
    
    /** Additional tags */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification")
    FGameplayTagContainer AdditionalTags;
};

/** Template validation result */
USTRUCT(BlueprintType)
struct ENEMYCREATOR_API FEnemyTemplateValidationResult
{
    GENERATED_BODY()
    
    /** Whether validation passed */
    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    bool bIsValid = false;
    
    /** List of validation errors */
    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    TArray<FText> ValidationErrors;
    
    /** List of validation warnings */
    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    TArray<FText> ValidationWarnings;
    
    /** Add an error message */
    void AddError(const FText& Error) { ValidationErrors.Add(Error); bIsValid = false; }
    
    /** Add a warning message */
    void AddWarning(const FText& Warning) { ValidationWarnings.Add(Warning); }
    
    /** Clear all results */
    void Clear() { bIsValid = true; ValidationErrors.Empty(); ValidationWarnings.Empty(); }
}; 