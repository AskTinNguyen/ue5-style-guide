# Pattern 4: Character Customization

## Problem Definition

Character customization systems in modern games need to handle:
- Dynamic appearance modifications at runtime
- Equipment visualization with proper attachment
- Modular character parts (head, body, limbs, etc.)
- Material parameter adjustments (colors, textures, etc.)
- Performance optimization for multiple customized characters
- Persistence of customization choices
- Network synchronization of appearance
- LOD support for customized characters

Traditional approaches often lead to performance issues, visual artifacts, or limited customization options.

## Solution Approach

Implement a flexible, performance-conscious customization system that:
1. Uses a modular component-based approach for character parts
2. Provides a clean interface for equipment visualization
3. Optimizes mesh and material operations
4. Handles network replication efficiently
5. Supports runtime modifications with minimal performance impact

## Implementation

### Character Customization Component

```cpp
// CharacterCustomizationComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CharacterCustomizationComponent.generated.h"

// Forward declarations
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;

// Enum for body parts
UENUM(BlueprintType)
enum class ECharacterPart : uint8
{
    Head,
    Torso,
    Arms,
    Legs,
    Hair,
    Facial
};

// Struct for customization data
USTRUCT(BlueprintType)
struct FCharacterPartData : public FTableRowBase
{
    GENERATED_BODY()
    
    // The mesh to use for this part
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<USkeletalMesh> Mesh;
    
    // Materials to apply to the mesh
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSoftObjectPtr<UMaterialInterface>> Materials;
    
    // Optional physics asset override
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UPhysicsAsset> PhysicsAsset;
    
    // Optional animation blueprint override
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<UAnimInstance> AnimClass;
};

// Struct for color customization
USTRUCT(BlueprintType)
struct FColorCustomization
{
    GENERATED_BODY()
    
    // Parameter name in the material
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ParameterName;
    
    // Color value
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor Color;
};

// Struct for texture customization
USTRUCT(BlueprintType)
struct FTextureCustomization
{
    GENERATED_BODY()
    
    // Parameter name in the material
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ParameterName;
    
    // Texture value
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture> Texture;
};

// Struct for scalar parameter customization
USTRUCT(BlueprintType)
struct FScalarCustomization
{
    GENERATED_BODY()
    
    // Parameter name in the material
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ParameterName;
    
    // Scalar value
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Value;
};

UCLASS(ClassGroup=(Character), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UCharacterCustomizationComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UCharacterCustomizationComponent();
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    // Apply a character part
    UFUNCTION(BlueprintCallable, Category = "Character|Customization")
    bool SetCharacterPart(ECharacterPart Part, FName PartId);
    
    // Apply color customization
    UFUNCTION(BlueprintCallable, Category = "Character|Customization")
    bool SetColorParameter(ECharacterPart Part, FName ParameterName, FLinearColor Color);
    
    // Apply texture customization
    UFUNCTION(BlueprintCallable, Category = "Character|Customization")
    bool SetTextureParameter(ECharacterPart Part, FName ParameterName, UTexture* Texture);
    
    // Apply scalar customization
    UFUNCTION(BlueprintCallable, Category = "Character|Customization")
    bool SetScalarParameter(ECharacterPart Part, FName ParameterName, float Value);
    
    // Apply a complete customization preset
    UFUNCTION(BlueprintCallable, Category = "Character|Customization")
    bool ApplyCustomizationPreset(const FString& PresetName);
    
    // Save current customization as a preset
    UFUNCTION(BlueprintCallable, Category = "Character|Customization")
    bool SaveCustomizationPreset(const FString& PresetName);
    
    // Get current customization data for saving
    UFUNCTION(BlueprintCallable, Category = "Character|Customization")
    FString GetSerializedCustomizationData();
    
    // Apply customization from serialized data
    UFUNCTION(BlueprintCallable, Category = "Character|Customization")
    bool ApplySerializedCustomizationData(const FString& SerializedData);
    
protected:
    // Reference to the main skeletal mesh component
    UPROPERTY()
    USkeletalMeshComponent* CharacterMesh;
    
    // Map of part type to mesh component
    UPROPERTY()
    TMap<ECharacterPart, USkeletalMeshComponent*> PartComponents;
    
    // Map of part type to dynamic material instances
    UPROPERTY()
    TMap<ECharacterPart, TArray<UMaterialInstanceDynamic*>> PartMaterials;
    
    // Data tables for different character parts
    UPROPERTY(EditDefaultsOnly, Category = "Customization|Data")
    TMap<ECharacterPart, UDataTable*> PartDataTables;
    
    // Current customization state
    UPROPERTY(ReplicatedUsing = OnRep_CurrentCustomization)
    TMap<ECharacterPart, FName> CurrentParts;
    
    UPROPERTY(Replicated)
    TArray<FColorCustomization> ColorCustomizations;
    
    UPROPERTY(Replicated)
    TArray<FTextureCustomization> TextureCustomizations;
    
    UPROPERTY(Replicated)
    TArray<FScalarCustomization> ScalarCustomizations;
    
    // Initialize the component mesh references
    void SetupMeshComponents();
    
    // Create dynamic material instances
    void SetupMaterialInstances();
    
    // Apply all current customizations
    void ApplyAllCustomizations();
    
    // Network replication
    UFUNCTION()
    void OnRep_CurrentCustomization();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

### Equipment Visualization System

```cpp
// EquipmentVisualizationComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "EquipmentVisualizationComponent.generated.h"

// Forward declarations
class USkeletalMeshComponent;
class UStaticMeshComponent;

// Enum for equipment slots
UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    Head,
    Chest,
    Hands,
    Legs,
    Feet,
    Back,
    MainHand,
    OffHand,
    TwoHanded,
    Accessory1,
    Accessory2
};

// Struct for equipment visual data
USTRUCT(BlueprintType)
struct FEquipmentVisualData : public FTableRowBase
{
    GENERATED_BODY()
    
    // Whether this is a skeletal or static mesh
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bIsSkeletalMesh;
    
    // The skeletal mesh to use (if applicable)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsSkeletalMesh"))
    TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
    
    // The static mesh to use (if applicable)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "!bIsSkeletalMesh"))
    TSoftObjectPtr<UStaticMesh> StaticMesh;
    
    // Materials to apply to the mesh
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSoftObjectPtr<UMaterialInterface>> Materials;
    
    // Socket to attach to
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName AttachSocket;
    
    // Transform offset
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FTransform AttachTransform;
    
    // Optional animation blueprint for skeletal meshes
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsSkeletalMesh"))
    TSoftClassPtr<UAnimInstance> AnimClass;
};

UCLASS(ClassGroup=(Character), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UEquipmentVisualizationComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UEquipmentVisualizationComponent();
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    // Equip an item visually
    UFUNCTION(BlueprintCallable, Category = "Character|Equipment")
    bool EquipItem(EEquipmentSlot Slot, FName ItemId);
    
    // Unequip an item visually
    UFUNCTION(BlueprintCallable, Category = "Character|Equipment")
    bool UnequipItem(EEquipmentSlot Slot);
    
    // Hide/show equipment
    UFUNCTION(BlueprintCallable, Category = "Character|Equipment")
    void SetEquipmentVisibility(EEquipmentSlot Slot, bool bIsVisible);
    
    // Set all equipment visibility
    UFUNCTION(BlueprintCallable, Category = "Character|Equipment")
    void SetAllEquipmentVisibility(bool bIsVisible);
    
protected:
    // Reference to the character mesh
    UPROPERTY()
    USkeletalMeshComponent* CharacterMesh;
    
    // Map of equipment slot to component
    UPROPERTY()
    TMap<EEquipmentSlot, USceneComponent*> EquipmentComponents;
    
    // Data table for equipment visuals
    UPROPERTY(EditDefaultsOnly, Category = "Equipment|Data")
    UDataTable* EquipmentDataTable;
    
    // Current equipment state
    UPROPERTY(ReplicatedUsing = OnRep_CurrentEquipment)
    TMap<EEquipmentSlot, FName> CurrentEquipment;
    
    // Initialize the component references
    void SetupComponents();
    
    // Create a component for an equipment slot
    USceneComponent* CreateEquipmentComponent(EEquipmentSlot Slot, const FEquipmentVisualData& VisualData);
    
    // Apply equipment visual
    bool ApplyEquipmentVisual(EEquipmentSlot Slot, const FEquipmentVisualData& VisualData);
    
    // Network replication
    UFUNCTION()
    void OnRep_CurrentEquipment();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

## Integration Tips

1. **Performance Optimization**:
   - Use asynchronous loading for meshes and textures
   - Implement LOD systems for customized characters
   - Batch material parameter updates
   - Consider mesh merging for complex customizations

2. **Network Considerations**:
   - Minimize replication frequency of customization data
   - Use compression for serialized customization data
   - Consider relevancy-based replication for distant characters

3. **Memory Management**:
   - Implement proper resource streaming
   - Share materials between similar customizations
   - Use texture atlases for common customization options

4. **Animation Integration**:
   - Ensure proper skeleton compatibility between parts
   - Handle animation retargeting for different body types
   - Set up proper physics constraints for equipment

## Example Usage Scenarios

### Basic Character Customization

```cpp
// In your game mode or character creation UI
void UCharacterCreationWidget::ApplyCustomization()
{
    // Get the customization component
    UCharacterCustomizationComponent* CustomizationComp = Character->FindComponentByClass<UCharacterCustomizationComponent>();
    if (!CustomizationComp)
    {
        return;
    }
    
    // Apply selected parts
    CustomizationComp->SetCharacterPart(ECharacterPart::Head, SelectedHeadID);
    CustomizationComp->SetCharacterPart(ECharacterPart::Hair, SelectedHairID);
    CustomizationComp->SetCharacterPart(ECharacterPart::Torso, SelectedTorsoID);
    CustomizationComp->SetCharacterPart(ECharacterPart::Arms, SelectedArmsID);
    CustomizationComp->SetCharacterPart(ECharacterPart::Legs, SelectedLegsID);
    
    // Apply color customizations
    CustomizationComp->SetColorParameter(ECharacterPart::Hair, "HairColor", SelectedHairColor);
    CustomizationComp->SetColorParameter(ECharacterPart::Head, "SkinTone", SelectedSkinTone);
    CustomizationComp->SetColorParameter(ECharacterPart::Torso, "ClothingPrimaryColor", SelectedPrimaryColor);
    CustomizationComp->SetColorParameter(ECharacterPart::Torso, "ClothingSecondaryColor", SelectedSecondaryColor);
    
    // Apply scalar parameters
    CustomizationComp->SetScalarParameter(ECharacterPart::Head, "WrinkleIntensity", AgeSliderValue);
    CustomizationComp->SetScalarParameter(ECharacterPart::Head, "ScarIntensity", ScarSliderValue);
    
    // Save customization as preset
    CustomizationComp->SaveCustomizationPreset(PresetName);
}
```

### Equipment Visualization

```cpp
// In your inventory system
void APlayerCharacter::EquipItemFromInventory(UInventoryItem* Item)
{
    if (!Item)
    {
        return;
    }
    
    // Get the equipment visualization component
    UEquipmentVisualizationComponent* EquipmentComp = FindComponentByClass<UEquipmentVisualizationComponent>();
    if (!EquipmentComp)
    {
        return;
    }
    
    // Apply the visual based on the item data
    EEquipmentSlot Slot = Item->GetEquipmentSlot();
    FName VisualID = Item->GetVisualID();
    
    // Handle two-handed weapons
    if (Item->IsTwoHandedWeapon())
    {
        // Unequip any items in conflicting slots
        EquipmentComp->UnequipItem(EEquipmentSlot::MainHand);
        EquipmentComp->UnequipItem(EEquipmentSlot::OffHand);
        
        // Equip the two-handed weapon
        EquipmentComp->EquipItem(EEquipmentSlot::TwoHanded, VisualID);
    }
    else
    {
        // If equipping a one-handed weapon, unequip any two-handed weapon
        if (Slot == EEquipmentSlot::MainHand || Slot == EEquipmentSlot::OffHand)
        {
            EquipmentComp->UnequipItem(EEquipmentSlot::TwoHanded);
        }
        
        // Equip the item
        EquipmentComp->EquipItem(Slot, VisualID);
    }
}
```

## AI Prompt Templates

### Creating a New Character Part

```
I need to create a new modular character part for my UE5 C++ customization system. 
The part should be a [PART_TYPE] with the following requirements:
- Compatible with the existing character skeleton
- Support for [NUMBER] material slots
- Customizable [FEATURE_1] and [FEATURE_2]
- LOD support for performance optimization

My current character customization component uses the pattern from the Character Pattern Library.
How should I implement this new part and integrate it with the existing system?
```

### Optimizing Character Customization Performance

```
My UE5 game has a character customization system with many modular parts and dynamic materials.
I'm experiencing performance issues when many customized characters are on screen.

Current implementation:
- Each character has [NUMBER] modular parts
- Each part has [NUMBER] material parameters
- Characters are updated [FREQUENCY]

How can I optimize this system while maintaining visual quality and customization flexibility?
```

## Performance Considerations

1. **Mesh Loading and Management**:
   - Use asynchronous loading for meshes and textures
   - Implement proper LOD systems
   - Consider mesh merging for static parts
   - Use instanced static meshes for common equipment

2. **Material Optimization**:
   - Minimize the number of dynamic material instances
   - Batch parameter updates
   - Use material parameter collections for shared values
   - Consider material instancing hierarchy for common parameters

3. **Memory Management**:
   - Implement proper asset streaming
   - Unload unused customization assets
   - Share materials between similar customizations
   - Use texture atlases for common customization options

4. **Network Optimization**:
   - Compress customization data for network transmission
   - Consider delta compression for updates
   - Implement relevancy-based detail levels for distant characters

## Common Pitfalls and Solutions

1. **Skeletal Compatibility Issues**:
   - Ensure all modular parts use the same skeleton or compatible skeletons
   - Set up proper retargeting for animations
   - Maintain consistent bone naming conventions

2. **Material Parameter Conflicts**:
   - Use consistent parameter naming across all customizable parts
   - Document parameter usage and ranges
   - Create a parameter registry to avoid conflicts

3. **Performance Degradation**:
   - Profile and identify bottlenecks (usually material updates or mesh swapping)
   - Implement LOD systems specifically for customization
   - Consider simplified representations for distant characters

4. **Visual Artifacts**:
   - Address seams between modular parts with overlapping geometry
   - Ensure consistent texel density across parts
   - Use masked materials at boundaries between parts 