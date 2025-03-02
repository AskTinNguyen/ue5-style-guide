# Enemy Creator Tool Documentation

## Overview
The Enemy Creator Tool is a powerful, user-friendly interface designed for game designers to create and configure enemies in Unreal Engine 5. It combines visual editing capabilities with AI assistance to streamline the enemy creation process while maintaining high-quality standards and performance optimization.

## Features

### 1. Visual Editor Interface
- **Template Management**
  - Create new enemy templates
  - Modify existing templates
  - Inherit from base templates
  - Real-time template validation

- **3D Preview System**
  - Real-time mesh preview
  - Animation playback
  - Combat radius visualization
  - Ability range display
  - AI behavior simulation

- **Property Customization**
  - Stat configuration
  - Visual asset assignment
  - Combat behavior setup
  - AI parameter tuning

### 2. AI Integration (AI Coding Assistant)
- **Behavior Tree Generation**
  ```cpp
  // Example: Generate a behavior tree from description
  void GenerateAIBehaviorTree(const FString& Description)
  {
      // "Create a ranged enemy that maintains distance and uses cover"
      EnemyCreatorTool->GenerateAIBehaviorTree(Description);
  }
  ```

- **Ability Suggestions**
  ```cpp
  // Example: Get ability suggestions based on enemy type
  void GetAbilitySuggestions(const FString& EnemyDescription)
  {
      // "A fast-moving assassin type enemy with stealth abilities"
      EnemyCreatorTool->SuggestAbilities(EnemyDescription);
  }
  ```

### 3. Debug Visualization
- **AI Debug Display**
  - Behavior tree state
  - Decision making process
  - Target selection
  - Path finding

- **Combat Visualization**
  - Attack ranges
  - Ability radiuses
  - Line of sight
  - Cover positions

## Usage Guide

### 1. Creating a New Enemy

```cpp
// 1. Create a new template
UEnemyTemplate* Template = CreateNewEnemyTemplate("AssassinEnemy", EEnemyType::Elite);

// 2. Configure base properties
Template->SetBaseStats(FEnemyBaseStats{
    Health = 100.0f,
    Damage = 25.0f,
    Speed = 600.0f
});

// 3. Generate AI behavior
GenerateAIBehaviorTree("Stealthy assassin that flanks and performs surprise attacks");

// 4. Apply suggested abilities
SuggestAbilities("Fast melee assassin with stealth and dash abilities");
```

### 2. Using Templates

```cpp
// 1. Get available templates
TArray<UEnemyTemplate*> Templates = GetAvailableTemplates();

// 2. Create configuration from template
UEnemyConfiguration* Config = CreateEnemyConfiguration(SelectedTemplate);

// 3. Customize configuration
Config->ModifyStats("Health", 150.0f);
Config->AddAbility("Stealth");
```

### 3. Preview and Testing

```cpp
// 1. Update preview
UpdatePreview(Config);

// 2. Test animations
PlayAnimation(StealthAnimation);

// 3. Simulate AI
SimulateAIBehavior();

// 4. Toggle debug info
ShowAIDebugInfo(true);
ShowCombatRadius(true);
```

## Best Practices

### 1. Template Design
- Create clear template hierarchies
- Use meaningful naming conventions
- Document template requirements
- Implement thorough validation

### 2. AI Behavior Description
- Be specific about desired behaviors
- Include combat style preferences
- Specify movement patterns
- Define target selection criteria

### 3. Performance Optimization
- Monitor resource usage
- Use appropriate LODs
- Optimize behavior tree complexity
- Profile AI performance

## Integration with Other Systems

### 1. Combat System
```cpp
// Example: Integrating with combat system
void SetupCombatBehavior(UEnemyConfiguration* Config)
{
    // Configure combat parameters
    Config->SetCombatStyle(ECombatStyle::Stealth);
    Config->SetPreferredRange(300.0f);
    Config->SetAggressionLevel(0.7f);
}
```

### 2. Animation System
```cpp
// Example: Setting up animations
void ConfigureAnimations(UEnemyConfiguration* Config)
{
    // Assign animation blueprint
    Config->SetAnimationBlueprint(AssassinAnimBP);
    
    // Configure animation states
    Config->AddAnimationState("Stealth_Idle");
    Config->AddAnimationState("Stealth_Walk");
    Config->AddAnimationTransition("Stealth_Idle", "Stealth_Walk");
}
```

## Troubleshooting

### Common Issues
1. **Template Validation Failures**
   - Check required properties
   - Verify inheritance chain
   - Validate asset references

2. **AI Generation Issues**
   - Be more specific in descriptions
   - Check API connectivity
   - Verify behavior tree syntax

3. **Performance Problems**
   - Reduce behavior tree complexity
   - Optimize ability calculations
   - Check resource usage

## Example Workflows

### 1. Creating a Boss Enemy
```cpp
// 1. Create boss template
UEnemyTemplate* BossTemplate = CreateNewEnemyTemplate("DragonBoss", EEnemyType::Boss);

// 2. Generate complex AI
GenerateAIBehaviorTree("Dragon boss with multiple phases, uses breath attacks and tail swipes, becomes more aggressive at low health");

// 3. Setup phases
BossTemplate->AddPhase("Phase1", 1.0f);
BossTemplate->AddPhase("Phase2", 0.5f);
BossTemplate->AddPhaseTransition("Phase1", "Phase2", 0.5f);
```

### 2. Creating Enemy Variants
```cpp
// 1. Create base template
UEnemyTemplate* BaseArcher = CreateNewEnemyTemplate("BaseArcher", EEnemyType::Standard);

// 2. Create variants
UEnemyTemplate* EliteArcher = CreateVariant(BaseArcher, "EliteArcher");
EliteArcher->ModifyStats(1.5f); // 50% stronger

UEnemyTemplate* StealthArcher = CreateVariant(BaseArcher, "StealthArcher");
StealthArcher->AddAbility("Stealth");
```

## Credits
The Enemy Creator Tool is part of the Enemy Pattern Library, designed to provide a robust foundation for enemy creation in AAA game development using Unreal Engine 5. It integrates with AI coding assistants like Cursor for intelligent AI generation and assistance. 