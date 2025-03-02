# Enemy Creator Tool - Technical Reference

## Architecture Overview

### 1. Core Components

#### EnemyCreatorTool
```cpp
UCLASS()
class GAME_API UEnemyCreatorTool : public UEditorUtilityWidget
{
    // Main editor interface
    // Template management
    // Preview system
    // Direct behavior tree management
}
```

#### EnemyPreviewViewport
```cpp
UCLASS()
class GAME_API UEnemyPreviewViewport : public UEditorViewportClient
{
    // 3D preview rendering
    // Debug visualization
    // Camera controls
}
```

#### Property Customization
```cpp
UCLASS()
class GAME_API UEnemyPropertyCustomization : public UObject
{
    // Property editing interface
    // Validation system
    // Change notification
}
```

### 2. Data Structures

#### Enemy Template
```cpp
USTRUCT(BlueprintType)
struct FEnemyTemplate
{
    GENERATED_BODY()
    
    // Base properties
    UPROPERTY(EditDefaultsOnly, Category = "Template")
    FString TemplateName;
    
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    FEnemyBaseStats BaseStats;
    
    // Visual assets
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    TSoftObjectPtr<USkeletalMesh> BaseMesh;
    
    // AI configuration
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BaseBehaviorTree;
}
```

#### Enemy Configuration
```cpp
USTRUCT(BlueprintType)
struct FEnemyConfiguration
{
    GENERATED_BODY()
    
    // Template reference
    UPROPERTY(EditDefaultsOnly, Category = "Template")
    TSoftObjectPtr<UEnemyTemplate> BaseTemplate;
    
    // Instance overrides
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    FEnemyStatsOverride StatsOverride;
    
    // Behavior configuration
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    EBehaviorPattern DefaultBehaviorPattern;
}
```

## Integration Points

### 1. AI System Integration

#### Behavior Pattern System
```cpp
// Predefined behavior patterns
UENUM(BlueprintType)
enum class EBehaviorPattern : uint8
{
    Patrol,
    Aggressive,
    Defensive,
    Support
};

// Pattern application
class FBehaviorPatternSystem
{
public:
    // Apply pattern to behavior tree
    static void ApplyPattern(UBehaviorTree* BehaviorTree, EBehaviorPattern Pattern);
    
    // Combine multiple patterns
    static void CombinePatterns(UBehaviorTree* BehaviorTree, TArray<EBehaviorPattern> Patterns);
};
```

#### Custom Task Creation
```cpp
// Example custom task
UCLASS()
class GAME_API UBTTask_CustomEnemyAction : public UBTTaskNode
{
    GENERATED_BODY()
    
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};
```

### 2. Editor Integration

#### Custom Details Panel
```cpp
class FEnemyDetailsCustomization : public IDetailCustomization
{
public:
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
    
    // Add custom categories
    void AddAICategory(IDetailLayoutBuilder& DetailBuilder);
    void AddCombatCategory(IDetailLayoutBuilder& DetailBuilder);
};
```

#### Preview Scene Setup
```cpp
void UEnemyPreviewViewport::SetupPreviewScene()
{
    // Create preview scene
    PreviewScene = new FPreviewScene();
    
    // Setup lighting
    PreviewScene->SetLightDirection(FRotator(-45.0f, -45.0f, 0.0f));
    PreviewScene->SetLightBrightness(8.0f);
    
    // Add preview floor
    FPreviewSceneFloor* PreviewFloor = new FPreviewSceneFloor(PreviewScene);
    PreviewScene->AddComponent(PreviewFloor->GetComponent(), FTransform::Identity);
}
```

## Extension Points

### 1. Adding New Features

#### Custom Property Types
```cpp
// Create custom property
USTRUCT(BlueprintType)
struct FCustomEnemyProperty
{
    GENERATED_BODY()
    
    // Property data
    UPROPERTY(EditDefaultsOnly)
    float Value;
    
    // Validation
    bool Validate() const;
};

// Register property type
void RegisterCustomProperty()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomPropertyTypeLayout(
        FCustomEnemyProperty::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomPropertyCustomization::MakeInstance)
    );
}
```

### 2. Preset System

#### Enemy Presets
```cpp
// Preset definitions
struct FEnemyPreset
{
    FString PresetName;
    FEnemyBaseStats BaseStats;
    EBehaviorPattern DefaultBehavior;
    
    static FEnemyPreset CreateBalancedPreset();
    static FEnemyPreset CreateAggressivePreset();
    static FEnemyPreset CreateDefensivePreset();
};

// Preset application
class FPresetSystem
{
public:
    static void ApplyPreset(UEnemyTemplate* Template, const FEnemyPreset& Preset);
    static void CustomizePreset(UEnemyTemplate* Template, const FEnemyPreset& Preset, const FEnemyCustomization& Custom);
};
```

Remember: This tool is designed for direct development in the Cursor IDE, focusing on efficient preset-based enemy creation and behavior pattern application.