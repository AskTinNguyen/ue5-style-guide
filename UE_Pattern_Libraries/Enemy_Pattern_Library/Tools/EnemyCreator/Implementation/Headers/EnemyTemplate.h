// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "EnemyTemplateTypes.h"
#include "EnemyTemplate.generated.h"

/**
 * Data asset for defining enemy templates
 * Supports inheritance, configuration, and validation
 */
UCLASS(BlueprintType)
class ENEMYCREATOR_API UEnemyTemplate : public UDataAsset
{
    GENERATED_BODY()

public:
    UEnemyTemplate();
    
    //~ Begin UObject Interface
    virtual void PostLoad() override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    //~ End UObject Interface
    
    //~ Begin Template Interface
    /** Validate this template and all its dependencies */
    virtual bool ValidateTemplate(FString& OutError);
    
    /** Apply this template to an enemy instance */
    virtual void ApplyToEnemy(class ABaseEnemy* Enemy);
    
    /** Create a new template inheriting from this one */
    virtual UEnemyTemplate* CreateChildTemplate(const FString& ChildName);
    
    /** Get the full inheritance chain for this template */
    virtual TArray<UEnemyTemplate*> GetInheritanceChain() const;
    //~ End Template Interface
    
    //~ Begin Property Accessors
    /** Get the template's unique name */
    FName GetTemplateName() const { return TemplateName; }
    
    /** Get the template's display name */
    FText GetDisplayName() const { return DisplayName; }
    
    /** Get the parent template if any */
    UEnemyTemplate* GetParentTemplate() const;
    
    /** Get the base stats for this template */
    const FEnemyBaseStats& GetBaseStats() const { return BaseStats; }
    
    /** Get the abilities defined in this template */
    const TArray<FEnemyAbilityDefinition>& GetAbilities() const { return Abilities; }
    //~ End Property Accessors
    
protected:
    //~ Begin Template Properties
    /** Unique name for this template */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template")
    FName TemplateName;
    
    /** Display name for this template */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template")
    FText DisplayName;
    
    /** Template description */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template", meta = (MultiLine = true))
    FText Description;
    
    /** Parent template to inherit from */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template")
    TSoftObjectPtr<UEnemyTemplate> ParentTemplate;
    
    /** Tags for categorizing and filtering templates */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template")
    FGameplayTagContainer TemplateTags;
    //~ End Template Properties
    
    //~ Begin Stats Properties
    /** Base stats for this enemy type */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (ShowOnlyInnerProperties))
    FEnemyBaseStats BaseStats;
    
    /** Stat scaling configuration */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    FEnemyStatScaling StatScaling;
    //~ End Stats Properties
    
    //~ Begin Visual Properties
    /** Base mesh for this enemy type */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    TSoftObjectPtr<USkeletalMesh> BaseMesh;
    
    /** Animation blueprint to use */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    TSoftObjectPtr<UAnimBlueprint> AnimationBlueprint;
    
    /** Material overrides */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    TMap<int32, TSoftObjectPtr<UMaterialInterface>> MaterialOverrides;
    
    /** Visual customization settings */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (ShowOnlyInnerProperties))
    FEnemyVisualCustomization VisualCustomization;
    //~ End Visual Properties
    
    //~ Begin AI Properties
    /** Behavior tree asset */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BehaviorTree;
    
    /** AI configuration data */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ShowOnlyInnerProperties))
    FEnemyAIConfig AIConfig;
    //~ End AI Properties
    
    //~ Begin Ability Properties
    /** Abilities available to this enemy type */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    TArray<FEnemyAbilityDefinition> Abilities;
    //~ End Ability Properties
    
private:
    //~ Begin Helper Functions
    /** Validate template dependencies */
    bool ValidateTemplateDependencies(FString& OutError) const;
    
    /** Validate visual assets */
    bool ValidateVisualAssets(FString& OutError) const;
    
    /** Validate AI configuration */
    bool ValidateAIConfig(FString& OutError) const;
    
    /** Validate abilities */
    bool ValidateAbilities(FString& OutError) const;
    
    /** Apply inherited properties */
    void ApplyInheritedProperties();
    
    /** Merge ability definitions */
    void MergeAbilityDefinitions(const TArray<FEnemyAbilityDefinition>& InheritedAbilities);
    //~ End Helper Functions
    
    /** Cached inheritance chain */
    UPROPERTY(Transient)
    TArray<UEnemyTemplate*> CachedInheritanceChain;
    
    /** Whether the inheritance chain needs updating */
    UPROPERTY(Transient)
    bool bInheritanceChainDirty;
    
    friend class UEnemyTemplateManager;
}; 