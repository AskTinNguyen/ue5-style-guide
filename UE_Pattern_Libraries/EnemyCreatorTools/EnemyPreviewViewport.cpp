#include "EnemyPreviewViewport.h"
#include "EnemyPreviewActor.h"
#include "PreviewScene.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "DrawDebugHelpers.h"

UEnemyPreviewViewport::UEnemyPreviewViewport()
{
    PreviewScene = new FPreviewScene();
    
    // Setup default camera position
    SetViewLocation(FVector(-300.0f, 0.0f, 200.0f));
    SetViewRotation(FRotator(-20.0f, 0.0f, 0.0f));
    
    // Create preview camera
    PreviewCamera = NewObject<UCameraComponent>();
    PreviewScene->AddComponent(PreviewCamera, FTransform::Identity);
    
    // Setup viewport defaults
    bShowDebugDisplay = false;
    bShowAIDebug = false;
    bShowCombatRadius = false;
    bShowAbilityRanges = false;
}

void UEnemyPreviewViewport::SetPreviewActor(AEnemyPreviewActor* InPreviewActor)
{
    // Remove existing preview actor
    if (PreviewActor)
    {
        PreviewScene->RemoveComponent(PreviewActor->GetRootComponent());
    }
    
    PreviewActor = InPreviewActor;
    
    if (PreviewActor)
    {
        // Add new preview actor to scene
        PreviewScene->AddComponent(PreviewActor->GetRootComponent(), FTransform::Identity);
        
        // Reset view to focus on actor
        FBoxSphereBounds Bounds = PreviewActor->GetRootComponent()->Bounds;
        SetViewLocation(Bounds.Origin - FVector(-300.0f, 0.0f, 200.0f));
        
        // Update debug display
        UpdateDebugVisuals();
    }
}

void UEnemyPreviewViewport::PlayAnimation(UAnimSequence* Animation)
{
    if (PreviewActor)
    {
        if (USkeletalMeshComponent* MeshComp = PreviewActor->GetMesh())
        {
            MeshComp->PlayAnimation(Animation, false);
        }
    }
}

void UEnemyPreviewViewport::StopAnimation()
{
    if (PreviewActor)
    {
        if (USkeletalMeshComponent* MeshComp = PreviewActor->GetMesh())
        {
            MeshComp->Stop();
        }
    }
}

void UEnemyPreviewViewport::ShowAIDebugInfo(bool bShow)
{
    bShowAIDebug = bShow;
    UpdateDebugVisuals();
}

void UEnemyPreviewViewport::ShowCombatRadius(bool bShow)
{
    bShowCombatRadius = bShow;
    UpdateDebugVisuals();
}

void UEnemyPreviewViewport::ShowAbilityRanges(bool bShow)
{
    bShowAbilityRanges = bShow;
    UpdateDebugVisuals();
}

void UEnemyPreviewViewport::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (PreviewScene)
    {
        PreviewScene->Tick(DeltaTime);
    }
    
    // Update debug visuals if enabled
    if (bShowDebugDisplay)
    {
        UpdateDebugVisuals();
    }
}

void UEnemyPreviewViewport::Draw(FViewport* Viewport, FCanvas* Canvas)
{
    Super::Draw(Viewport, Canvas);
    
    if (!PreviewActor || !bShowDebugDisplay)
    {
        return;
    }
    
    // Draw AI debug info
    if (bShowAIDebug)
    {
        // Draw behavior tree state
        FString AIInfo = PreviewActor->GetAIDebugString();
        Canvas->DrawShadowedString(10, 10, *AIInfo, GEngine->GetSmallFont(), FLinearColor::White);
    }
    
    // Draw combat radius
    if (bShowCombatRadius)
    {
        float CombatRadius = PreviewActor->GetCombatRadius();
        DrawDebugCylinder(
            PreviewScene->GetWorld(),
            PreviewActor->GetActorLocation(),
            PreviewActor->GetActorLocation() + FVector(0, 0, 10),
            CombatRadius,
            32,
            FColor::Red,
            false,
            -1.0f,
            0,
            2.0f
        );
    }
    
    // Draw ability ranges
    if (bShowAbilityRanges)
    {
        TArray<float> AbilityRanges = PreviewActor->GetAbilityRanges();
        for (float Range : AbilityRanges)
        {
            DrawDebugCylinder(
                PreviewScene->GetWorld(),
                PreviewActor->GetActorLocation(),
                PreviewActor->GetActorLocation() + FVector(0, 0, 10),
                Range,
                32,
                FColor::Blue,
                false,
                -1.0f,
                0,
                1.0f
            );
        }
    }
}

void UEnemyPreviewViewport::SetupPreviewScene()
{
    if (!PreviewScene)
    {
        return;
    }
    
    // Add ground plane
    FPreviewSceneFloor* PreviewFloor = new FPreviewSceneFloor(PreviewScene);
    PreviewScene->AddComponent(PreviewFloor->GetComponent(), FTransform::Identity);
    
    // Setup lighting
    PreviewScene->SetLightDirection(FRotator(-45.0f, -45.0f, 0.0f));
    PreviewScene->SetLightBrightness(8.0f);
}

void UEnemyPreviewViewport::UpdateDebugVisuals()
{
    if (!PreviewActor)
    {
        return;
    }
    
    // Clear existing debug visuals
    FlushPersistentDebugLines(PreviewScene->GetWorld());
    
    // Update based on current debug settings
    if (bShowDebugDisplay)
    {
        if (bShowAIDebug)
        {
            PreviewActor->EnableAIDebugging();
        }
        
        if (bShowCombatRadius || bShowAbilityRanges)
        {
            PreviewActor->EnableCombatDebugging();
        }
    }
} 