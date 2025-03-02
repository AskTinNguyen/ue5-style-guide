#pragma once

#include "CoreMinimal.h"
#include "EnemyCreatorTypes.h"
#include "EnemyTemplate.h"
#include "EnemyCreatorTool.generated.h"

UCLASS(BlueprintType)
class ENEMYCREATOR_API UEnemyCreatorTool : public UObject
{
    GENERATED_BODY()
    
public:
    UEnemyCreatorTool();
    
    //~ Begin Template Management
    /** Create a new enemy template */
    UFUNCTION(BlueprintCallable, Category = "Enemy Creation")
    UEnemyTemplate* CreateNewEnemyTemplate(const FString& TemplateName, EEnemyType EnemyType);
    
    /** Create a configuration from template */
    UFUNCTION(BlueprintCallable, Category = "Enemy Creation")
    UEnemyConfiguration* CreateEnemyConfiguration(UEnemyTemplate* BaseTemplate);
    
    /** Update preview with configuration */
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void UpdatePreview(UEnemyConfiguration* Config);
    //~ End Template Management
    
    //~ Begin AI Features
    /** Simulate AI behavior in preview */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SimulateAIBehavior();
    
    /** Generate AI behavior tree from description */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void GenerateAIBehaviorTree(const FString& BehaviorDescription);
    
    /** Get ability suggestions */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SuggestAbilities(const FString& EnemyDescription);
    //~ End AI Features
    
protected:
    //~ Begin UI Components
    /** Initialize preview viewport */
    void InitializePreviewViewport();
    
    /** Setup property customization */
    void SetupPropertyCustomization();
    
    /** Register AI callbacks */
    void RegisterAICallbacks();
    //~ End UI Components
    
    //~ Begin Callbacks
    /** Called when behavior tree is generated */
    UFUNCTION()
    void OnBehaviorTreeGenerated(UBehaviorTree* GeneratedBT);
    
    /** Called when abilities are suggested */
    UFUNCTION()
    void OnAbilitiesSuggested(const TArray<FEnemyAbilityDefinition>& Abilities);
    //~ End Callbacks
    
protected:
    /** Preview actor instance */
    UPROPERTY()
    AEnemyPreviewActor* PreviewActor;
    
    /** Preview viewport widget */
    UPROPERTY()
    class UEnemyPreviewViewport* PreviewViewport;
    
    /** Property customization widget */
    UPROPERTY()
    class UEnemyPropertyCustomization* PropertyCustomization;
}; 