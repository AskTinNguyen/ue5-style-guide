#include "EnemyCreatorTool.h"
#include "EnemyPreviewViewport.h"
#include "EnemyPropertyCustomization.h"
#include "EnemyPreviewActor.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"

UEnemyCreatorTool::UEnemyCreatorTool()
{
    // Initialize components
    PreviewViewport = CreateDefaultSubobject<UEnemyPreviewViewport>(TEXT("PreviewViewport"));
    PropertyCustomization = CreateDefaultSubobject<UEnemyPropertyCustomization>(TEXT("PropertyCustomization"));
}

void UEnemyCreatorTool::InitializeComponent()
{
    Super::InitializeComponent();
    
    InitializePreviewViewport();
    SetupPropertyCustomization();
    InitializeBehaviorPatterns();
}

UEnemyTemplate* UEnemyCreatorTool::CreateNewEnemyTemplate(const FString& TemplateName, EEnemyType EnemyType)
{
    UEnemyTemplate* NewTemplate = NewObject<UEnemyTemplate>();
    NewTemplate->TemplateName = TemplateName;
    NewTemplate->EnemyType = EnemyType;
    
    // Initialize with default values based on type
    switch (EnemyType)
    {
        case EEnemyType::Standard:
            ApplyBalancedPreset(NewTemplate->Configuration);
            break;
        case EEnemyType::Elite:
            ApplyAggressivePreset(NewTemplate->Configuration);
            break;
        case EEnemyType::Boss:
            // Custom boss initialization
            break;
    }
    
    return NewTemplate;
}

UBehaviorTree* UEnemyCreatorTool::CreateBehaviorTree()
{
    UBehaviorTree* NewBT = NewObject<UBehaviorTree>();
    // Initialize with basic structure
    return NewBT;
}

void UEnemyCreatorTool::ApplyBehaviorPattern(UBehaviorTree* BehaviorTree, EBehaviorPattern Pattern)
{
    if (!BehaviorTree) return;
    
    switch (Pattern)
    {
        case EBehaviorPattern::Patrol:
            // Add patrol behavior nodes
            break;
        case EBehaviorPattern::Aggressive:
            // Add aggressive combat nodes
            break;
        case EBehaviorPattern::Defensive:
            // Add defensive behavior nodes
            break;
        case EBehaviorPattern::Support:
            // Add support behavior nodes
            break;
    }
}

void UEnemyCreatorTool::ApplyBalancedPreset(UEnemyConfiguration* Config)
{
    if (!Config) return;
    
    // Set balanced stats
    Config->Health = 100.0f;
    Config->Damage = 20.0f;
    Config->MovementSpeed = 300.0f;
    Config->AttackRange = 200.0f;
}

void UEnemyCreatorTool::ApplyAggressivePreset(UEnemyConfiguration* Config)
{
    if (!Config) return;
    
    // Set aggressive stats
    Config->Health = 80.0f;
    Config->Damage = 30.0f;
    Config->MovementSpeed = 350.0f;
    Config->AttackRange = 150.0f;
}

void UEnemyCreatorTool::ApplyDefensivePreset(UEnemyConfiguration* Config)
{
    if (!Config) return;
    
    // Set defensive stats
    Config->Health = 150.0f;
    Config->Damage = 15.0f;
    Config->MovementSpeed = 250.0f;
    Config->AttackRange = 250.0f;
}

void UEnemyCreatorTool::InitializeBehaviorPatterns()
{
    // Initialize common behavior patterns
    // These will be available for quick application
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
        FString BTDefinition = AIAssistant->GenerateBehaviorTree(BehaviorDescription);
        
        // Parse and create behavior tree asset
        if (UBehaviorTree* NewBT = CreateBehaviorTreeFromDefinition(BTDefinition))
        {
            // Apply to preview actor
            if (PreviewActor)
            {
                PreviewActor->SetBehaviorTree(NewBT);
                SimulateAIBehavior();
            }
        }
    }
}

void UEnemyCreatorTool::SuggestAbilities(const FString& EnemyDescription)
{
    // Use OpenAI to suggest abilities based on description
    if (UOpenAIInterface* AIAssistant = UOpenAIInterface::Get())
    {
        TArray<FEnemyAbilityData> SuggestedAbilities;
        AIAssistant->GenerateAbilitySuggestions(EnemyDescription, SuggestedAbilities);
        
        // Update property customization with suggestions
        if (PropertyCustomization)
        {
            PropertyCustomization->UpdateAbilitySuggestions(SuggestedAbilities);
        }
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