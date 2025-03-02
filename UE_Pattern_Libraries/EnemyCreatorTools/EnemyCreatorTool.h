// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "EnemyTemplate.h"
#include "EnemyConfiguration.h"
#include "EnemyCreatorTool.generated.h"

/**
 * Enemy Creator Tool - A streamlined interface for creating and configuring enemies
 * Optimized for direct development in Cursor IDE
 */
UCLASS()
class GAME_API UEnemyCreatorTool : public UEditorUtilityWidget
{
    GENERATED_BODY()

public:
    // Core creation interface
    UFUNCTION(BlueprintCallable, Category = "Enemy Creation")
    UEnemyTemplate* CreateNewEnemyTemplate(const FString& TemplateName, EEnemyType EnemyType);
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Creation")
    UEnemyConfiguration* CreateEnemyConfiguration(UEnemyTemplate* BaseTemplate);

    // Template management
    UFUNCTION(BlueprintCallable, Category = "Enemy Templates")
    TArray<UEnemyTemplate*> GetAvailableTemplates() const;
    
    UFUNCTION(BlueprintCallable, Category = "Enemy Templates")
    bool ValidateTemplate(UEnemyTemplate* Template, FString& OutError);

    // Direct behavior tree management
    UFUNCTION(BlueprintCallable, Category = "AI")
    UBehaviorTree* CreateBehaviorTree();
    
    UFUNCTION(BlueprintCallable, Category = "AI")
    void ApplyBehaviorPattern(UBehaviorTree* BehaviorTree, EBehaviorPattern Pattern);
    
    UFUNCTION(BlueprintCallable, Category = "AI")
    bool ValidateBehaviorTree(UBehaviorTree* BehaviorTree, FString& OutError);

    // Preview system
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void UpdatePreview(UEnemyConfiguration* Config);
    
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void SimulateAIBehavior();

    // Quick presets
    UFUNCTION(BlueprintCallable, Category = "Presets")
    void ApplyBalancedPreset(UEnemyConfiguration* Config);
    
    UFUNCTION(BlueprintCallable, Category = "Presets")
    void ApplyAggressivePreset(UEnemyConfiguration* Config);
    
    UFUNCTION(BlueprintCallable, Category = "Presets")
    void ApplyDefensivePreset(UEnemyConfiguration* Config);

protected:
    // UI components
    UPROPERTY()
    class UEnemyPreviewViewport* PreviewViewport;
    
    UPROPERTY()
    class UEnemyPropertyCustomization* PropertyCustomization;
    
    // Preview actor
    UPROPERTY()
    class AEnemyPreviewActor* PreviewActor;

    // Helper functions
    void InitializePreviewViewport();
    void SetupPropertyCustomization();
    void InitializeBehaviorPatterns();
}; 