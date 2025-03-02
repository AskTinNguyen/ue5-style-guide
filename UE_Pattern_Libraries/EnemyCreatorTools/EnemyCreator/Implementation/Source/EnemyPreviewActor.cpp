#include "EnemyCreatorTypes.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"

AEnemyPreviewActor::AEnemyPreviewActor()
{
    // Set defaults
    bReplicates = false;
    bIsEditorOnlyActor = true;
    
    // Create AI controller
    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyPreviewActor::SetBehaviorTree(UBehaviorTree* NewBT)
{
    if (NewBT != CurrentBehaviorTree)
    {
        CurrentBehaviorTree = NewBT;
        
        // Update AI controller if it exists
        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            if (CurrentBehaviorTree)
            {
                AIController->RunBehaviorTree(CurrentBehaviorTree);
            }
            else
            {
                AIController->StopTree();
            }
        }
    }
}

UBehaviorTree* AEnemyPreviewActor::GetBehaviorTree() const
{
    return CurrentBehaviorTree;
}

void AEnemyPreviewActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    // Setup for preview
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        // Enable physics simulation for preview
        MeshComp->SetSimulatePhysics(true);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
}

void AEnemyPreviewActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Start with current behavior tree if one is set
    if (CurrentBehaviorTree)
    {
        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            AIController->RunBehaviorTree(CurrentBehaviorTree);
        }
    }
} 