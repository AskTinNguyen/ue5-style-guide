# Pattern 5: Persistence and Serialization

## Problem Statement

Game developers need a robust system to save and load inventory data across game sessions, level transitions, and save/load operations. This system must handle complex item data, maintain data integrity, support versioning for backward compatibility, and integrate with the game's overall save system.

## Solution Approach

Implement a persistence architecture that:

1. Provides serialization for all inventory-related data structures
2. Supports different storage formats (binary, JSON, etc.)
3. Handles versioning for backward compatibility
4. Validates data during loading to prevent corruption
5. Integrates with the game's save/load system
6. Optimizes storage size and loading performance

## Implementation

### Core Components

#### 1. Serializable Item Structs

```cpp
// SerializableItemTypes.h
#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "ItemInstance.h"
#include "SerializableItemTypes.generated.h"

// Serializable version of FItemInstance
USTRUCT(BlueprintType)
struct YOURGAME_API FSerializableItemInstance
{
    GENERATED_BODY()
    
    // Primary asset ID of the item data
    UPROPERTY(BlueprintReadWrite, Category = "Item")
    FPrimaryAssetId ItemDataId;
    
    // Current stack count
    UPROPERTY(BlueprintReadWrite, Category = "Item")
    int32 StackCount;
    
    // Unique identifier for this specific item instance
    UPROPERTY(BlueprintReadWrite, Category = "Item")
    FGuid ItemGuid;
    
    // Optional item level (for leveled items)
    UPROPERTY(BlueprintReadWrite, Category = "Item")
    int32 ItemLevel;
    
    // Optional item durability (for items that can be damaged)
    UPROPERTY(BlueprintReadWrite, Category = "Item")
    float Durability;
    
    // Optional custom data (serialized as JSON)
    UPROPERTY(BlueprintReadWrite, Category = "Item")
    FString CustomData;
    
    // Default constructor
    FSerializableItemInstance()
        : StackCount(1)
        , ItemGuid(FGuid::NewGuid())
        , ItemLevel(1)
        , Durability(100.0f)
    {
    }
    
    // Constructor from FItemInstance
    FSerializableItemInstance(const FItemInstance& ItemInstance)
    {
        if (ItemInstance.ItemData)
        {
            ItemDataId = ItemInstance.ItemData->GetPrimaryAssetId();
        }
        
        StackCount = ItemInstance.StackCount;
        ItemGuid = ItemInstance.ItemGuid;
        ItemLevel = ItemInstance.ItemLevel;
        Durability = ItemInstance.Durability;
        CustomData = ItemInstance.CustomData;
    }
    
    // Convert to FItemInstance
    FItemInstance ToItemInstance(UObject* WorldContextObject) const;
};

// Serializable container data
USTRUCT(BlueprintType)
struct YOURGAME_API FSerializableContainerData
{
    GENERATED_BODY()
    
    // Container items
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    TArray<FSerializableItemInstance> Items;
    
    // Container capacity
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    int32 Capacity;
    
    // Container weight capacity
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    float WeightCapacity;
    
    // Container type identifier
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    FName ContainerType;
    
    // Container owner identifier
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    FGuid OwnerId;
    
    // Container unique identifier
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    FGuid ContainerId;
    
    // Version for backward compatibility
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    int32 Version;
    
    // Default constructor
    FSerializableContainerData()
        : Capacity(0)
        , WeightCapacity(0.0f)
        , ContainerType(NAME_None)
        , ContainerId(FGuid::NewGuid())
        , Version(1)
    {
    }
};

// Serializable slotted container data
USTRUCT(BlueprintType)
struct YOURGAME_API FSerializableSlottedContainerData : public FSerializableContainerData
{
    GENERATED_BODY()
    
    // Slot mappings (slot index to item index in Items array)
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    TMap<int32, int32> SlotMapping;
    
    // Locked slots
    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    TArray<int32> LockedSlots;
    
    // Default constructor
    FSerializableSlottedContainerData()
        : FSerializableContainerData()
    {
        ContainerType = FName("Slotted");
    }
};
```

#### 2. Item Instance Serialization Implementation

```cpp
// SerializableItemTypes.cpp
#include "SerializableItemTypes.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

FItemInstance FSerializableItemInstance::ToItemInstance(UObject* WorldContextObject) const
{
    FItemInstance Result;
    
    // Load the item data asset
    if (ItemDataId.IsValid())
    {
        UAssetManager& AssetManager = UAssetManager::Get();
        UItemDataAsset* LoadedItemData = Cast<UItemDataAsset>(AssetManager.GetPrimaryAssetObject(ItemDataId));
        
        // If not loaded yet, try to load synchronously
        if (!LoadedItemData)
        {
            LoadedItemData = Cast<UItemDataAsset>(UAssetManager::GetStreamableManager().LoadSynchronous(ItemDataId));
        }
        
        Result.ItemData = LoadedItemData;
    }
    
    // Copy other properties
    Result.StackCount = StackCount;
    Result.ItemGuid = ItemGuid;
    Result.ItemLevel = ItemLevel;
    Result.Durability = Durability;
    Result.CustomData = CustomData;
    
    return Result;
}
```

#### 3. Inventory Save Game Object

```cpp
// InventorySaveGame.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SerializableItemTypes.h"
#include "InventorySaveGame.generated.h"

UCLASS()
class YOURGAME_API UInventorySaveGame : public USaveGame
{
    GENERATED_BODY()
    
public:
    UInventorySaveGame();
    
    // Save game version
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    int32 SaveVersion;
    
    // Player inventory containers
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    TMap<FGuid, FSerializableContainerData> PlayerContainers;
    
    // World item containers (chests, etc.)
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    TMap<FGuid, FSerializableContainerData> WorldContainers;
    
    // Equipment containers
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    TMap<FGuid, FSerializableSlottedContainerData> EquipmentContainers;
    
    // Save timestamp
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    FDateTime SaveTimestamp;
};
```

#### 4. Inventory Persistence Manager

```cpp
// InventoryPersistenceManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InventoryContainerInterface.h"
#include "SerializableItemTypes.h"
#include "InventorySaveGame.h"
#include "InventoryPersistenceManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySaved, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryLoaded, bool, bSuccess);

UCLASS()
class YOURGAME_API UInventoryPersistenceManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // Initialize and shutdown
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Persistence")
    FOnInventorySaved OnInventorySaved;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Persistence")
    FOnInventoryLoaded OnInventoryLoaded;
    
    // Save all registered containers
    UFUNCTION(BlueprintCallable, Category = "Inventory|Persistence")
    bool SaveAllInventories(const FString& SlotName);
    
    // Load all registered containers
    UFUNCTION(BlueprintCallable, Category = "Inventory|Persistence")
    bool LoadAllInventories(const FString& SlotName);
    
    // Register a container for persistence
    UFUNCTION(BlueprintCallable, Category = "Inventory|Persistence")
    void RegisterContainer(UObject* ContainerObject, const FGuid& ContainerId, bool bIsPlayerContainer = true);
    
    // Unregister a container
    UFUNCTION(BlueprintCallable, Category = "Inventory|Persistence")
    void UnregisterContainer(const FGuid& ContainerId);
    
    // Serialize a container to save data
    UFUNCTION(BlueprintCallable, Category = "Inventory|Persistence")
    FSerializableContainerData SerializeContainer(UObject* ContainerObject);
    
    // Deserialize save data to a container
    UFUNCTION(BlueprintCallable, Category = "Inventory|Persistence")
    bool DeserializeContainer(UObject* ContainerObject, const FSerializableContainerData& SaveData);
    
private:
    // Map of registered containers
    TMap<FGuid, TWeakObjectPtr<UObject>> RegisteredContainers;
    
    // Map of container types (player vs. world)
    TMap<FGuid, bool> ContainerTypes;
    
    // Current save game object
    UPROPERTY()
    UInventorySaveGame* CurrentSaveGame;
    
    // Create a new save game object
    UInventorySaveGame* CreateSaveGameObject();
    
    // Convert between runtime and serializable formats
    FSerializableItemInstance ConvertToSerializable(const FItemInstance& ItemInstance);
    FItemInstance ConvertFromSerializable(const FSerializableItemInstance& SerializedItem);
    
    // Validate loaded data
    bool ValidateSaveData(UInventorySaveGame* SaveGame);
    
    // Handle version migration
    bool MigrateSaveData(UInventorySaveGame* SaveGame);
};
```

#### 5. Container Serialization Interface

```cpp
// InventorySerializationInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SerializableItemTypes.h"
#include "InventorySerializationInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInventorySerializationInterface : public UInterface
{
    GENERATED_BODY()
};

class YOURGAME_API IInventorySerializationInterface
{
    GENERATED_BODY()

public:
    // Serialize container to save data
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Persistence")
    FSerializableContainerData SerializeContainer();
    
    // Deserialize save data to container
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Persistence")
    bool DeserializeContainer(const FSerializableContainerData& SaveData);
    
    // Get container ID
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Persistence")
    FGuid GetContainerId() const;
    
    // Get container type
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Persistence")
    FName GetContainerType() const;
};
```

## Usage Examples

### Implementing Serialization in Container Components

```cpp
// Add to InventoryContainerComponent.h
#include "InventorySerializationInterface.h"

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UInventoryContainerComponent : public UActorComponent, 
    public IInventoryContainerInterface, 
    public IInventorySerializationInterface
{
    GENERATED_BODY()
    
    // ... existing code ...
    
    // Container ID for persistence
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence")
    FGuid ContainerId;
    
    // Whether to auto-register for persistence
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence")
    bool bAutoRegisterForPersistence;
    
    // Whether this is a player container
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence")
    bool bIsPlayerContainer;
    
    // IInventorySerializationInterface implementation
    virtual FSerializableContainerData SerializeContainer_Implementation() override;
    virtual bool DeserializeContainer_Implementation(const FSerializableContainerData& SaveData) override;
    virtual FGuid GetContainerId_Implementation() const override { return ContainerId; }
    virtual FName GetContainerType_Implementation() const override { return FName("Basic"); }
    
protected:
    // Register for persistence on begin play
    virtual void BeginPlay() override;
    
    // Unregister on end play
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};

// Implementation in InventoryContainerComponent.cpp
void UInventoryContainerComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Generate a container ID if not set
    if (ContainerId.IsValid() == false)
    {
        ContainerId = FGuid::NewGuid();
    }
    
    // Auto-register for persistence if enabled
    if (bAutoRegisterForPersistence)
    {
        UInventoryPersistenceManager* PersistenceManager = GetGameInstance()->GetSubsystem<UInventoryPersistenceManager>();
        if (PersistenceManager)
        {
            PersistenceManager->RegisterContainer(this, ContainerId, bIsPlayerContainer);
        }
    }
}

void UInventoryContainerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Unregister from persistence manager
    if (bAutoRegisterForPersistence)
    {
        UInventoryPersistenceManager* PersistenceManager = GetGameInstance()->GetSubsystem<UInventoryPersistenceManager>();
        if (PersistenceManager)
        {
            PersistenceManager->UnregisterContainer(ContainerId);
        }
    }
    
    Super::EndPlay(EndPlayReason);
}

FSerializableContainerData UInventoryContainerComponent::SerializeContainer_Implementation()
{
    FSerializableContainerData SaveData;
    
    // Set container properties
    SaveData.Capacity = Capacity;
    SaveData.WeightCapacity = WeightCapacity;
    SaveData.ContainerType = GetContainerType_Implementation();
    SaveData.ContainerId = ContainerId;
    SaveData.Version = 1;
    
    // Set owner ID if available
    AActor* OwningActor = GetOwner();
    if (OwningActor)
    {
        if (APlayerState* PlayerState = Cast<APlayerState>(OwningActor))
        {
            SaveData.OwnerId = FGuid::NewGuid(); // Use player ID in a real implementation
        }
        else
        {
            SaveData.OwnerId = FGuid(); // No owner or world item
        }
    }
    
    // Serialize all items
    for (const FItemInstance& Item : Items)
    {
        if (Item.IsValid())
        {
            FSerializableItemInstance SerializedItem(Item);
            SaveData.Items.Add(SerializedItem);
        }
    }
    
    return SaveData;
}

bool UInventoryContainerComponent::DeserializeContainer_Implementation(const FSerializableContainerData& SaveData)
{
    // Validate container ID
    if (SaveData.ContainerId != ContainerId)
    {
        UE_LOG(LogInventory, Warning, TEXT("Container ID mismatch during deserialization"));
        return false;
    }
    
    // Update container properties
    Capacity = SaveData.Capacity;
    WeightCapacity = SaveData.WeightCapacity;
    
    // Clear existing items
    Items.Empty();
    
    // Deserialize items
    for (const FSerializableItemInstance& SerializedItem : SaveData.Items)
    {
        FItemInstance Item = SerializedItem.ToItemInstance(this);
        if (Item.IsValid())
        {
            Items.Add(Item);
        }
        else
        {
            UE_LOG(LogInventory, Warning, TEXT("Failed to deserialize item: %s"), *SerializedItem.ItemDataId.ToString());
        }
    }
    
    // Notify inventory changed
    NotifyInventoryChanged();
    
    return true;
}
```

### Saving and Loading Inventory Data

```cpp
// In your game mode or save game manager
void AYourGameMode::SaveGame(const FString& SlotName)
{
    // Save inventory data
    UInventoryPersistenceManager* PersistenceManager = GetGameInstance()->GetSubsystem<UInventoryPersistenceManager>();
    if (PersistenceManager)
    {
        bool bInventorySaved = PersistenceManager->SaveAllInventories(SlotName);
        
        if (bInventorySaved)
        {
            UE_LOG(LogGame, Display, TEXT("Inventory data saved successfully"));
        }
        else
        {
            UE_LOG(LogGame, Warning, TEXT("Failed to save inventory data"));
        }
    }
    
    // Save other game data
    // ...
}

void AYourGameMode::LoadGame(const FString& SlotName)
{
    // Load inventory data
    UInventoryPersistenceManager* PersistenceManager = GetGameInstance()->GetSubsystem<UInventoryPersistenceManager>();
    if (PersistenceManager)
    {
        bool bInventoryLoaded = PersistenceManager->LoadAllInventories(SlotName);
        
        if (bInventoryLoaded)
        {
            UE_LOG(LogGame, Display, TEXT("Inventory data loaded successfully"));
        }
        else
        {
            UE_LOG(LogGame, Warning, TEXT("Failed to load inventory data"));
        }
    }
    
    // Load other game data
    // ...
}
```

### Handling Custom Item Data

```cpp
// For items with custom data
void AYourGameCharacter::AddCustomItemToInventory(UItemDataAsset* ItemData, const FString& CustomData)
{
    if (!ItemData)
    {
        return;
    }
    
    // Create the item instance
    FItemInstance Item(ItemData);
    
    // Set custom data (e.g., JSON string with custom properties)
    Item.CustomData = CustomData;
    
    // Add to inventory
    UInventoryContainerComponent* Inventory = FindComponentByClass<UInventoryContainerComponent>();
    if (Inventory)
    {
        int32 RemainingCount = 0;
        Inventory->AddItem(Item, RemainingCount);
    }
}

// Example of using custom data
void AYourGameCharacter::CreateEnchantedWeapon(UItemDataAsset* WeaponData, const TArray<FName>& Enchantments)
{
    if (!WeaponData)
    {
        return;
    }
    
    // Create JSON object for custom data
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    
    // Add enchantments array
    TArray<TSharedPtr<FJsonValue>> EnchantmentValues;
    for (const FName& Enchantment : Enchantments)
    {
        EnchantmentValues.Add(MakeShareable(new FJsonValueString(Enchantment.ToString())));
    }
    JsonObject->SetArrayField(TEXT("Enchantments"), EnchantmentValues);
    
    // Add creation timestamp
    JsonObject->SetStringField(TEXT("CreatedAt"), FDateTime::Now().ToString());
    
    // Serialize to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    // Add to inventory with custom data
    AddCustomItemToInventory(WeaponData, OutputString);
}

// Reading custom data
void AYourGameCharacter::UseEnchantedWeapon(const FItemInstance& WeaponItem)
{
    if (!WeaponItem.IsValid() || WeaponItem.CustomData.IsEmpty())
    {
        return;
    }
    
    // Parse JSON
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(WeaponItem.CustomData);
    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        // Get enchantments
        TArray<TSharedPtr<FJsonValue>> EnchantmentValues = JsonObject->GetArrayField(TEXT("Enchantments"));
        TArray<FName> Enchantments;
        
        for (const TSharedPtr<FJsonValue>& Value : EnchantmentValues)
        {
            Enchantments.Add(FName(*Value->AsString()));
        }
        
        // Apply enchantment effects
        for (const FName& Enchantment : Enchantments)
        {
            UE_LOG(LogInventory, Display, TEXT("Applying enchantment effect: %s"), *Enchantment.ToString());
            // Apply effect logic here
        }
    }
}
```

## AI Prompt Templates

### Implementing Custom Serialization

```
Implement custom serialization for [ITEM_TYPE] items in Unreal Engine 5 C++ that:
- Stores [SPECIFIC_PROPERTY_1] and [SPECIFIC_PROPERTY_2]
- Handles versioning for backward compatibility
- Validates data during loading
- Integrates with the existing inventory serialization system
Include any necessary structs, helper functions, and serialization/deserialization methods.
```

### Creating Save Game Extensions

```
Extend the inventory save system in Unreal Engine 5 C++ to:
- Support [STORAGE_FORMAT] for [USE_CASE]
- Add [FEATURE] to the existing persistence system
- Optimize [PERFORMANCE_ASPECT]
- Handle [EDGE_CASE]
Include both C++ code and Blueprint-callable functions.
```

## Integration Tips

1. **Async Loading**: Consider using async loading for large inventories to avoid hitches.

2. **Data Validation**: Always validate loaded data to prevent corruption or exploits.

3. **Versioning**: Implement proper versioning to handle save data from older game versions.

4. **Error Handling**: Provide graceful fallbacks when items can't be loaded.

5. **Custom Data**: Use structured formats like JSON for custom item data to ensure extensibility.

## Performance Considerations

1. **Save Size Optimization**: Only save necessary data to keep save files small.

2. **Lazy Loading**: Consider lazy-loading item data only when needed.

3. **Batch Processing**: Process serialization in batches for large inventories.

4. **Compression**: Use compression for large save files.

## Common Pitfalls

1. **Missing Assets**: Handle cases where item data assets are missing or have been renamed.

2. **Circular References**: Be careful with circular references in saved data.

3. **Version Compatibility**: Test loading saves from all supported game versions.

4. **Thread Safety**: Ensure serialization is thread-safe if using async saving/loading.

5. **Save Corruption**: Implement safeguards against save corruption, such as backups or checksums. 