# Pattern 2: Container System

## Problem Statement

Game developers need a flexible, efficient system to store, organize, and manage collections of items within an inventory. This system must handle various container types (backpacks, chests, equipment slots), support item stacking, enforce capacity constraints, and provide a clean API for item operations.

## Solution Approach

Implement a modular container architecture that:

1. Provides a common interface for all inventory containers
2. Supports different container types with specialized behaviors
3. Handles item stacking and organization automatically
4. Enforces container-specific constraints (weight limits, slot restrictions)
5. Offers robust item manipulation operations (add, remove, move, split, merge)

## Implementation

### Core Components

#### 1. Item Instance Struct

```cpp
// ItemInstance.h
#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "ItemInstance.generated.h"

USTRUCT(BlueprintType)
struct YOURGAME_API FItemInstance
{
    GENERATED_BODY()
    
    // The item data asset defining this item
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UItemDataAsset* ItemData;
    
    // Current stack count
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 1))
    int32 StackCount;
    
    // Unique identifier for this specific item instance
    UPROPERTY(BlueprintReadOnly, Category = "Item")
    FGuid ItemGuid;
    
    // Optional item level (for leveled items)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 1))
    int32 ItemLevel;
    
    // Optional item durability (for items that can be damaged)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 0, ClampMax = 100))
    float Durability;
    
    // Optional custom data (serialized as JSON)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString CustomData;
    
    // Constructor
    FItemInstance()
        : ItemData(nullptr)
        , StackCount(1)
        , ItemGuid(FGuid::NewGuid())
        , ItemLevel(1)
        , Durability(100.0f)
    {
    }
    
    FItemInstance(UItemDataAsset* InItemData, int32 InStackCount = 1)
        : ItemData(InItemData)
        , StackCount(InStackCount)
        , ItemGuid(FGuid::NewGuid())
        , ItemLevel(1)
        , Durability(100.0f)
    {
    }
    
    // Validity check
    bool IsValid() const
    {
        return ItemData != nullptr && StackCount > 0;
    }
    
    // Equality operators
    bool operator==(const FItemInstance& Other) const
    {
        return ItemGuid == Other.ItemGuid;
    }
    
    bool operator!=(const FItemInstance& Other) const
    {
        return !(*this == Other);
    }
    
    // Get the weight of this stack
    float GetTotalWeight() const
    {
        if (!IsValid())
        {
            return 0.0f;
        }
        return ItemData->GetItemWeight() * StackCount;
    }
    
    // Get the value of this stack
    int32 GetTotalValue() const
    {
        if (!IsValid())
        {
            return 0;
        }
        return ItemData->GetItemValue() * StackCount;
    }
};
```

#### 2. Container Interface

```cpp
// InventoryContainerInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInstance.h"
#include "InventoryContainerInterface.generated.h"

// Result enum for inventory operations
UENUM(BlueprintType)
enum class EInventoryResult : uint8
{
    Success UMETA(DisplayName = "Success"),
    Failed UMETA(DisplayName = "Failed"),
    NoSpace UMETA(DisplayName = "No Space"),
    InvalidItem UMETA(DisplayName = "Invalid Item"),
    NotFound UMETA(DisplayName = "Not Found"),
    PartialSuccess UMETA(DisplayName = "Partial Success"),
    ExceedsWeightLimit UMETA(DisplayName = "Exceeds Weight Limit"),
    ItemLocked UMETA(DisplayName = "Item Locked")
};

UINTERFACE(MinimalAPI, Blueprintable)
class UInventoryContainerInterface : public UInterface
{
    GENERATED_BODY()
};

class YOURGAME_API IInventoryContainerInterface
{
    GENERATED_BODY()

public:
    // Core container operations
    virtual EInventoryResult AddItem(const FItemInstance& Item, int32& OutRemainingCount) = 0;
    virtual EInventoryResult RemoveItem(const FGuid& ItemGuid, int32 Count, FItemInstance& OutRemovedItem) = 0;
    virtual EInventoryResult GetItem(const FGuid& ItemGuid, FItemInstance& OutItem) const = 0;
    
    // Container properties
    virtual int32 GetCapacity() const = 0;
    virtual int32 GetItemCount() const = 0;
    virtual float GetTotalWeight() const = 0;
    virtual float GetWeightCapacity() const = 0;
    virtual bool HasFreeSpace() const = 0;
    
    // Item queries
    virtual bool ContainsItem(const FGuid& ItemGuid) const = 0;
    virtual bool ContainsItemOfType(UItemDataAsset* ItemData) const = 0;
    virtual int32 GetTotalItemCountOfType(UItemDataAsset* ItemData) const = 0;
    
    // Get all items
    virtual TArray<FItemInstance> GetAllItems() const = 0;
};
```

#### 3. Base Container Component

```cpp
// InventoryContainerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryContainerInterface.h"
#include "ItemInstance.h"
#include "InventoryContainerComponent.generated.h"

// Delegate for inventory changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, class UInventoryContainerComponent*, Container);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, class UInventoryContainerComponent*, Container, const FItemInstance&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, class UInventoryContainerComponent*, Container, const FItemInstance&, Item);

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UInventoryContainerComponent : public UActorComponent, public IInventoryContainerInterface
{
    GENERATED_BODY()
    
public:    
    UInventoryContainerComponent();
    
    // Container properties
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 1))
    int32 Capacity;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 0))
    float WeightCapacity;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    bool bEnforceWeightLimit;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    bool bAutoStackItems;
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnItemAdded OnItemAdded;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnItemRemoved OnItemRemoved;
    
protected:
    // The items in this container
    UPROPERTY(ReplicatedUsing = OnRep_Items)
    TArray<FItemInstance> Items;
    
    // Replication notification
    UFUNCTION()
    void OnRep_Items();
    
    // Notify inventory changed
    virtual void NotifyInventoryChanged();
    
public:
    // IInventoryContainerInterface implementation
    virtual EInventoryResult AddItem(const FItemInstance& Item, int32& OutRemainingCount) override;
    virtual EInventoryResult RemoveItem(const FGuid& ItemGuid, int32 Count, FItemInstance& OutRemovedItem) override;
    virtual EInventoryResult GetItem(const FGuid& ItemGuid, FItemInstance& OutItem) const override;
    virtual int32 GetCapacity() const override { return Capacity; }
    virtual int32 GetItemCount() const override { return Items.Num(); }
    virtual float GetTotalWeight() const override;
    virtual float GetWeightCapacity() const override { return WeightCapacity; }
    virtual bool HasFreeSpace() const override { return Items.Num() < Capacity; }
    virtual bool ContainsItem(const FGuid& ItemGuid) const override;
    virtual bool ContainsItemOfType(UItemDataAsset* ItemData) const override;
    virtual int32 GetTotalItemCountOfType(UItemDataAsset* ItemData) const override;
    virtual TArray<FItemInstance> GetAllItems() const override { return Items; }
    
    // Blueprint-callable functions
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult AddItemFromData(UItemDataAsset* ItemData, int32 Count = 1, int32& OutRemainingCount);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult RemoveItemByData(UItemDataAsset* ItemData, int32 Count = 1);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool CanAddItem(const FItemInstance& Item, int32& OutMaxAddCount) const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool CanAddItemOfType(UItemDataAsset* ItemData, int32 Count = 1) const;
    
    // Networking
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

#### 4. Specialized Container Types

```cpp
// SlottedContainerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "InventoryContainerComponent.h"
#include "SlottedContainerComponent.generated.h"

// Struct for a slotted inventory item
USTRUCT(BlueprintType)
struct FSlottedItemInstance
{
    GENERATED_BODY()
    
    // The item in this slot
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FItemInstance Item;
    
    // The slot index
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 SlotIndex;
    
    // Whether this slot is locked
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    bool bIsLocked;
    
    // Constructor
    FSlottedItemInstance()
        : SlotIndex(-1)
        , bIsLocked(false)
    {
    }
    
    FSlottedItemInstance(const FItemInstance& InItem, int32 InSlotIndex)
        : Item(InItem)
        , SlotIndex(InSlotIndex)
        , bIsLocked(false)
    {
    }
};

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class YOURGAME_API USlottedContainerComponent : public UInventoryContainerComponent
{
    GENERATED_BODY()
    
public:
    USlottedContainerComponent();
    
    // Override container operations to work with slots
    virtual EInventoryResult AddItem(const FItemInstance& Item, int32& OutRemainingCount) override;
    virtual EInventoryResult RemoveItem(const FGuid& ItemGuid, int32 Count, FItemInstance& OutRemovedItem) override;
    
    // Slot-specific operations
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult AddItemToSlot(const FItemInstance& Item, int32 SlotIndex, int32& OutRemainingCount);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult RemoveItemFromSlot(int32 SlotIndex, int32 Count, FItemInstance& OutRemovedItem);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult SwapSlots(int32 SlotIndexA, int32 SlotIndexB);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult GetItemAtSlot(int32 SlotIndex, FItemInstance& OutItem) const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool IsSlotEmpty(int32 SlotIndex) const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool IsSlotLocked(int32 SlotIndex) const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetSlotLocked(int32 SlotIndex, bool bLocked);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 FindFirstEmptySlot() const;
    
protected:
    // The slotted items
    UPROPERTY(ReplicatedUsing = OnRep_SlottedItems)
    TArray<FSlottedItemInstance> SlottedItems;
    
    // Replication notification
    UFUNCTION()
    void OnRep_SlottedItems();
    
    // Initialize slots
    virtual void InitializeSlots();
    
public:
    // Networking
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

#### 5. Equipment Container

```cpp
// EquipmentContainerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "SlottedContainerComponent.h"
#include "EquippableItemDataAsset.h"
#include "EquipmentContainerComponent.generated.h"

// Delegate for equipment changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemEquipped, class UEquipmentContainerComponent*, Container, const FItemInstance&, Item, EEquipmentSlot, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemUnequipped, class UEquipmentContainerComponent*, Container, const FItemInstance&, Item, EEquipmentSlot, Slot);

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UEquipmentContainerComponent : public USlottedContainerComponent
{
    GENERATED_BODY()
    
public:
    UEquipmentContainerComponent();
    
    // Equipment-specific delegates
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Equipment")
    FOnItemEquipped OnItemEquipped;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Equipment")
    FOnItemUnequipped OnItemUnequipped;
    
    // Equipment operations
    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    EInventoryResult EquipItem(const FItemInstance& Item, EEquipmentSlot Slot, FItemInstance& OutPreviousItem);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    EInventoryResult UnequipItem(EEquipmentSlot Slot, FItemInstance& OutUnequippedItem);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    EInventoryResult GetEquippedItem(EEquipmentSlot Slot, FItemInstance& OutItem) const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    bool IsSlotEquipped(EEquipmentSlot Slot) const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    TMap<EEquipmentSlot, FItemInstance> GetAllEquippedItems() const;
    
protected:
    // Map equipment slots to container slots
    UPROPERTY()
    TMap<EEquipmentSlot, int32> EquipmentSlotMap;
    
    // Initialize equipment slots
    virtual void InitializeSlots() override;
    
    // Validate equipment compatibility
    bool CanEquipItemInSlot(const FItemInstance& Item, EEquipmentSlot Slot) const;
};
```

## Usage Examples

### Basic Inventory Container

```cpp
// Create a basic inventory container
UInventoryContainerComponent* Inventory = CreateDefaultSubobject<UInventoryContainerComponent>(TEXT("Inventory"));
Inventory->Capacity = 20;
Inventory->WeightCapacity = 100.0f;
Inventory->bEnforceWeightLimit = true;
Inventory->bAutoStackItems = true;

// Add an item to the inventory
UItemManagerSubsystem* ItemManager = GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
UItemDataAsset* HealthPotion = ItemManager->GetItemDataByName(FName("Consumable"), FName("HealthPotion"));

FItemInstance PotionInstance(HealthPotion, 5); // Create 5 health potions
int32 RemainingCount = 0;
EInventoryResult Result = Inventory->AddItem(PotionInstance, RemainingCount);

if (Result == EInventoryResult::Success)
{
    UE_LOG(LogInventory, Display, TEXT("Added potions successfully!"));
}
else if (Result == EInventoryResult::PartialSuccess)
{
    UE_LOG(LogInventory, Warning, TEXT("Added some potions, but %d couldn't fit"), RemainingCount);
}
```

### Equipment Container

```cpp
// Create an equipment container
UEquipmentContainerComponent* Equipment = CreateDefaultSubobject<UEquipmentContainerComponent>(TEXT("Equipment"));

// Equip an item
UItemManagerSubsystem* ItemManager = GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
UEquippableItemDataAsset* Sword = Cast<UEquippableItemDataAsset>(
    ItemManager->GetItemDataByName(FName("Weapon"), FName("IronSword")));

if (Sword)
{
    FItemInstance SwordInstance(Sword);
    FItemInstance PreviousItem;
    EInventoryResult Result = Equipment->EquipItem(SwordInstance, EEquipmentSlot::Weapon, PreviousItem);
    
    if (Result == EInventoryResult::Success)
    {
        UE_LOG(LogInventory, Display, TEXT("Equipped sword successfully!"));
        
        // If there was a previous item in that slot, do something with it
        if (PreviousItem.IsValid())
        {
            UE_LOG(LogInventory, Display, TEXT("Replaced previous weapon: %s"), 
                *PreviousItem.ItemData->GetItemName().ToString());
        }
    }
}
```

### Moving Items Between Containers

```cpp
// Function to move an item between containers
EInventoryResult MoveItemBetweenContainers(
    IInventoryContainerInterface* SourceContainer,
    IInventoryContainerInterface* DestContainer,
    const FGuid& ItemGuid,
    int32 Count)
{
    // Get the item from the source container
    FItemInstance Item;
    EInventoryResult GetResult = SourceContainer->GetItem(ItemGuid, Item);
    
    if (GetResult != EInventoryResult::Success)
    {
        return GetResult;
    }
    
    // Adjust count to not exceed what's available
    Count = FMath::Min(Count, Item.StackCount);
    
    // Create a copy of the item with the requested count
    FItemInstance ItemToMove = Item;
    ItemToMove.StackCount = Count;
    
    // Try to add to destination container
    int32 RemainingCount = 0;
    EInventoryResult AddResult = DestContainer->AddItem(ItemToMove, RemainingCount);
    
    if (AddResult == EInventoryResult::Failed)
    {
        return AddResult;
    }
    
    // Calculate how many were actually added
    int32 AddedCount = Count - RemainingCount;
    
    if (AddedCount <= 0)
    {
        return EInventoryResult::Failed;
    }
    
    // Remove the successfully added items from source
    FItemInstance RemovedItem;
    EInventoryResult RemoveResult = SourceContainer->RemoveItem(ItemGuid, AddedCount, RemovedItem);
    
    // Return appropriate result
    if (RemainingCount > 0)
    {
        return EInventoryResult::PartialSuccess;
    }
    
    return EInventoryResult::Success;
}
```

## AI Prompt Templates

### Creating Specialized Container Types

```
Create a specialized inventory container component for [CONTAINER_TYPE] in Unreal Engine 5 C++.
It should extend UInventoryContainerComponent and include functionality for:
- [SPECIFIC_FUNCTIONALITY_1]
- [SPECIFIC_FUNCTIONALITY_2]
Include any necessary structs, helper functions, and override appropriate interface methods.
```

### Implementing Container Constraints

```
Implement a constraint system for inventory containers in Unreal Engine 5 C++ that:
- Enforces [CONSTRAINT_TYPE] (e.g., item category restrictions, level requirements)
- Provides validation methods for checking constraints
- Integrates with the existing inventory container interface
```

## Integration Tips

1. **Networking**: Ensure proper replication of inventory data for multiplayer games.

2. **Save/Load**: Implement serialization for inventory containers to support game saving.

3. **UI Integration**: Connect container components to UI widgets for inventory displays.

4. **Event System**: Use delegates to notify other systems of inventory changes.

5. **Validation**: Always validate operations to prevent duplication or loss of items.

## Performance Considerations

1. **Container Size**: Be mindful of container capacity in multiplayer games to limit replication costs.

2. **Item Lookup**: Use efficient data structures for quick item lookups in large inventories.

3. **Batch Operations**: Implement batch operations for moving multiple items at once.

4. **Replication Optimization**: Consider using replication conditions to only replicate when necessary.

## Common Pitfalls

1. **Race Conditions**: Be careful with concurrent inventory operations in multiplayer.

2. **Item Duplication**: Validate all operations to prevent item duplication exploits.

3. **UI Synchronization**: Ensure UI stays in sync with the actual inventory state.

4. **Complex Constraints**: Don't over-complicate container constraints, which can lead to bugs.

5. **Circular Dependencies**: Avoid circular references between inventory systems. 