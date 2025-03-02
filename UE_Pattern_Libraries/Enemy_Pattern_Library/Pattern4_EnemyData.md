# Pattern 4: Enemy Data-Driven Design

## Problem Statement
Creating a data-driven framework that allows non-engineers to safely create, modify, and configure enemy content while maintaining system integrity and performance.

## Solution Overview
A comprehensive data asset system with validation, visual tools, and template-based creation to enable content creators to work efficiently without requiring direct code access.

## Core Components

### 1. Enemy Template System

```cpp
// Base enemy template data asset
UCLASS(BlueprintType)
class GAME_API UEnemyTemplate : public UDataAsset
{
    GENERATED_BODY()
public:
    // Template identification
    UPROPERTY(EditDefaultsOnly, Category = "Template")
    FName TemplateName;
    
    UPROPERTY(EditDefaultsOnly, Category = "Template")
    FText DisplayName;
    
    // Base stats template
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    FEnemyBaseStats BaseStats;
    
    // Visual assets
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    TSoftObjectPtr<USkeletalMesh> BaseMesh;
    
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    TSoftObjectPtr<UAnimBlueprint> BaseAnimBlueprint;
    
    // Behavior template
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BaseBehaviorTree;
    
    // Ability templates
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<FEnemyAbilityTemplate> BaseAbilities;
    
    // Validation interface
    virtual bool ValidateTemplate(FString& OutError);
    virtual bool CanInheritFrom(const UEnemyTemplate* OtherTemplate) const;
};
```

### 2. Enemy Configuration System

```cpp
// Enemy instance configuration
UCLASS(BlueprintType)
class GAME_API UEnemyConfiguration : public UDataAsset
{
    GENERATED_BODY()
public:
    // Template reference
    UPROPERTY(EditDefaultsOnly, Category = "Template")
    TSoftObjectPtr<UEnemyTemplate> BaseTemplate;
    
    // Instance overrides
    UPROPERTY(EditDefaultsOnly, Category = "Stats", meta=(ShowOnlyInnerProperties))
    FEnemyStatsOverride StatsOverride;
    
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FEnemyVisualOverride VisualOverride;
    
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    FEnemyAbilityOverride AbilityOverride;
    
    // Validation and application
    bool ApplyConfiguration(ABaseEnemy* TargetEnemy) const;
    bool ValidateConfiguration(FString& OutError) const;
};

// Stats override structure
USTRUCT(BlueprintType)
struct FEnemyStatsOverride
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly)
    TMap<FName, float> StatMultipliers;
    
    UPROPERTY(EditDefaultsOnly)
    TMap<FName, float> StatAdditions;
    
    UPROPERTY(EditDefaultsOnly)
    TMap<FName, FString> StatReplacements;
};
```

### 3. Visual Tools Integration

```cpp
// Enemy editor tool interface
UCLASS()
class GAME_API UEnemyEditorTool : public UObject
{
    GENERATED_BODY()
public:
    // Template management
    UFUNCTION(BlueprintCallable, Category = "Enemy Editor")
    UEnemyTemplate* CreateTemplateFromExisting(UEnemyTemplate* BaseTemplate);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Editor")
    bool ValidateTemplateInheritance(UEnemyTemplate* DerivedTemplate);
    
    // Configuration tools
    UFUNCTION(BlueprintCallable, Category = "Enemy Editor")
    UEnemyConfiguration* CreateConfigurationFromTemplate(UEnemyTemplate* Template);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Editor")
    bool PreviewConfiguration(UEnemyConfiguration* Config, class UEnemyPreviewActor* PreviewActor);
};
```

## Usage Examples

### 1. Creating Enemy Templates

```cpp
// Example of template creation and validation
UEnemyTemplate* UEnemyEditorTool::CreateTemplateFromExisting(UEnemyTemplate* BaseTemplate)
{
    if (!BaseTemplate)
    {
        return nullptr;
    }
    
    // Create new template
    UEnemyTemplate* NewTemplate = DuplicateObject<UEnemyTemplate>(BaseTemplate, nullptr);
    
    // Clear unique identifiers
    NewTemplate->TemplateName = NAME_None;
    NewTemplate->DisplayName = FText::GetEmpty();
    
    // Validate inheritance
    FString ValidationError;
    if (!NewTemplate->ValidateTemplate(ValidationError))
    {
        UE_LOG(LogEnemyEditor, Warning, TEXT("Template validation failed: %s"), *ValidationError);
        return nullptr;
    }
    
    return NewTemplate;
}
```

### 2. Configuring Enemies

```cpp
// Example of applying configuration
bool UEnemyConfiguration::ApplyConfiguration(ABaseEnemy* TargetEnemy) const
{
    if (!TargetEnemy || !BaseTemplate.IsValid())
    {
        return false;
    }
    
    // Apply base template
    if (!BaseTemplate->ApplyToEnemy(TargetEnemy))
    {
        return false;
    }
    
    // Apply stat overrides
    for (const auto& StatOverride : StatsOverride.StatMultipliers)
    {
        TargetEnemy->MultiplyStatValue(StatOverride.Key, StatOverride.Value);
    }
    
    // Apply visual overrides
    if (VisualOverride.MeshOverride.IsValid())
    {
        TargetEnemy->GetMesh()->SetSkeletalMesh(VisualOverride.MeshOverride.Get());
    }
    
    // Apply ability overrides
    if (UEnemyAbilityComponent* AbilityComp = TargetEnemy->GetAbilityComponent())
    {
        for (const auto& AbilityOverride : AbilityOverride.ModifiedAbilities)
        {
            AbilityComp->ModifyAbility(AbilityOverride.AbilityName, AbilityOverride);
        }
    }
    
    return true;
}
```

### 3. Editor Tool Integration

```cpp
// Example of editor preview functionality
bool UEnemyEditorTool::PreviewConfiguration(UEnemyConfiguration* Config, UEnemyPreviewActor* PreviewActor)
{
    if (!Config || !PreviewActor)
    {
        return false;
    }
    
    // Setup preview actor
    PreviewActor->ResetToDefault();
    
    // Apply configuration
    if (!Config->ApplyConfiguration(PreviewActor))
    {
        return false;
    }
    
    // Update preview viewport
    if (UEnemyEditorViewport* Viewport = GetEditorViewport())
    {
        Viewport->RefreshViewport();
    }
    
    return true;
}
```

## Best Practices

1. **Template Design**
   - Create clear template hierarchies
   - Implement thorough validation
   - Document template requirements

2. **Configuration Management**
   - Use meaningful default values
   - Implement version control
   - Provide clear error messages

3. **Tool Development**
   - Create intuitive user interfaces
   - Implement undo/redo support
   - Provide real-time preview

## Integration Guidelines

1. **With Content Pipeline**
   ```cpp
   // Example of content pipeline integration
   void UEnemyEditorTool::OnAssetCreated(UObject* Asset)
   {
       if (UEnemyTemplate* Template = Cast<UEnemyTemplate>(Asset))
       {
           // Register template in content browser
           RegisterNewTemplate(Template);
           
           // Update template cache
           UpdateTemplateCache();
           
           // Notify content browser
           NotifyTemplateCreated(Template);
       }
   }
   ```

2. **With Version Control**
   ```cpp
   // Example of version control integration
   bool UEnemyEditorTool::SaveTemplate(UEnemyTemplate* Template)
   {
       if (!Template)
       {
           return false;
       }
       
       // Check out file
       if (!SourceControlHelpers::CheckOutFile(Template->GetPackage()))
       {
           return false;
       }
       
       // Save package
       return UPackage::SavePackage(Template->GetPackage(),
           Template,
           RF_Standalone | RF_Public,
           *FPackageName::LongPackageNameToFilename(Template->GetPackage()->GetName()));
   }
   ```

## Common Pitfalls

1. **Data Validation**
   - Missing validation checks
   - Incomplete error handling
   - Poor user feedback

2. **Performance Impact**
   - Inefficient data structures
   - Heavy runtime validation
   - Resource leaks

3. **User Experience**
   - Confusing interface
   - Poor error messages
   - Lack of documentation

## Testing Strategy

1. **Template Testing**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyTemplateTest, "Enemy.Data.Template",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyTemplateTest::RunTest(const FString& Parameters)
   {
       // Test template creation
       UEnemyTemplate* Template = CreateTestTemplate();
       TestNotNull("Template should be created", Template);
       
       // Test template validation
       FString ValidationError;
       TestTrue("Template should be valid", Template->ValidateTemplate(ValidationError));
       
       // Test template inheritance
       UEnemyTemplate* DerivedTemplate = CreateDerivedTemplate(Template);
       TestTrue("Should allow inheritance", Template->CanInheritFrom(DerivedTemplate));
       
       return true;
   }
   ```

2. **Configuration Testing**
   ```cpp
   IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyConfigTest, "Enemy.Data.Config",
       EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
   
   bool FEnemyConfigTest::RunTest(const FString& Parameters)
   {
       // Test configuration creation
       UEnemyConfiguration* Config = CreateTestConfig();
       TestNotNull("Config should be created", Config);
       
       // Test configuration application
       ABaseEnemy* TestEnemy = CreateTestEnemy();
       TestTrue("Should apply configuration", Config->ApplyConfiguration(TestEnemy));
       
       // Test override values
       TestEqual("Should apply stat override",
           TestEnemy->GetStatValue("Health"),
           Config->StatsOverride.StatMultipliers.FindRef("Health"));
       
       return true;
   }
   ```

## Debugging Tools

1. **Template Debugging**
   ```cpp
   void UEnemyTemplate::DebugPrintTemplate() const
   {
       UE_LOG(LogEnemyEditor, Log, TEXT("Template: %s"), *GetName());
       UE_LOG(LogEnemyEditor, Log, TEXT("Stats:"));
       for (const auto& Stat : BaseStats.Stats)
       {
           UE_LOG(LogEnemyEditor, Log, TEXT("  %s: %f"), *Stat.Key.ToString(), Stat.Value);
       }
       UE_LOG(LogEnemyEditor, Log, TEXT("Abilities:"));
       for (const auto& Ability : BaseAbilities)
       {
           UE_LOG(LogEnemyEditor, Log, TEXT("  %s"), *Ability.AbilityName.ToString());
       }
   }
   ```

2. **Configuration Validation**
   ```cpp
   void UEnemyConfiguration::ValidateAndLogIssues() const
   {
       FString ValidationError;
       if (!ValidateConfiguration(ValidationError))
       {
           UE_LOG(LogEnemyEditor, Warning, TEXT("Configuration validation failed:"));
           UE_LOG(LogEnemyEditor, Warning, TEXT("  %s"), *ValidationError);
       }
       
       // Check for potential issues
       for (const auto& StatOverride : StatsOverride.StatMultipliers)
       {
           if (StatOverride.Value <= 0.0f)
           {
               UE_LOG(LogEnemyEditor, Warning, TEXT("Invalid stat multiplier for %s: %f"),
                   *StatOverride.Key.ToString(), StatOverride.Value);
           }
       }
   }
   ```

## Editor Integration

1. **Custom Editor Views**
   ```cpp
   void FEnemyTemplateEditor::RegisterLayoutExtension(FLayoutExtender& Extender)
   {
       Extender.ExtendLayout(TEXT("EnemyTemplate"), ELayoutExtensionPosition::After,
           FCreatePropertyView::CreateLambda([](const FPropertyViewParameters& Params)
           {
               return SNew(SEnemyTemplatePropertyView)
                   .Object(Params.Object)
                   .ReadOnly(Params.bIsReadOnly);
           }));
   }
   ```

2. **Property Customization**
   ```cpp
   void FEnemyPropertyCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
   {
       // Add custom category
       IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Enemy");
       
       // Add custom rows
       Category.AddCustomRow(FText::FromString("Preview"))
           .NameContent()
           [
               SNew(STextBlock)
               .Text(FText::FromString("Preview"))
           ]
           .ValueContent()
           [
               SNew(SEnemyPreviewWidget)
               .Enemy(GetEditedEnemy())
           ];
   }
   ```

## Credits

This pattern is part of the Enemy Pattern Library, designed to provide a robust foundation for data-driven enemy content creation in Unreal Engine 5. 