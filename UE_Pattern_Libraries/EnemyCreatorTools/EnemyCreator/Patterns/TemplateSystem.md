# Template System Pattern

## Problem Statement
Creating a flexible and maintainable template system for enemy creation that supports inheritance, overrides, and validation while remaining performant and user-friendly.

## Solution Overview
A hierarchical template system using data assets and configuration objects to define enemy templates with inheritance support and runtime validation.

## Core Components

### 1. Template Asset
```cpp
UCLASS(BlueprintType)
class ENEMYCREATOR_API UEnemyTemplate : public UDataAsset
{
    GENERATED_BODY()
public:
    // Basic properties
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template")
    FName TemplateName;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template")
    FText DisplayName;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template")
    FText Description;
    
    // Inheritance
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Template")
    TSoftObjectPtr<UEnemyTemplate> ParentTemplate;
    
    // Core stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    FEnemyBaseStats BaseStats;
    
    // Visual setup
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    TSoftObjectPtr<USkeletalMesh> BaseMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    TSoftObjectPtr<UAnimBlueprint> AnimationBlueprint;
    
    // AI configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BehaviorTree;
    
    // Abilities
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    TArray<FEnemyAbilityDefinition> Abilities;
    
    // Template interface
    virtual bool ValidateTemplate(FString& OutError);
    virtual void ApplyToEnemy(ABaseEnemy* Enemy);
    virtual UEnemyTemplate* CreateChildTemplate();
};
```

### 2. Configuration System
```cpp
USTRUCT(BlueprintType)
struct ENEMYCREATOR_API FEnemyConfiguration
{
    GENERATED_BODY()
    
    // Template reference
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template")
    TSoftObjectPtr<UEnemyTemplate> BaseTemplate;
    
    // Stat overrides
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    TMap<FName, float> StatMultipliers;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    TMap<FName, float> StatOverrides;
    
    // Visual overrides
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FEnemyVisualOverrides VisualOverrides;
    
    // AI overrides
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FEnemyAIOverrides AIOverrides;
    
    // Ability modifications
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TArray<FEnemyAbilityModification> AbilityModifications;
    
    // Configuration interface
    bool ValidateConfiguration(FString& OutError) const;
    bool ApplyConfiguration(ABaseEnemy* Enemy) const;
    FEnemyConfiguration CreateVariant() const;
};
```

### 3. Template Manager
```cpp
UCLASS()
class ENEMYCREATOR_API UEnemyTemplateManager : public UObject
{
    GENERATED_BODY()
public:
    // Template management
    UFUNCTION(BlueprintCallable, Category = "Templates")
    UEnemyTemplate* CreateNewTemplate(const FString& Name, UEnemyTemplate* Parent = nullptr);
    
    UFUNCTION(BlueprintCallable, Category = "Templates")
    bool SaveTemplate(UEnemyTemplate* Template);
    
    UFUNCTION(BlueprintCallable, Category = "Templates")
    bool DeleteTemplate(UEnemyTemplate* Template);
    
    // Template queries
    UFUNCTION(BlueprintCallable, Category = "Templates")
    TArray<UEnemyTemplate*> GetAllTemplates() const;
    
    UFUNCTION(BlueprintCallable, Category = "Templates")
    TArray<UEnemyTemplate*> GetTemplateChildren(UEnemyTemplate* Parent) const;
    
    // Template validation
    UFUNCTION(BlueprintCallable, Category = "Templates")
    bool ValidateTemplateHierarchy(UEnemyTemplate* Template, FString& OutError) const;
    
protected:
    // Template cache
    UPROPERTY()
    TMap<FName, UEnemyTemplate*> TemplateCache;
    
    // Helper functions
    void UpdateTemplateCache();
    bool ValidateTemplateDependencies(UEnemyTemplate* Template) const;
};
```

## Usage Examples

### 1. Creating a Template Hierarchy
```cpp
// Create base melee enemy template
UEnemyTemplate* BaseMelee = TemplateManager->CreateNewTemplate("BaseMelee");
BaseMelee->BaseStats.SetHealth(100.0f);
BaseMelee->BaseStats.SetDamage(20.0f);
BaseMelee->BaseStats.SetSpeed(300.0f);

// Create specialized assassin template
UEnemyTemplate* Assassin = TemplateManager->CreateNewTemplate("Assassin", BaseMelee);
Assassin->StatMultipliers.Add("Speed", 1.5f);
Assassin->StatMultipliers.Add("Damage", 1.2f);
Assassin->StatMultipliers.Add("Health", 0.8f);

// Add assassin-specific abilities
FEnemyAbilityDefinition StealthAbility;
StealthAbility.AbilityName = "Stealth";
StealthAbility.CooldownTime = 15.0f;
Assassin->Abilities.Add(StealthAbility);
```

### 2. Creating and Applying Configurations
```cpp
// Create configuration from template
FEnemyConfiguration Config;
Config.BaseTemplate = Assassin;

// Add stat modifications
Config.StatMultipliers.Add("Damage", 1.2f); // 20% more damage
Config.StatOverrides.Add("Health", 150.0f); // Set health to 150

// Apply visual overrides
Config.VisualOverrides.MeshOverride = AssassinEliteMesh;
Config.VisualOverrides.MaterialOverrides.Add(0, EliteMaterial);

// Apply to enemy
ABaseEnemy* Enemy = SpawnEnemy();
Config.ApplyConfiguration(Enemy);
```

## Best Practices

1. **Template Design**
   - Create clear template hierarchies
   - Keep templates focused and single-purpose
   - Document template requirements
   - Use meaningful naming conventions

2. **Configuration Management**
   - Validate all configurations
   - Handle missing references gracefully
   - Implement version control support
   - Cache template data appropriately

3. **Performance Optimization**
   - Minimize template loading times
   - Optimize inheritance chain resolution
   - Cache frequently used templates
   - Implement proper garbage collection

## Common Pitfalls

1. **Template Management**
   - Circular dependencies in template hierarchy
   - Missing or invalid references
   - Inconsistent override behavior
   - Poor template organization

2. **Configuration Issues**
   - Invalid stat modifications
   - Unhandled null references
   - Incorrect inheritance resolution
   - Missing validation checks

## Performance Considerations

1. **Template Loading**
```cpp
// Implement async template loading
void UEnemyTemplateManager::LoadTemplateAsync(
    const FName& TemplateName, 
    FTemplateLoadedDelegate OnLoaded)
{
    // Check cache first
    if (UEnemyTemplate* CachedTemplate = TemplateCache.FindRef(TemplateName))
    {
        OnLoaded.ExecuteIfBound(CachedTemplate);
        return;
    }
    
    // Load asynchronously
    StreamableManager.RequestAsyncLoad(
        TemplatePath / TemplateName.ToString(),
        FStreamableDelegate::CreateUObject(
            this,
            &UEnemyTemplateManager::OnTemplateLoaded,
            TemplateName,
            OnLoaded
        )
    );
}
```

2. **Template Caching**
```cpp
// Implement smart template caching
void UEnemyTemplateManager::UpdateTemplateCache()
{
    // Clear unused templates
    for (auto It = TemplateCache.CreateIterator(); It; ++It)
    {
        if (!It.Value() || It.Value()->HasAnyFlags(RF_Unreachable))
        {
            It.RemoveCurrent();
        }
    }
    
    // Add commonly used templates
    TArray<FAssetData> TemplateAssets;
    GetAllTemplateAssets(TemplateAssets);
    
    for (const FAssetData& AssetData : TemplateAssets)
    {
        if (ShouldCacheTemplate(AssetData))
        {
            LoadTemplateIntoCache(AssetData);
        }
    }
}
```

## Testing Strategy

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyTemplateTest, "EnemyCreator.Templates",
    EAutomationTestFlags::ApplicationContextMask | 
    EAutomationTestFlags::ProductFilter)

bool FEnemyTemplateTest::RunTest(const FString& Parameters)
{
    // Test template creation
    UEnemyTemplate* Template = CreateTestTemplate();
    TestNotNull("Template created successfully", Template);
    
    // Test inheritance
    UEnemyTemplate* Child = CreateChildTemplate(Template);
    TestTrue("Inheritance valid", ValidateTemplateHierarchy(Child));
    
    // Test configuration
    FEnemyConfiguration Config;
    Config.BaseTemplate = Template;
    TestTrue("Configuration valid", Config.ValidateConfiguration());
    
    return true;
}
```

## Implementation Templates

Place these files in your project's Implementation directory:

1. `EnemyTemplate.h` - Base template class
2. `EnemyConfiguration.h` - Configuration system
3. `EnemyTemplateManager.h` - Template management
4. `EnemyTemplateTypes.h` - Common types and structures

## References

- [UE5 Data Asset System](https://docs.unrealengine.com/5.0/en-US/data-assets-in-unreal-engine/)
- [Asset Management](https://docs.unrealengine.com/5.0/en-US/asset-management-in-unreal-engine/)
- [Property System](https://docs.unrealengine.com/5.0/en-US/property-system-in-unreal-engine/) 