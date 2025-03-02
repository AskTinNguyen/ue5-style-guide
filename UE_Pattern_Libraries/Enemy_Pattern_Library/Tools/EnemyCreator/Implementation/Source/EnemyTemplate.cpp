#include "EnemyTemplate.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"

UEnemyTemplate::UEnemyTemplate()
{
    // Initialize default values
    BaseStats = FEnemyBaseStats();
    AIConfig = FEnemyAIConfig();
    VisualCustomization = FEnemyVisualCustomization();
    bNeedsInheritanceChainUpdate = true;
}

bool UEnemyTemplate::ValidateTemplate(FEnemyTemplateValidationResult& OutResult) const
{
    OutResult.Clear();
    
    // Validate basic properties
    if (TemplateName.IsNone())
    {
        OutResult.AddError(NSLOCTEXT("EnemyCreator", "NoTemplateName", "Template name is required"));
    }
    
    if (DisplayName.IsEmpty())
    {
        OutResult.AddWarning(NSLOCTEXT("EnemyCreator", "NoDisplayName", "Display name is empty"));
    }
    
    // Validate parent template
    if (ParentTemplate)
    {
        FEnemyTemplateValidationResult ParentResult;
        if (!ParentTemplate->ValidateTemplate(ParentResult))
        {
            OutResult.AddError(FText::Format(
                NSLOCTEXT("EnemyCreator", "InvalidParentTemplate", "Parent template '{0}' is invalid"),
                FText::FromString(ParentTemplate->GetTemplateName().ToString())
            ));
            OutResult.ValidationErrors.Append(ParentResult.ValidationErrors);
        }
    }
    
    // Validate visual assets
    if (!ValidateVisualAssets(OutResult))
    {
        return false;
    }
    
    // Validate AI configuration
    if (!ValidateAIConfiguration(OutResult))
    {
        return false;
    }
    
    // Validate abilities
    if (!ValidateAbilities(OutResult))
    {
        return false;
    }
    
    return OutResult.bIsValid;
}

bool UEnemyTemplate::ApplyToInstance(ACharacter* EnemyInstance, const FEnemyTemplateModification* Modification) const
{
    if (!EnemyInstance)
    {
        return false;
    }
    
    // Apply base stats
    FEnemyBaseStats FinalStats = BaseStats;
    if (Modification)
    {
        for (const auto& StatMod : Modification->StatMultipliers)
        {
            float* StatValue = GetStatValuePtr(FinalStats, StatMod.Key);
            if (StatValue)
            {
                *StatValue *= StatMod.Value;
            }
        }
    }
    
    // Apply visual customization
    FEnemyVisualCustomization FinalVisuals = VisualCustomization;
    if (Modification)
    {
        FinalVisuals = Modification->VisualModifications;
    }
    ApplyVisualCustomization(EnemyInstance, FinalVisuals);
    
    // Apply AI configuration
    FEnemyAIConfig FinalAIConfig = AIConfig;
    if (Modification)
    {
        FinalAIConfig = Modification->AIModifications;
    }
    ApplyAIConfiguration(EnemyInstance, FinalAIConfig);
    
    // Apply abilities
    UAbilitySystemComponent* AbilitySystem = EnemyInstance->FindComponentByClass<UAbilitySystemComponent>();
    if (AbilitySystem)
    {
        for (const auto& Ability : Abilities)
        {
            if (Modification && Modification->ModifiedAbilities.Contains(Ability.AbilityName))
            {
                ApplyAbility(AbilitySystem, Modification->ModifiedAbilities[Ability.AbilityName]);
            }
            else
            {
                ApplyAbility(AbilitySystem, Ability);
            }
        }
    }
    
    return true;
}

UEnemyTemplate* UEnemyTemplate::CreateChildTemplate(const FName& NewTemplateName)
{
    UEnemyTemplate* ChildTemplate = NewObject<UEnemyTemplate>();
    ChildTemplate->ParentTemplate = this;
    ChildTemplate->TemplateName = NewTemplateName;
    
    // Inherit properties
    ChildTemplate->BaseStats = BaseStats;
    ChildTemplate->AIConfig = AIConfig;
    ChildTemplate->VisualCustomization = VisualCustomization;
    ChildTemplate->Abilities = Abilities;
    
    return ChildTemplate;
}

const TArray<UEnemyTemplate*>& UEnemyTemplate::GetInheritanceChain() const
{
    if (bNeedsInheritanceChainUpdate)
    {
        CachedInheritanceChain.Empty();
        UEnemyTemplate* Current = const_cast<UEnemyTemplate*>(this);
        
        while (Current)
        {
            CachedInheritanceChain.Add(Current);
            Current = Current->ParentTemplate;
            
            // Detect circular inheritance
            if (CachedInheritanceChain.Contains(Current))
            {
                break;
            }
        }
        
        bNeedsInheritanceChainUpdate = false;
    }
    
    return CachedInheritanceChain;
}

bool UEnemyTemplate::ValidateVisualAssets(FEnemyTemplateValidationResult& OutResult) const
{
    // Validate skeletal mesh
    if (!VisualCustomization.SkeletalMesh.IsValid())
    {
        OutResult.AddError(NSLOCTEXT("EnemyCreator", "NoSkeletalMesh", "Skeletal mesh is required"));
        return false;
    }
    
    // Validate animation blueprint
    if (!VisualCustomization.AnimationBlueprint.IsValid())
    {
        OutResult.AddWarning(NSLOCTEXT("EnemyCreator", "NoAnimBP", "No animation blueprint specified"));
    }
    
    return true;
}

bool UEnemyTemplate::ValidateAIConfiguration(FEnemyTemplateValidationResult& OutResult) const
{
    // Validate behavior tree
    if (!AIConfig.BehaviorTree.IsValid())
    {
        OutResult.AddError(NSLOCTEXT("EnemyCreator", "NoBehaviorTree", "Behavior tree is required"));
        return false;
    }
    
    // Validate blackboard
    if (!AIConfig.Blackboard.IsValid())
    {
        OutResult.AddError(NSLOCTEXT("EnemyCreator", "NoBlackboard", "Blackboard is required"));
        return false;
    }
    
    return true;
}

bool UEnemyTemplate::ValidateAbilities(FEnemyTemplateValidationResult& OutResult) const
{
    bool bIsValid = true;
    
    for (const auto& Ability : Abilities)
    {
        if (Ability.AbilityName.IsNone())
        {
            OutResult.AddError(FText::Format(
                NSLOCTEXT("EnemyCreator", "NoAbilityName", "Ability name is required for ability at index {0}"),
                FText::AsNumber(Abilities.IndexOfByKey(Ability))
            ));
            bIsValid = false;
        }
        
        if (!Ability.AbilityClass.IsValid())
        {
            OutResult.AddError(FText::Format(
                NSLOCTEXT("EnemyCreator", "NoAbilityClass", "Ability class is required for ability '{0}'"),
                FText::FromName(Ability.AbilityName)
            ));
            bIsValid = false;
        }
    }
    
    return bIsValid;
}

void UEnemyTemplate::ApplyVisualCustomization(ACharacter* EnemyInstance, const FEnemyVisualCustomization& Visuals) const
{
    if (!EnemyInstance)
    {
        return;
    }
    
    // Apply skeletal mesh
    if (USkeletalMeshComponent* MeshComponent = EnemyInstance->GetMesh())
    {
        if (Visuals.SkeletalMesh.IsValid())
        {
            MeshComponent->SetSkeletalMesh(Visuals.SkeletalMesh.Get());
        }
        
        // Apply scale
        MeshComponent->SetRelativeScale3D(Visuals.Scale);
        
        // Apply material parameters
        for (const auto& ScalarParam : Visuals.ScalarParameters)
        {
            MeshComponent->SetScalarParameterValueOnMaterials(ScalarParam.Key, ScalarParam.Value);
        }
        
        for (const auto& VectorParam : Visuals.VectorParameters)
        {
            MeshComponent->SetVectorParameterValueOnMaterials(VectorParam.Key, VectorParam.Value);
        }
        
        for (const auto& TextureParam : Visuals.TextureParameters)
        {
            if (TextureParam.Value.IsValid())
            {
                MeshComponent->SetTextureParameterValueOnMaterials(TextureParam.Key, TextureParam.Value.Get());
            }
        }
    }
}

void UEnemyTemplate::ApplyAIConfiguration(ACharacter* EnemyInstance, const FEnemyAIConfig& Config) const
{
    if (!EnemyInstance)
    {
        return;
    }
    
    if (AAIController* AIController = Cast<AAIController>(EnemyInstance->GetController()))
    {
        // Apply behavior tree
        if (Config.BehaviorTree.IsValid())
        {
            AIController->RunBehaviorTree(Config.BehaviorTree.Get());
        }
        
        // Apply blackboard
        if (Config.Blackboard.IsValid())
        {
            AIController->UseBlackboard(Config.Blackboard.Get(), nullptr);
        }
        
        // Set behavior parameters
        if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent())
        {
            for (const auto& Param : Config.BehaviorParameters)
            {
                Blackboard->SetValueAsFloat(Param.Key, Param.Value);
            }
        }
    }
}

void UEnemyTemplate::ApplyAbility(UAbilitySystemComponent* AbilitySystem, const FEnemyAbilityDefinition& Ability) const
{
    if (!AbilitySystem || !Ability.AbilityClass.IsValid())
    {
        return;
    }
    
    // Grant ability
    FGameplayAbilitySpec AbilitySpec(
        Ability.AbilityClass.Get(),
        1,  // Level
        INDEX_NONE,  // Input ID
        this  // Source object
    );
    
    AbilitySystem->GiveAbility(AbilitySpec);
    
    // Apply effects
    for (const auto& Effect : Ability.AbilityEffects)
    {
        if (Effect.IsValid())
        {
            FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
            EffectContext.AddSourceObject(this);
            
            const FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(
                Effect.Get(),
                1,  // Level
                EffectContext
            );
            
            if (SpecHandle.IsValid())
            {
                AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            }
        }
    }
} 