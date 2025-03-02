#pragma once

#include "CoreMinimal.h"
#include "EnemyTemplateTypes.h"
#include "EnemyCreatorTypes.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
    Melee       UMETA(DisplayName = "Melee"),
    Ranged      UMETA(DisplayName = "Ranged"),
    Support     UMETA(DisplayName = "Support"),
    Elite       UMETA(DisplayName = "Elite"),
    Boss        UMETA(DisplayName = "Boss")
};

/** Configuration for enemy instances */
UCLASS(BlueprintType)
class ENEMYCREATOR_API UEnemyConfiguration : public UObject
{
    GENERATED_BODY()
public:
    /** Base template to use */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TSoftObjectPtr<UEnemyTemplate> BaseTemplate;
    
    /** Template modifications */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FEnemyTemplateModification Modifications;
    
    /** Initialize from template */
    void InitializeFromTemplate(UEnemyTemplate* Template);
    
    /** Apply configuration to an enemy instance */
    bool ApplyConfiguration(class ABaseEnemy* Enemy);
    
    /** Validate configuration */
    bool ValidateConfiguration(FEnemyTemplateValidationResult& OutResult) const;
};

/** Preview actor for enemy templates */
UCLASS(BlueprintType)
class ENEMYCREATOR_API AEnemyPreviewActor : public ACharacter
{
    GENERATED_BODY()
public:
    /** Set the behavior tree to use */
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void SetBehaviorTree(UBehaviorTree* NewBT);
    
    /** Get the current behavior tree */
    UFUNCTION(BlueprintCallable, Category = "Preview")
    UBehaviorTree* GetBehaviorTree() const;
    
protected:
    /** Current behavior tree */
    UPROPERTY()
    UBehaviorTree* CurrentBehaviorTree;
}; 