#include "EnemyCreatorTool.h"
#include "EnemyPreviewViewport.h"
#include "EnemyPropertyCustomization.h"
#include "OpenAIInterface.h"

UEnemyCreatorTool::UEnemyCreatorTool()
{
    // Initialize components
    InitializePreviewViewport();
    SetupPropertyCustomization();
    RegisterAICallbacks();
}

UEnemyTemplate* UEnemyCreatorTool::CreateNewEnemyTemplate(const FString& TemplateName, EEnemyType EnemyType)
{
    // Create new template asset
    UEnemyTemplate* NewTemplate = NewObject<UEnemyTemplate>();
    
    // Setup basic template properties
    NewTemplate->TemplateName = FName(*TemplateName);
    NewTemplate->DisplayName = FText::FromString(TemplateName);
    
    // Initialize with default values based on enemy type
    InitializeTemplateDefaults(NewTemplate, EnemyType);
    
    // Validate template
    FEnemyTemplateValidationResult ValidationResult;
    if (!NewTemplate->ValidateTemplate(ValidationResult))
    {
        for (const FText& Error : ValidationResult.ValidationErrors)
        {
            UE_LOG(LogEnemyEditor, Warning, TEXT("Template validation error: %s"), *Error.ToString());
        }
        return nullptr;
    }
    
    return NewTemplate;
}

UEnemyConfiguration* UEnemyCreatorTool::CreateEnemyConfiguration(UEnemyTemplate* BaseTemplate)
{
    if (!BaseTemplate)
    {
        return nullptr;
    }
    
    // Create new configuration
    UEnemyConfiguration* NewConfig = NewObject<UEnemyConfiguration>();
    NewConfig->BaseTemplate = BaseTemplate;
    
    // Initialize with template defaults
    NewConfig->InitializeFromTemplate(BaseTemplate);
    
    // Update preview
    UpdatePreview(NewConfig);
    
    return NewConfig;
}

void UEnemyCreatorTool::UpdatePreview(UEnemyConfiguration* Config)
{
    if (!PreviewActor || !Config)
    {
        return;
    }
    
    // Apply configuration to preview actor
    Config->ApplyConfiguration(PreviewActor);
    
    // Update viewport
    if (PreviewViewport)
    {
        PreviewViewport->RefreshViewport();
    }
}

void UEnemyCreatorTool::SimulateAIBehavior()
{
    if (!PreviewActor)
    {
        return;
    }
    
    // Get AI controller
    if (AAIController* AIController = Cast<AAIController>(PreviewActor->GetController()))
    {
        // Run behavior tree in simulation mode
        if (UBehaviorTree* BehaviorTree = PreviewActor->GetBehaviorTree())
        {
            AIController->RunBehaviorTree(BehaviorTree);
        }
    }
}

void UEnemyCreatorTool::GenerateAIBehaviorTree(const FString& BehaviorDescription)
{
    // Use OpenAI to generate behavior tree structure
    if (UOpenAIInterface* AIAssistant = UOpenAIInterface::Get())
    {
        AIAssistant->GenerateBehaviorTree(BehaviorDescription);
    }
}

void UEnemyCreatorTool::SuggestAbilities(const FString& EnemyDescription)
{
    // Use OpenAI to suggest abilities based on description
    if (UOpenAIInterface* AIAssistant = UOpenAIInterface::Get())
    {
        AIAssistant->GenerateAbilitySuggestions(EnemyDescription);
    }
}

void UEnemyCreatorTool::OnBehaviorTreeGenerated(UBehaviorTree* GeneratedBT)
{
    if (GeneratedBT && PreviewActor)
    {
        PreviewActor->SetBehaviorTree(GeneratedBT);
        SimulateAIBehavior();
    }
}

void UEnemyCreatorTool::OnAbilitiesSuggested(const TArray<FEnemyAbilityDefinition>& Abilities)
{
    if (PropertyCustomization)
    {
        PropertyCustomization->UpdateAbilitySuggestions(Abilities);
    }
}

void UEnemyCreatorTool::InitializePreviewViewport()
{
    if (!PreviewViewport)
    {
        PreviewViewport = CreateWidget<UEnemyPreviewViewport>(this);
        PreviewViewport->SetPreviewActor(PreviewActor);
    }
}

void UEnemyCreatorTool::SetupPropertyCustomization()
{
    if (!PropertyCustomization)
    {
        PropertyCustomization = CreateWidget<UEnemyPropertyCustomization>(this);
        PropertyCustomization->OnPropertyChanged.AddDynamic(this, &UEnemyCreatorTool::UpdatePreview);
    }
}

void UEnemyCreatorTool::RegisterAICallbacks()
{
    // Register callbacks for AI assistance features
    if (UOpenAIInterface* AIAssistant = UOpenAIInterface::Get())
    {
        AIAssistant->OnBehaviorTreeGenerated.AddDynamic(this, &UEnemyCreatorTool::OnBehaviorTreeGenerated);
        AIAssistant->OnAbilitiesSuggested.AddDynamic(this, &UEnemyCreatorTool::OnAbilitiesSuggested);
    }
}

void UEnemyCreatorTool::InitializeTemplateDefaults(UEnemyTemplate* Template, EEnemyType EnemyType)
{
    if (!Template)
    {
        return;
    }
    
    // Set base stats based on enemy type
    FEnemyBaseStats& BaseStats = Template->GetBaseStats();
    switch (EnemyType)
    {
        case EEnemyType::Melee:
            BaseStats.Health = 100.0f;
            BaseStats.Damage = 25.0f;
            BaseStats.Speed = 400.0f;
            BaseStats.AttackSpeed = 1.0f;
            BaseStats.Defense = 15.0f;
            BaseStats.CriticalChance = 0.05f;
            BaseStats.CriticalMultiplier = 2.0f;
            break;
            
        case EEnemyType::Ranged:
            BaseStats.Health = 80.0f;
            BaseStats.Damage = 20.0f;
            BaseStats.Speed = 350.0f;
            BaseStats.AttackSpeed = 0.8f;
            BaseStats.Defense = 10.0f;
            BaseStats.CriticalChance = 0.1f;
            BaseStats.CriticalMultiplier = 2.5f;
            break;
            
        case EEnemyType::Support:
            BaseStats.Health = 90.0f;
            BaseStats.Damage = 15.0f;
            BaseStats.Speed = 375.0f;
            BaseStats.AttackSpeed = 0.9f;
            BaseStats.Defense = 12.0f;
            BaseStats.CriticalChance = 0.03f;
            BaseStats.CriticalMultiplier = 1.8f;
            break;
            
        case EEnemyType::Elite:
            BaseStats.Health = 200.0f;
            BaseStats.Damage = 35.0f;
            BaseStats.Speed = 425.0f;
            BaseStats.AttackSpeed = 1.2f;
            BaseStats.Defense = 25.0f;
            BaseStats.CriticalChance = 0.15f;
            BaseStats.CriticalMultiplier = 2.8f;
            break;
            
        case EEnemyType::Boss:
            BaseStats.Health = 500.0f;
            BaseStats.Damage = 50.0f;
            BaseStats.Speed = 350.0f;
            BaseStats.AttackSpeed = 0.7f;
            BaseStats.Defense = 40.0f;
            BaseStats.CriticalChance = 0.2f;
            BaseStats.CriticalMultiplier = 3.0f;
            break;
    }
    
    // Set AI configuration defaults
    FEnemyAIConfig& AIConfig = Template->GetAIConfig();
    switch (EnemyType)
    {
        case EEnemyType::Melee:
            AIConfig.AggressionLevel = 0.8f;
            AIConfig.PreferredRange = 200.0f;
            AIConfig.bUseCover = false;
            AIConfig.bCoordinateWithAllies = true;
            break;
            
        case EEnemyType::Ranged:
            AIConfig.AggressionLevel = 0.4f;
            AIConfig.PreferredRange = 800.0f;
            AIConfig.bUseCover = true;
            AIConfig.bCoordinateWithAllies = true;
            break;
            
        case EEnemyType::Support:
            AIConfig.AggressionLevel = 0.2f;
            AIConfig.PreferredRange = 600.0f;
            AIConfig.bUseCover = true;
            AIConfig.bCoordinateWithAllies = true;
            break;
            
        case EEnemyType::Elite:
            AIConfig.AggressionLevel = 0.7f;
            AIConfig.PreferredRange = 400.0f;
            AIConfig.bUseCover = true;
            AIConfig.bCoordinateWithAllies = true;
            break;
            
        case EEnemyType::Boss:
            AIConfig.AggressionLevel = 0.9f;
            AIConfig.PreferredRange = 300.0f;
            AIConfig.bUseCover = false;
            AIConfig.bCoordinateWithAllies = false;
            break;
    }
    
    // Add type-specific gameplay tags
    FGameplayTagContainer& Tags = Template->GetTemplateTags();
    Tags.AddTag(FGameplayTag::RequestGameplayTag(FName(*FString::Printf(TEXT("Enemy.Type.%s"), *UEnum::GetValueAsString(EnemyType)))));
    
    // Initialize visual customization with defaults
    FEnemyVisualCustomization& Visuals = Template->GetVisualCustomization();
    switch (EnemyType)
    {
        case EEnemyType::Boss:
            Visuals.Scale = FVector(2.0f);
            break;
            
        case EEnemyType::Elite:
            Visuals.Scale = FVector(1.5f);
            break;
            
        default:
            Visuals.Scale = FVector(1.0f);
            break;
    }
    
    // Load default assets based on type
    LoadDefaultAssets(Template, EnemyType);
}

void UEnemyCreatorTool::LoadDefaultAssets(UEnemyTemplate* Template, EEnemyType EnemyType)
{
    if (!Template)
    {
        return;
    }
    
    // Construct paths based on enemy type
    FString TypeString = UEnum::GetValueAsString(EnemyType).RightChop(11); // Remove "EEnemyType::"
    FString BasePath = FString::Printf(TEXT("/Game/Enemies/%s/"), *TypeString);
    
    // Load default mesh
    FString MeshPath = BasePath + TEXT("SK_") + TypeString;
    if (USkeletalMesh* DefaultMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *MeshPath)))
    {
        Template->GetVisualCustomization().SkeletalMesh = DefaultMesh;
    }
    
    // Load default animation blueprint
    FString AnimBPPath = BasePath + TEXT("ABP_") + TypeString;
    if (UAnimBlueprint* DefaultAnimBP = Cast<UAnimBlueprint>(StaticLoadObject(UAnimBlueprint::StaticClass(), nullptr, *AnimBPPath)))
    {
        Template->GetVisualCustomization().AnimationBlueprint = DefaultAnimBP;
    }
    
    // Load default behavior tree
    FString BTPath = BasePath + TEXT("BT_") + TypeString;
    if (UBehaviorTree* DefaultBT = Cast<UBehaviorTree>(StaticLoadObject(UBehaviorTree::StaticClass(), nullptr, *BTPath)))
    {
        Template->GetAIConfig().BehaviorTree = DefaultBT;
    }
    
    // Load default abilities
    LoadDefaultAbilities(Template, EnemyType);
}

void UEnemyCreatorTool::LoadDefaultAbilities(UEnemyTemplate* Template, EEnemyType EnemyType)
{
    if (!Template)
    {
        return;
    }
    
    TArray<FEnemyAbilityDefinition>& Abilities = Template->GetAbilities();
    Abilities.Empty();
    
    // Add common abilities
    FEnemyAbilityDefinition BasicAttack;
    BasicAttack.AbilityName = TEXT("BasicAttack");
    BasicAttack.DisplayName = NSLOCTEXT("EnemyAbilities", "BasicAttack", "Basic Attack");
    BasicAttack.Description = NSLOCTEXT("EnemyAbilities", "BasicAttackDesc", "Basic melee or ranged attack");
    BasicAttack.CooldownTime = 1.0f;
    BasicAttack.Range = (EnemyType == EEnemyType::Ranged) ? 800.0f : 200.0f;
    Abilities.Add(BasicAttack);
    
    // Add type-specific abilities
    switch (EnemyType)
    {
        case EEnemyType::Melee:
            {
                FEnemyAbilityDefinition Charge;
                Charge.AbilityName = TEXT("Charge");
                Charge.DisplayName = NSLOCTEXT("EnemyAbilities", "Charge", "Charge Attack");
                Charge.Description = NSLOCTEXT("EnemyAbilities", "ChargeDesc", "Charge towards target and deal damage");
                Charge.CooldownTime = 8.0f;
                Charge.Range = 600.0f;
                Abilities.Add(Charge);
                
                FEnemyAbilityDefinition Cleave;
                Cleave.AbilityName = TEXT("Cleave");
                Cleave.DisplayName = NSLOCTEXT("EnemyAbilities", "Cleave", "Cleaving Strike");
                Cleave.Description = NSLOCTEXT("EnemyAbilities", "CleaveDesc", "Wide sweeping attack that hits multiple targets");
                Cleave.CooldownTime = 5.0f;
                Cleave.Range = 250.0f;
                Abilities.Add(Cleave);
            }
            break;
            
        case EEnemyType::Ranged:
            {
                FEnemyAbilityDefinition PowerShot;
                PowerShot.AbilityName = TEXT("PowerShot");
                PowerShot.DisplayName = NSLOCTEXT("EnemyAbilities", "PowerShot", "Power Shot");
                PowerShot.Description = NSLOCTEXT("EnemyAbilities", "PowerShotDesc", "Charged shot that deals high damage");
                PowerShot.CooldownTime = 10.0f;
                PowerShot.Range = 1000.0f;
                Abilities.Add(PowerShot);
                
                FEnemyAbilityDefinition Volley;
                Volley.AbilityName = TEXT("Volley");
                Volley.DisplayName = NSLOCTEXT("EnemyAbilities", "Volley", "Arrow Volley");
                Volley.Description = NSLOCTEXT("EnemyAbilities", "VolleyDesc", "Fire multiple projectiles in an area");
                Volley.CooldownTime = 15.0f;
                Volley.Range = 800.0f;
                Abilities.Add(Volley);
            }
            break;
            
        case EEnemyType::Support:
            {
                FEnemyAbilityDefinition Heal;
                Heal.AbilityName = TEXT("Heal");
                Heal.DisplayName = NSLOCTEXT("EnemyAbilities", "Heal", "Healing Pulse");
                Heal.Description = NSLOCTEXT("EnemyAbilities", "HealDesc", "Heal nearby allies");
                Heal.CooldownTime = 12.0f;
                Heal.Range = 500.0f;
                Abilities.Add(Heal);
                
                FEnemyAbilityDefinition Buff;
                Buff.AbilityName = TEXT("Buff");
                Buff.DisplayName = NSLOCTEXT("EnemyAbilities", "Buff", "Battle Cry");
                Buff.Description = NSLOCTEXT("EnemyAbilities", "BuffDesc", "Increase damage of nearby allies");
                Buff.CooldownTime = 20.0f;
                Buff.Range = 600.0f;
                Abilities.Add(Buff);
            }
            break;
            
        case EEnemyType::Elite:
            {
                FEnemyAbilityDefinition Ultimate;
                Ultimate.AbilityName = TEXT("Ultimate");
                Ultimate.DisplayName = NSLOCTEXT("EnemyAbilities", "Ultimate", "Elite Power");
                Ultimate.Description = NSLOCTEXT("EnemyAbilities", "UltimateDesc", "Powerful ability unique to this elite enemy");
                Ultimate.CooldownTime = 30.0f;
                Ultimate.Range = 400.0f;
                Abilities.Add(Ultimate);
            }
            break;
            
        case EEnemyType::Boss:
            {
                FEnemyAbilityDefinition Phase1;
                Phase1.AbilityName = TEXT("Phase1");
                Phase1.DisplayName = NSLOCTEXT("EnemyAbilities", "Phase1", "Phase 1 Ultimate");
                Phase1.Description = NSLOCTEXT("EnemyAbilities", "Phase1Desc", "First phase special ability");
                Phase1.CooldownTime = 45.0f;
                Phase1.Range = 1000.0f;
                Abilities.Add(Phase1);
                
                FEnemyAbilityDefinition Phase2;
                Phase2.AbilityName = TEXT("Phase2");
                Phase2.DisplayName = NSLOCTEXT("EnemyAbilities", "Phase2", "Phase 2 Ultimate");
                Phase2.Description = NSLOCTEXT("EnemyAbilities", "Phase2Desc", "Second phase special ability");
                Phase2.CooldownTime = 60.0f;
                Phase2.Range = 1000.0f;
                Abilities.Add(Phase2);
            }
            break;
    }
    
    // Load ability assets
    for (FEnemyAbilityDefinition& Ability : Abilities)
    {
        FString AbilityPath = FString::Printf(TEXT("/Game/Enemies/%s/Abilities/GA_%s_%s"), 
            *UEnum::GetValueAsString(EnemyType).RightChop(11),
            *UEnum::GetValueAsString(EnemyType).RightChop(11),
            *Ability.AbilityName.ToString());
            
        // Load ability class
        if (UClass* AbilityClass = LoadClass<UGameplayAbility>(nullptr, *AbilityPath))
        {
            Ability.AbilityClass = AbilityClass;
        }
        
        // Load montage
        FString MontagePath = FString::Printf(TEXT("/Game/Enemies/%s/Animations/AM_%s_%s"),
            *UEnum::GetValueAsString(EnemyType).RightChop(11),
            *UEnum::GetValueAsString(EnemyType).RightChop(11),
            *Ability.AbilityName.ToString());
            
        if (UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *MontagePath))
        {
            Ability.AbilityMontage = Montage;
        }
    }
} 