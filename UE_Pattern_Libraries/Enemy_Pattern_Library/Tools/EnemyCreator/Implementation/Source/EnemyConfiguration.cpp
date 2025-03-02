#include "EnemyCreatorTypes.h"
#include "BaseEnemy.h"
#include "AbilitySystemComponent.h"

void UEnemyConfiguration::InitializeFromTemplate(UEnemyTemplate* Template)
{
    if (!Template)
    {
        return;
    }
    
    // Store template reference
    BaseTemplate = Template;
    
    // Initialize modifications with template defaults
    Modifications.StatMultipliers.Empty();
    Modifications.ModifiedAbilities.Empty();
    
    // Copy AI config
    Modifications.AIModifications = Template->GetAIConfig();
    
    // Copy visual customization
    Modifications.VisualModifications = Template->GetVisualCustomization();
    
    // Initialize with template's gameplay tags
    Modifications.AdditionalTags = Template->GetTemplateTags();
}

bool UEnemyConfiguration::ApplyConfiguration(ABaseEnemy* Enemy)
{
    if (!Enemy || !BaseTemplate.IsValid())
    {
        return false;
    }
    
    // Get the template
    UEnemyTemplate* Template = BaseTemplate.Get();
    if (!Template)
    {
        return false;
    }
    
    // Apply template with modifications
    return Template->ApplyToInstance(Enemy, &Modifications);
}

bool UEnemyConfiguration::ValidateConfiguration(FEnemyTemplateValidationResult& OutResult) const
{
    OutResult.Clear();
    
    // Validate base template
    if (!BaseTemplate.IsValid())
    {
        OutResult.AddError(NSLOCTEXT("EnemyCreator", "NoBaseTemplate", "No base template specified"));
        return false;
    }
    
    // Get the template
    UEnemyTemplate* Template = BaseTemplate.Get();
    if (!Template)
    {
        OutResult.AddError(NSLOCTEXT("EnemyCreator", "InvalidBaseTemplate", "Base template reference is invalid"));
        return false;
    }
    
    // Validate template first
    if (!Template->ValidateTemplate(OutResult))
    {
        return false;
    }
    
    // Validate stat multipliers
    for (const auto& StatMod : Modifications.StatMultipliers)
    {
        if (StatMod.Value <= 0.0f)
        {
            OutResult.AddError(FText::Format(
                NSLOCTEXT("EnemyCreator", "InvalidStatMultiplier", "Invalid multiplier value {0} for stat {1}"),
                FText::AsNumber(StatMod.Value),
                FText::FromName(StatMod.Key)
            ));
            return false;
        }
    }
    
    // Validate modified abilities
    for (const auto& AbilityMod : Modifications.ModifiedAbilities)
    {
        if (AbilityMod.Key.IsNone())
        {
            OutResult.AddError(NSLOCTEXT("EnemyCreator", "InvalidAbilityModification", "Invalid ability modification key"));
            return false;
        }
        
        // Verify the ability exists in template
        bool bAbilityFound = false;
        for (const auto& TemplateAbility : Template->GetAbilities())
        {
            if (TemplateAbility.AbilityName == AbilityMod.Key)
            {
                bAbilityFound = true;
                break;
            }
        }
        
        if (!bAbilityFound)
        {
            OutResult.AddError(FText::Format(
                NSLOCTEXT("EnemyCreator", "UnknownAbilityModification", "Modification targets unknown ability {0}"),
                FText::FromName(AbilityMod.Key)
            ));
            return false;
        }
    }
    
    return true;
} 