# Pattern 3: Item Interaction Framework

## Problem Statement

Game developers need a robust system for players to interact with items in the game world, including picking up, dropping, using, and equipping items. This system must handle networking, animations, visual feedback, and integrate with the inventory container system while maintaining a clean, extensible architecture.

## Solution Approach

Implement an interaction framework that:

1. Provides a consistent interface for all item interactions
2. Supports different interaction types (pickup, use, equip, examine)
3. Handles networking for multiplayer games
4. Integrates with animation and VFX systems
5. Connects with the inventory container system
6. Allows for easy extension with new interaction types

## Implementation

### Core Components

#### 1. Interactable Interface

```cpp
// InteractableInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// Interaction types
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    Pickup UMETA(DisplayName = "Pickup"),
    Use UMETA(DisplayName = "Use"),
    Examine UMETA(DisplayName = "Examine"),
    Talk UMETA(DisplayName = "Talk"),
    Open UMETA(DisplayName = "Open"),
    Custom UMETA(DisplayName = "Custom")
};

// Interaction result
UENUM(BlueprintType)
enum class EInteractionResult : uint8
{
    Success UMETA(DisplayName = "Success"),
    Failed UMETA(DisplayName = "Failed"),
    InProgress UMETA(DisplayName = "In Progress"),
    Canceled UMETA(DisplayName = "Canceled")
};

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

class YOURGAME_API IInteractableInterface
{
    GENERATED_BODY()

public:
    // Check if interaction is possible
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    bool CanInteract(AActor* Interactor, EInteractionType& OutInteractionType);
    
    // Perform the interaction
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    EInteractionResult Interact(AActor* Interactor, EInteractionType InteractionType);
    
    // Get interaction text to display in UI
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractionText(AActor* Interactor, EInteractionType InteractionType);
    
    // Get interaction duration (for interactions that take time)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    float GetInteractionDuration(AActor* Interactor, EInteractionType InteractionType);
    
    // Get available interaction types
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    TArray<EInteractionType> GetInteractionTypes(AActor* Interactor);
};
```

#### 2. World Item Actor

```cpp
// WorldItemActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "ItemInstance.h"
#include "WorldItemActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldItemPickedUp, AWorldItemActor*, WorldItem);

UCLASS()
class YOURGAME_API AWorldItemActor : public AActor, public IInteractableInterface
{
    GENERATED_BODY()
    
public:    
    AWorldItemActor();

protected:
    virtual void BeginPlay() override;
    
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent* InteractionVolume;
    
    // Item data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", ReplicatedUsing = OnRep_ItemInstance)
    FItemInstance ItemInstance;
    
    // Visual settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    bool bRotateItem;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (EditCondition = "bRotateItem"))
    float RotationSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    bool bBobUpAndDown;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (EditCondition = "bBobUpAndDown"))
    float BobAmplitude;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (EditCondition = "bBobUpAndDown"))
    float BobFrequency;
    
    // Interaction settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    float PickupRadius;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    bool bDestroyOnPickup;
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Item")
    FOnWorldItemPickedUp OnWorldItemPickedUp;
    
    // Replication
    UFUNCTION()
    void OnRep_ItemInstance();
    
public:    
    virtual void Tick(float DeltaTime) override;
    
    // Initialize with item data
    UFUNCTION(BlueprintCallable, Category = "Item")
    void InitializeWithItem(const FItemInstance& InItemInstance);
    
    // Get item instance
    UFUNCTION(BlueprintCallable, Category = "Item")
    FItemInstance GetItemInstance() const { return ItemInstance; }
    
    // IInteractableInterface implementation
    virtual bool CanInteract_Implementation(AActor* Interactor, EInteractionType& OutInteractionType) override;
    virtual EInteractionResult Interact_Implementation(AActor* Interactor, EInteractionType InteractionType) override;
    virtual FText GetInteractionText_Implementation(AActor* Interactor, EInteractionType InteractionType) override;
    virtual float GetInteractionDuration_Implementation(AActor* Interactor, EInteractionType InteractionType) override;
    virtual TArray<EInteractionType> GetInteractionTypes_Implementation(AActor* Interactor) override;
    
protected:
    // Handle pickup interaction
    virtual EInteractionResult HandlePickup(AActor* Interactor);
    
    // Handle examine interaction
    virtual EInteractionResult HandleExamine(AActor* Interactor);
    
    // Update visual appearance based on item data
    virtual void UpdateVisuals();
    
    // Networking
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

#### 3. Item Interaction Component

```cpp
// ItemInteractionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableInterface.h"
#include "ItemInteractionComponent.generated.h"

// Delegate for interaction events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionStarted, AActor*, Interactor, AActor*, InteractableActor, EInteractionType, InteractionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInteractionCompleted, AActor*, Interactor, AActor*, InteractableActor, EInteractionType, InteractionType, EInteractionResult, Result);

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UItemInteractionComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:    
    UItemInteractionComponent();

protected:
    virtual void BeginPlay() override;
    
    // Interaction settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    float InteractionDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    bool bUseInteractionTrace;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (EditCondition = "bUseInteractionTrace"))
    TArray<TEnumAsByte<EObjectTypeQuery>> InteractionTraceObjectTypes;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    bool bShowDebugTrace;
    
    // Current interaction state
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    AActor* CurrentInteractableActor;
    
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    EInteractionType CurrentInteractionType;
    
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    float InteractionProgress;
    
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    bool bIsInteracting;
    
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    float InteractionStartTime;
    
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    float InteractionDuration;
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Interaction")
    FOnInteractionStarted OnInteractionStarted;
    
    UPROPERTY(BlueprintAssignable, Category = "Interaction")
    FOnInteractionCompleted OnInteractionCompleted;
    
public:    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Find interactable in front of the player
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    AActor* FindInteractableInRange(TArray<EInteractionType>& OutAvailableInteractions);
    
    // Start interaction
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    bool StartInteraction(AActor* InteractableActor, EInteractionType InteractionType);
    
    // Cancel current interaction
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void CancelInteraction();
    
    // Get current interaction text
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    FText GetCurrentInteractionText() const;
    
    // Check if currently interacting
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    bool IsInteracting() const { return bIsInteracting; }
    
    // Get interaction progress (0-1)
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    float GetInteractionProgress() const { return InteractionProgress; }
    
protected:
    // Perform interaction trace
    AActor* PerformInteractionTrace(float Distance, FHitResult& OutHit);
    
    // Complete current interaction
    void CompleteInteraction(EInteractionResult Result);
    
    // Update interaction progress
    void UpdateInteractionProgress(float DeltaTime);
};
```

#### 4. Item Use Handler

```cpp
// ItemUseHandler.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInstance.h"
#include "ItemUseHandler.generated.h"

// Result of using an item
UENUM(BlueprintType)
enum class EItemUseResult : uint8
{
    Success UMETA(DisplayName = "Success"),
    Failed UMETA(DisplayName = "Failed"),
    Canceled UMETA(DisplayName = "Canceled"),
    Cooldown UMETA(DisplayName = "On Cooldown"),
    NoEffect UMETA(DisplayName = "No Effect")
};

UINTERFACE(MinimalAPI, Blueprintable)
class UItemUseHandler : public UInterface
{
    GENERATED_BODY()
};

class YOURGAME_API IItemUseHandler
{
    GENERATED_BODY()

public:
    // Check if the item can be used
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
    bool CanUseItem(const FItemInstance& ItemInstance);
    
    // Use the item
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
    EItemUseResult UseItem(const FItemInstance& ItemInstance);
    
    // Get cooldown remaining for item use
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
    float GetItemCooldownRemaining(const FItemInstance& ItemInstance);
    
    // Get use animation to play
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
    UAnimMontage* GetItemUseAnimation(const FItemInstance& ItemInstance);
};
```

#### 5. Item Drop Manager

```cpp
// ItemDropManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInstance.h"
#include "WorldItemActor.h"
#include "ItemDropManager.generated.h"

UCLASS()
class YOURGAME_API AItemDropManager : public AActor
{
    GENERATED_BODY()
    
public:    
    AItemDropManager();

protected:
    virtual void BeginPlay() override;
    
    // World item actor class
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    TSubclassOf<AWorldItemActor> DefaultWorldItemClass;
    
    // Custom world item classes based on item type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    TMap<FName, TSubclassOf<AWorldItemActor>> CustomWorldItemClasses;
    
    // Drop settings
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
    float DropForce;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
    float DropSpread;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
    float DropUpwardForce;
    
public:    
    // Spawn a world item from an item instance
    UFUNCTION(BlueprintCallable, Category = "Item")
    AWorldItemActor* SpawnWorldItem(const FItemInstance& ItemInstance, const FVector& Location, const FRotator& Rotation);
    
    // Drop an item from an actor with physics
    UFUNCTION(BlueprintCallable, Category = "Item")
    AWorldItemActor* DropItem(const FItemInstance& ItemInstance, AActor* SourceActor, float ForceMultiplier = 1.0f);
    
    // Drop multiple items from an actor
    UFUNCTION(BlueprintCallable, Category = "Item")
    TArray<AWorldItemActor*> DropItems(const TArray<FItemInstance>& ItemInstances, AActor* SourceActor, float ForceMultiplier = 1.0f);
    
protected:
    // Get the appropriate world item class for an item
    TSubclassOf<AWorldItemActor> GetWorldItemClassForItem(const FItemInstance& ItemInstance);
    
    // Calculate drop velocity
    FVector CalculateDropVelocity(AActor* SourceActor, float ForceMultiplier);
};
```

## Usage Examples

### Setting Up Player Interaction

```cpp
// In your character class constructor
InteractionComponent = CreateDefaultSubobject<UItemInteractionComponent>(TEXT("InteractionComponent"));
InteractionComponent->InteractionDistance = 250.0f;
InteractionComponent->bUseInteractionTrace = true;

// In your character's input setup
void AYourGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Bind interaction input
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AYourGameCharacter::StartInteraction);
    PlayerInputComponent->BindAction("Interact", IE_Released, this, &AYourGameCharacter::StopInteraction);
}

// Interaction methods
void AYourGameCharacter::StartInteraction()
{
    TArray<EInteractionType> AvailableInteractions;
    AActor* InteractableActor = InteractionComponent->FindInteractableInRange(AvailableInteractions);
    
    if (InteractableActor && AvailableInteractions.Num() > 0)
    {
        // Start the first available interaction
        InteractionComponent->StartInteraction(InteractableActor, AvailableInteractions[0]);
    }
}

void AYourGameCharacter::StopInteraction()
{
    if (InteractionComponent->IsInteracting())
    {
        InteractionComponent->CancelInteraction();
    }
}
```

### Creating a Pickupable Item

```cpp
// Spawn a world item
UItemManagerSubsystem* ItemManager = GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
UItemDataAsset* HealthPotion = ItemManager->GetItemDataByName(FName("Consumable"), FName("HealthPotion"));

if (HealthPotion)
{
    FItemInstance PotionInstance(HealthPotion, 1);
    
    // Get the drop manager
    AItemDropManager* DropManager = Cast<AItemDropManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AItemDropManager::StaticClass()));
    
    if (DropManager)
    {
        // Spawn the item in the world
        FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
        FRotator SpawnRotation = FRotator::ZeroRotator;
        
        AWorldItemActor* WorldItem = DropManager->SpawnWorldItem(PotionInstance, SpawnLocation, SpawnRotation);
        
        // Subscribe to pickup event
        if (WorldItem)
        {
            WorldItem->OnWorldItemPickedUp.AddDynamic(this, &AYourClass::OnItemPickedUp);
        }
    }
}

// Handle item pickup
void AYourClass::OnItemPickedUp(AWorldItemActor* WorldItem)
{
    UE_LOG(LogInventory, Display, TEXT("Item picked up: %s"), 
        *WorldItem->GetItemInstance().ItemData->GetItemName().ToString());
}
```

### Implementing Item Use

```cpp
// In your character class, implement the ItemUseHandler interface
UCLASS()
class YOURGAME_API AYourGameCharacter : public ACharacter, public IItemUseHandler
{
    GENERATED_BODY()
    
    // ... other character code ...
    
    // IItemUseHandler implementation
    virtual bool CanUseItem_Implementation(const FItemInstance& ItemInstance) override;
    virtual EItemUseResult UseItem_Implementation(const FItemInstance& ItemInstance) override;
    virtual float GetItemCooldownRemaining_Implementation(const FItemInstance& ItemInstance) override;
    virtual UAnimMontage* GetItemUseAnimation_Implementation(const FItemInstance& ItemInstance) override;
    
    // Item cooldowns
    UPROPERTY()
    TMap<FPrimaryAssetId, float> ItemCooldowns;
};

// Implementation
bool AYourGameCharacter::CanUseItem_Implementation(const FItemInstance& ItemInstance)
{
    if (!ItemInstance.IsValid() || !ItemInstance.ItemData->CanBeUsed())
    {
        return false;
    }
    
    // Check cooldown
    if (GetItemCooldownRemaining(ItemInstance) > 0.0f)
    {
        return false;
    }
    
    return true;
}

EItemUseResult AYourGameCharacter::UseItem_Implementation(const FItemInstance& ItemInstance)
{
    if (!CanUseItem(ItemInstance))
    {
        return EItemUseResult::Failed;
    }
    
    // Handle different item types
    FName ItemType = ItemInstance.ItemData->GetItemType();
    
    if (ItemType == FName("Consumable"))
    {
        // Apply consumable effect
        // ...
        
        // Set cooldown
        ItemCooldowns.Add(ItemInstance.ItemData->GetPrimaryAssetId(), GetWorld()->GetTimeSeconds() + 1.0f);
        
        return EItemUseResult::Success;
    }
    
    return EItemUseResult::NoEffect;
}

float AYourGameCharacter::GetItemCooldownRemaining_Implementation(const FItemInstance& ItemInstance)
{
    if (!ItemInstance.IsValid())
    {
        return 0.0f;
    }
    
    FPrimaryAssetId ItemId = ItemInstance.ItemData->GetPrimaryAssetId();
    float* CooldownEndTime = ItemCooldowns.Find(ItemId);
    
    if (CooldownEndTime)
    {
        float RemainingTime = *CooldownEndTime - GetWorld()->GetTimeSeconds();
        return FMath::Max(0.0f, RemainingTime);
    }
    
    return 0.0f;
}

UAnimMontage* AYourGameCharacter::GetItemUseAnimation_Implementation(const FItemInstance& ItemInstance)
{
    // Return appropriate animation based on item type
    // ...
    
    return nullptr;
}
```

## AI Prompt Templates

### Creating Custom Interactable Items

```
Create a custom interactable item class for [ITEM_TYPE] in Unreal Engine 5 C++.
It should:
- Extend AWorldItemActor or implement IInteractableInterface
- Support [INTERACTION_TYPE_1] and [INTERACTION_TYPE_2] interactions
- Include custom visual effects for [VISUAL_EFFECT]
- Handle networking for multiplayer games
Include any necessary components, helper functions, and override appropriate interface methods.
```

### Implementing Item Use Effects

```
Implement a use effect system for [ITEM_CATEGORY] items in Unreal Engine 5 C++ that:
- Applies [EFFECT_TYPE] to the user or target
- Supports different effect magnitudes based on item properties
- Includes visual and audio feedback
- Integrates with the existing item use handler interface
```

## Integration Tips

1. **Animation Integration**: Connect item interactions with the animation system for smooth visual feedback.

2. **Networking**: Ensure all interactions are properly replicated for multiplayer games.

3. **UI Feedback**: Provide clear UI indicators for available interactions and interaction progress.

4. **Sound Effects**: Add appropriate sound effects for different interaction types.

5. **VFX**: Use particle effects to enhance interaction visual feedback.

## Performance Considerations

1. **Interaction Queries**: Optimize interaction detection to avoid expensive traces every frame.

2. **LOD System**: Implement a level-of-detail system for world items to reduce rendering cost at distance.

3. **Pooling**: Consider object pooling for frequently spawned world items.

4. **Batched Physics**: Use batched physics updates for dropped items.

## Common Pitfalls

1. **Authority Issues**: Ensure interactions respect server authority in multiplayer games.

2. **Animation Sync**: Make sure interaction durations match animation lengths.

3. **Collision Problems**: Carefully set up collision profiles to avoid interaction issues.

4. **UI Desync**: Keep the UI in sync with actual interaction state.

5. **Edge Cases**: Handle edge cases like interrupted interactions or simultaneous interactions by multiple players. 