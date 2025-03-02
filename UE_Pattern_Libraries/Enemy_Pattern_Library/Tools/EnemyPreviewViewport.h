#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"
#include "EnemyPreviewViewport.generated.h"

class AEnemyPreviewActor;

/**
 * Custom viewport for previewing enemy characters
 */
UCLASS()
class GAME_API UEnemyPreviewViewport : public UEditorViewportClient
{
    GENERATED_BODY()

public:
    UEnemyPreviewViewport();

    // Preview actor management
    void SetPreviewActor(AEnemyPreviewActor* InPreviewActor);
    AEnemyPreviewActor* GetPreviewActor() const { return PreviewActor; }

    // Viewport controls
    void ToggleDebugDisplay();
    void SetViewLocation(const FVector& NewLocation);
    void SetViewRotation(const FRotator& NewRotation);

    // Animation preview
    void PlayAnimation(UAnimSequence* Animation);
    void StopAnimation();

    // Debug visualization
    void ShowAIDebugInfo(bool bShow);
    void ShowCombatRadius(bool bShow);
    void ShowAbilityRanges(bool bShow);

protected:
    // Preview scene
    UPROPERTY()
    class FPreviewScene* PreviewScene;

    // Preview actor
    UPROPERTY()
    AEnemyPreviewActor* PreviewActor;

    // Camera controls
    UPROPERTY()
    class UCameraComponent* PreviewCamera;

    // Debug settings
    UPROPERTY()
    bool bShowDebugDisplay;

    UPROPERTY()
    bool bShowAIDebug;

    UPROPERTY()
    bool bShowCombatRadius;

    UPROPERTY()
    bool bShowAbilityRanges;

    // Viewport overrides
    virtual void Tick(float DeltaTime) override;
    virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;

private:
    void SetupPreviewScene();
    void UpdateDebugVisuals();
}; 