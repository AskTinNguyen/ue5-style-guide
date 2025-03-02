# Pattern 1: Item Data Management

## Problem Statement

Game developers need a flexible, maintainable system to define, store, and access item data in inventory systems. This includes handling various item types, properties, categories, and metadata while ensuring the system is extensible and designer-friendly.

## Solution Approach

Implement a data-driven architecture using UE5's data assets and interfaces to create a robust item definition system that:

1. Separates item data from behavior
2. Supports inheritance for item types
3. Provides flexible property management
4. Enables easy extension by designers and programmers

## Implementation

### Core Components

#### 1. Item Data Interface

```cpp
// ItemDataInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemDataInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UItemDataInterface : public UInterface
{
    GENERATED_BODY()
};

class YOURGAME_API IItemDataInterface
{
    GENERATED_BODY()

public:
    // Core item information
    virtual FText GetItemName() const = 0;
    virtual FText GetItemDescription() const = 0;
    virtual UTexture2D* GetItemIcon() const = 0;
    virtual int32 GetMaxStackSize() const = 0;
    virtual float GetItemWeight() const = 0;
    
    // Item categorization
    virtual FName GetItemType() const = 0;
    virtual FName GetItemCategory() const = 0;
    
    // Item value
    virtual int32 GetItemValue() const = 0;
    
    // Optional methods with default implementations
    virtual bool CanBeUsed() const { return false; }
    virtual bool CanBeEquipped() const { return false; }
    virtual bool CanBeStacked() const { return true; }
};
```

#### 2. Base Item Data Asset

```cpp
// ItemDataAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataInterface.h"
#include "ItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Common UMETA(DisplayName = "Common"),
    Uncommon UMETA(DisplayName = "Uncommon"),
    Rare UMETA(DisplayName = "Rare"),
    Epic UMETA(DisplayName = "Epic"),
    Legendary UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct FItemStatModifier
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StatName;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Value;
};

UCLASS(BlueprintType, Blueprintable)
class YOURGAME_API UItemDataAsset : public UPrimaryDataAsset, public IItemDataInterface
{
    GENERATED_BODY()
    
public:
    UItemDataAsset();
    
    // Core properties
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText ItemName;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
    FText ItemDescription;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UTexture2D* ItemIcon;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FName ItemType;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FName ItemCategory;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = 1))
    int32 MaxStackSize;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float ItemWeight;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 ItemValue;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    EItemRarity Rarity;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TArray<FItemStatModifier> StatModifiers;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UStaticMesh* WorldMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bCanBeUsed;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bCanBeEquipped;
    
    // IItemDataInterface implementation
    virtual FText GetItemName() const override { return ItemName; }
    virtual FText GetItemDescription() const override { return ItemDescription; }
    virtual UTexture2D* GetItemIcon() const override { return ItemIcon; }
    virtual int32 GetMaxStackSize() const override { return MaxStackSize; }
    virtual float GetItemWeight() const override { return ItemWeight; }
    virtual FName GetItemType() const override { return ItemType; }
    virtual FName GetItemCategory() const override { return ItemCategory; }
    virtual int32 GetItemValue() const override { return ItemValue; }
    virtual bool CanBeUsed() const override { return bCanBeUsed; }
    virtual bool CanBeEquipped() const override { return bCanBeEquipped; }
    virtual bool CanBeStacked() const override { return MaxStackSize > 1; }
    
    // Asset management
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(ItemType, GetFName());
    }
};
```

#### 3. Item Data Asset Implementation

```cpp
// ItemDataAsset.cpp
#include "ItemDataAsset.h"

UItemDataAsset::UItemDataAsset()
{
    ItemType = FName("Item");
    ItemCategory = FName("Misc");
    MaxStackSize = 99;
    ItemWeight = 1.0f;
    ItemValue = 1;
    Rarity = EItemRarity::Common;
    bCanBeUsed = false;
    bCanBeEquipped = false;
}
```

#### 4. Specialized Item Data Assets

```cpp
// EquippableItemDataAsset.h
#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "EquippableItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    None UMETA(DisplayName = "None"),
    Head UMETA(DisplayName = "Head"),
    Chest UMETA(DisplayName = "Chest"),
    Hands UMETA(DisplayName = "Hands"),
    Legs UMETA(DisplayName = "Legs"),
    Feet UMETA(DisplayName = "Feet"),
    Weapon UMETA(DisplayName = "Weapon"),
    Shield UMETA(DisplayName = "Shield"),
    Accessory UMETA(DisplayName = "Accessory")
};

UCLASS(BlueprintType)
class YOURGAME_API UEquippableItemDataAsset : public UItemDataAsset
{
    GENERATED_BODY()
    
public:
    UEquippableItemDataAsset();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    EEquipmentSlot EquipmentSlot;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    USkeletalMesh* EquipmentMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    TSubclassOf<class UGameplayEffect> EquipEffect;
    
    virtual bool CanBeEquipped() const override { return true; }
};
```

#### 5. Item Manager Subsystem

```cpp
// ItemManagerSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/AssetManager.h"
#include "ItemDataAsset.h"
#include "ItemManagerSubsystem.generated.h"

UCLASS()
class YOURGAME_API UItemManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // Initialize and shutdown
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // Item data retrieval
    UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
    UItemDataAsset* GetItemData(FPrimaryAssetId ItemId);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
    UItemDataAsset* GetItemDataByName(FName ItemType, FName ItemName);
    
    // Item listing
    UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
    TArray<UItemDataAsset*> GetAllItems();
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
    TArray<UItemDataAsset*> GetItemsByType(FName ItemType);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
    TArray<UItemDataAsset*> GetItemsByCategory(FName Category);
    
private:
    // Cache for loaded item data
    UPROPERTY()
    TMap<FPrimaryAssetId, UItemDataAsset*> LoadedItems;
    
    // Load all items of a specific type
    void LoadItemsOfType(FName ItemType);
};
```

## Usage Examples

### Creating Item Data Assets

1. In the Unreal Editor, right-click in the Content Browser
2. Select "Miscellaneous > Data Asset"
3. Choose "ItemDataAsset" or one of its child classes
4. Fill in the item properties in the editor

### Retrieving Item Data in Code

```cpp
// Get the item manager subsystem
UItemManagerSubsystem* ItemManager = GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();

// Get an item by its asset ID
FPrimaryAssetId PotionId = FPrimaryAssetId(FName("Consumable"), FName("HealthPotion"));
UItemDataAsset* HealthPotion = ItemManager->GetItemData(PotionId);

// Use the item data
if (HealthPotion)
{
    FText ItemName = HealthPotion->GetItemName();
    UTexture2D* Icon = HealthPotion->GetItemIcon();
    int32 MaxStack = HealthPotion->GetMaxStackSize();
}
```

### Retrieving Item Data in Blueprints

1. Get a reference to the Item Manager Subsystem
2. Call "Get Item Data" or "Get Item Data By Name"
3. Use the returned Item Data Asset

## AI Prompt Templates

### Creating New Item Types

```
Create a specialized item data asset class for [ITEM_TYPE] in Unreal Engine 5 C++.
It should extend UItemDataAsset and include properties for:
- [SPECIFIC_PROPERTY_1]
- [SPECIFIC_PROPERTY_2]
Include any necessary enums, structs, and override appropriate interface methods.
```

### Implementing Item Manager Extensions

```
Extend the UItemManagerSubsystem to add functionality for:
- [FUNCTIONALITY_DESCRIPTION]
Include both C++ code and Blueprint-callable functions.
```

## Integration Tips

1. **Asset Registry**: Use UE5's Asset Registry to efficiently load and manage item data assets.

2. **Blueprint Integration**: Expose key functionality to Blueprints for designer control.

3. **Data Tables**: For simpler items, consider using Data Tables alongside Data Assets.

4. **Modular Design**: Keep the item data system decoupled from inventory containers.

5. **Asset Bundles**: Use Asset Bundles for efficient loading of related items.

## Performance Considerations

1. **Async Loading**: Load item data asynchronously to avoid hitches.

2. **Caching**: Cache frequently accessed items to reduce lookup times.

3. **Memory Management**: Be mindful of keeping too many item assets loaded simultaneously.

4. **Blueprint Usage**: Minimize complex Blueprint operations on large item datasets.

## Common Pitfalls

1. **Circular Dependencies**: Avoid circular references between item types.

2. **Over-Engineering**: Don't create overly complex inheritance hierarchies.

3. **Missing Validation**: Always validate item data before use.

4. **Hardcoded References**: Avoid hardcoded references to specific items. 