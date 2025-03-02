# Pattern 4: Inventory UI Integration

## Problem Statement

Game developers need a flexible, responsive UI system to visualize and interact with inventory data. This system must support drag-and-drop operations, tooltips, context menus, and provide a seamless user experience while maintaining a clean separation between game logic and presentation.

## Solution Approach

Implement a UI architecture that:

1. Creates a clean separation between inventory data and its visual representation
2. Provides reusable UI components for common inventory elements
3. Supports drag-and-drop operations for item manipulation
4. Handles tooltips and context menus for detailed item information
5. Responds to inventory data changes in real-time
6. Scales across different screen sizes and input methods

## Implementation

### Core Components

#### 1. Inventory Widget Base

```cpp
// InventoryWidgetBase.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryContainerInterface.h"
#include "ItemInstance.h"
#include "InventoryWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryWidgetItemSelected, const FItemInstance&, SelectedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryWidgetItemAction, const FItemInstance&, Item, const FName&, ActionName);

UCLASS(Abstract, Blueprintable)
class YOURGAME_API UInventoryWidgetBase : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UInventoryWidgetBase(const FObjectInitializer& ObjectInitializer);
    
protected:
    // The inventory container to display
    UPROPERTY(BlueprintReadWrite, Category = "Inventory", meta = (ExposeOnSpawn = true))
    TScriptInterface<IInventoryContainerInterface> InventoryContainer;
    
    // Currently selected item
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    FItemInstance SelectedItem;
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryWidgetItemSelected OnItemSelected;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryWidgetItemAction OnItemAction;
    
    // Widget initialization
    virtual void NativeConstruct() override;
    
    // Widget tick
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
    // Refresh the inventory display
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void RefreshInventory();
    
    // Handle inventory container changes
    UFUNCTION()
    virtual void OnInventoryContainerChanged(class UInventoryContainerComponent* Container);
    
public:
    // Set the inventory container to display
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void SetInventoryContainer(const TScriptInterface<IInventoryContainerInterface>& InContainer);
    
    // Get the inventory container
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TScriptInterface<IInventoryContainerInterface> GetInventoryContainer() const { return InventoryContainer; }
    
    // Select an item
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void SelectItem(const FItemInstance& Item);
    
    // Get the selected item
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FItemInstance GetSelectedItem() const { return SelectedItem; }
    
    // Perform an action on an item
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void PerformItemAction(const FItemInstance& Item, const FName& ActionName);
};
```

#### 2. Inventory Slot Widget

```cpp
// InventorySlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemInstance.h"
#include "InventorySlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySlotSelected, class UInventorySlotWidget*, SlotWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotClicked, class UInventorySlotWidget*, SlotWidget, const FPointerEvent&, MouseEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotHovered, class UInventorySlotWidget*, SlotWidget, bool, bIsHovered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySlotDragDetected, class UInventorySlotWidget*, SlotWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySlotDragCancelled, class UInventorySlotWidget*, SlotWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotDrop, class UInventorySlotWidget*, SlotWidget, class UInventorySlotWidget*, DroppedOnSlot);

UCLASS(Abstract, Blueprintable)
class YOURGAME_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UInventorySlotWidget(const FObjectInitializer& ObjectInitializer);
    
protected:
    // The item in this slot
    UPROPERTY(BlueprintReadWrite, Category = "Inventory", meta = (ExposeOnSpawn = true))
    FItemInstance ItemInstance;
    
    // Slot index
    UPROPERTY(BlueprintReadWrite, Category = "Inventory", meta = (ExposeOnSpawn = true))
    int32 SlotIndex;
    
    // Whether this slot is selected
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    bool bIsSelected;
    
    // Whether this slot is locked
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    bool bIsLocked;
    
    // UI Components (to be bound in Blueprint)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UImage* ItemIcon;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UTextBlock* StackCountText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UBorder* SelectionBorder;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UImage* LockIcon;
    
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventorySlotSelected OnSlotSelected;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventorySlotClicked OnSlotClicked;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventorySlotHovered OnSlotHovered;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventorySlotDragDetected OnSlotDragDetected;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventorySlotDragCancelled OnSlotDragCancelled;
    
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventorySlotDrop OnSlotDrop;
    
    // Widget initialization
    virtual void NativeConstruct() override;
    
    // Widget tick
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
    // Update the visual appearance
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void UpdateSlotVisuals();
    
    // Input event handlers
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    
public:
    // Set the item in this slot
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void SetItem(const FItemInstance& InItemInstance);
    
    // Get the item in this slot
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FItemInstance GetItem() const { return ItemInstance; }
    
    // Set the slot index
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetSlotIndex(int32 InSlotIndex) { SlotIndex = InSlotIndex; }
    
    // Get the slot index
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetSlotIndex() const { return SlotIndex; }
    
    // Set whether this slot is selected
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void SetSelected(bool bInIsSelected);
    
    // Check if this slot is selected
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool IsSelected() const { return bIsSelected; }
    
    // Set whether this slot is locked
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void SetLocked(bool bInIsLocked);
    
    // Check if this slot is locked
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool IsLocked() const { return bIsLocked; }
    
    // Check if this slot is empty
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool IsEmpty() const { return !ItemInstance.IsValid(); }
};
```

#### 3. Grid Inventory Widget

```cpp
// GridInventoryWidget.h
#pragma once

#include "CoreMinimal.h"
#include "InventoryWidgetBase.h"
#include "InventorySlotWidget.h"
#include "GridInventoryWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class YOURGAME_API UGridInventoryWidget : public UInventoryWidgetBase
{
    GENERATED_BODY()
    
public:
    UGridInventoryWidget(const FObjectInitializer& ObjectInitializer);
    
protected:
    // Grid layout settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
    int32 GridColumns;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
    int32 GridRows;
    
    // Slot widget class
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;
    
    // UI Components (to be bound in Blueprint)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UUniformGridPanel* GridPanel;
    
    // Array of slot widgets
    UPROPERTY(BlueprintReadOnly, Category = "Grid")
    TArray<UInventorySlotWidget*> SlotWidgets;
    
    // Widget initialization
    virtual void NativeConstruct() override;
    
    // Refresh the inventory display
    virtual void RefreshInventory() override;
    
    // Create the grid slots
    UFUNCTION(BlueprintCallable, Category = "Grid")
    virtual void CreateGridSlots();
    
    // Handle slot selection
    UFUNCTION()
    virtual void OnSlotSelected(UInventorySlotWidget* SlotWidget);
    
    // Handle slot clicked
    UFUNCTION()
    virtual void OnSlotClicked(UInventorySlotWidget* SlotWidget, const FPointerEvent& MouseEvent);
    
    // Handle slot drag detected
    UFUNCTION()
    virtual void OnSlotDragDetected(UInventorySlotWidget* SlotWidget);
    
    // Handle slot drop
    UFUNCTION()
    virtual void OnSlotDrop(UInventorySlotWidget* SlotWidget, UInventorySlotWidget* DroppedOnSlot);
    
public:
    // Set the grid dimensions
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void SetGridDimensions(int32 Columns, int32 Rows);
    
    // Get a slot widget by index
    UFUNCTION(BlueprintCallable, Category = "Grid")
    UInventorySlotWidget* GetSlotWidgetAtIndex(int32 Index) const;
    
    // Get the number of slots
    UFUNCTION(BlueprintCallable, Category = "Grid")
    int32 GetSlotCount() const { return SlotWidgets.Num(); }
};
```

#### 4. Item Tooltip Widget

```cpp
// ItemTooltipWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemInstance.h"
#include "ItemTooltipWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class YOURGAME_API UItemTooltipWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UItemTooltipWidget(const FObjectInitializer& ObjectInitializer);
    
protected:
    // The item to display
    UPROPERTY(BlueprintReadOnly, Category = "Item")
    FItemInstance ItemInstance;
    
    // UI Components (to be bound in Blueprint)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UTextBlock* ItemNameText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UTextBlock* ItemDescriptionText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UTextBlock* ItemTypeText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UTextBlock* ItemValueText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UTextBlock* ItemWeightText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UImage* ItemRarityBorder;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    class UVerticalBox* StatModifiersBox;
    
    // Widget initialization
    virtual void NativeConstruct() override;
    
    // Update the tooltip content
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual void UpdateTooltipContent();
    
    // Get color for item rarity
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual FLinearColor GetRarityColor() const;
    
public:
    // Set the item to display
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual void SetItem(const FItemInstance& InItemInstance);
    
    // Get the item being displayed
    UFUNCTION(BlueprintCallable, Category = "Item")
    FItemInstance GetItem() const { return ItemInstance; }
};
```

#### 5. Item Drag Drop Operation

```cpp
// ItemDragDropOperation.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemInstance.h"
#include "ItemDragDropOperation.generated.h"

UCLASS()
class YOURGAME_API UItemDragDropOperation : public UDragDropOperation
{
    GENERATED_BODY()
    
public:
    UItemDragDropOperation();
    
    // The item being dragged
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    FItemInstance DraggedItem;
    
    // Source slot index
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    int32 SourceSlotIndex;
    
    // Source inventory container
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    TScriptInterface<IInventoryContainerInterface> SourceContainer;
    
    // Whether this is a split operation
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    bool bIsSplitOperation;
    
    // Split count (if this is a split operation)
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    int32 SplitCount;
};
```

## Usage Examples

### Creating a Basic Inventory UI

```cpp
// In your game HUD or UI manager
void AYourGameHUD::ShowInventory(AActor* OwningActor)
{
    // Get the inventory container from the actor
    IInventoryContainerInterface* Container = Cast<IInventoryContainerInterface>(OwningActor);
    
    if (!Container)
    {
        // Try to get it from a component
        UInventoryContainerComponent* ContainerComp = OwningActor->FindComponentByClass<UInventoryContainerComponent>();
        if (ContainerComp)
        {
            Container = ContainerComp;
        }
    }
    
    if (!Container)
    {
        UE_LOG(LogUI, Warning, TEXT("Cannot show inventory: Actor does not have an inventory container"));
        return;
    }
    
    // Create the inventory widget
    if (!InventoryWidget)
    {
        InventoryWidget = CreateWidget<UGridInventoryWidget>(GetOwningPlayerController(), InventoryWidgetClass);
        
        if (InventoryWidget)
        {
            // Set up the grid dimensions
            InventoryWidget->SetGridDimensions(5, 6);
            
            // Subscribe to item selection
            InventoryWidget->OnItemSelected.AddDynamic(this, &AYourGameHUD::OnInventoryItemSelected);
            
            // Subscribe to item actions
            InventoryWidget->OnItemAction.AddDynamic(this, &AYourGameHUD::OnInventoryItemAction);
        }
    }
    
    if (InventoryWidget)
    {
        // Set the container and add to viewport
        InventoryWidget->SetInventoryContainer(Container);
        InventoryWidget->AddToViewport();
        
        // Set input mode to UI
        APlayerController* PC = GetOwningPlayerController();
        if (PC)
        {
            PC->SetInputMode(FInputModeGameAndUI());
            PC->bShowMouseCursor = true;
        }
    }
}

// Handle item selection
void AYourGameHUD::OnInventoryItemSelected(const FItemInstance& SelectedItem)
{
    // Show item tooltip
    if (!TooltipWidget && SelectedItem.IsValid())
    {
        TooltipWidget = CreateWidget<UItemTooltipWidget>(GetOwningPlayerController(), ItemTooltipWidgetClass);
        
        if (TooltipWidget)
        {
            TooltipWidget->SetItem(SelectedItem);
            TooltipWidget->AddToViewport();
        }
    }
    else if (TooltipWidget)
    {
        if (SelectedItem.IsValid())
        {
            TooltipWidget->SetItem(SelectedItem);
        }
        else
        {
            TooltipWidget->RemoveFromParent();
            TooltipWidget = nullptr;
        }
    }
}

// Handle item actions
void AYourGameHUD::OnInventoryItemAction(const FItemInstance& Item, const FName& ActionName)
{
    if (ActionName == FName("Use"))
    {
        // Find an item use handler
        AActor* OwningActor = GetOwningPawn();
        IItemUseHandler* UseHandler = Cast<IItemUseHandler>(OwningActor);
        
        if (UseHandler)
        {
            EItemUseResult Result = UseHandler->UseItem(Item);
            
            // Show result feedback
            if (Result == EItemUseResult::Success)
            {
                // Show success feedback
                // ...
            }
            else
            {
                // Show failure feedback
                // ...
            }
        }
    }
    else if (ActionName == FName("Drop"))
    {
        // Handle drop action
        // ...
    }
    else if (ActionName == FName("Equip"))
    {
        // Handle equip action
        // ...
    }
}
```

### Implementing Drag and Drop

```cpp
// In your InventorySlotWidget subclass

// Handle drag detected
FReply UYourInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (IsEmpty() || IsLocked())
    {
        return FReply::Unhandled();
    }
    
    // Create the drag-drop operation
    UItemDragDropOperation* DragDropOp = NewObject<UItemDragDropOperation>();
    DragDropOp->DraggedItem = ItemInstance;
    DragDropOp->SourceSlotIndex = SlotIndex;
    
    // Get the parent inventory widget
    UInventoryWidgetBase* ParentInventory = Cast<UInventoryWidgetBase>(GetParent());
    if (ParentInventory)
    {
        DragDropOp->SourceContainer = ParentInventory->GetInventoryContainer();
    }
    
    // Check if this is a split operation (e.g., holding Shift key)
    if (InMouseEvent.IsShiftDown() && ItemInstance.StackCount > 1)
    {
        DragDropOp->bIsSplitOperation = true;
        DragDropOp->SplitCount = FMath::Max(1, ItemInstance.StackCount / 2);
    }
    else
    {
        DragDropOp->bIsSplitOperation = false;
        DragDropOp->SplitCount = ItemInstance.StackCount;
    }
    
    // Create a drag widget
    UYourDragVisualWidget* DragVisual = CreateWidget<UYourDragVisualWidget>(GetOwningPlayer(), DragVisualWidgetClass);
    if (DragVisual)
    {
        DragVisual->SetItem(ItemInstance);
        DragVisual->SetStackCount(DragDropOp->SplitCount);
        DragDropOp->DefaultDragVisual = DragVisual;
    }
    
    // Start the drag operation
    DragDropOp->Pivot = EDragPivot::CenterCenter;
    
    // Broadcast the event
    OnSlotDragDetected.Broadcast(this);
    
    return FReply::Handled().BeginDragDrop(DragDropOp);
}

// Handle drop
bool UYourInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
    if (!ItemDragDrop)
    {
        return false;
    }
    
    // Get the source and destination containers
    UInventoryWidgetBase* ParentInventory = Cast<UInventoryWidgetBase>(GetParent());
    if (!ParentInventory)
    {
        return false;
    }
    
    IInventoryContainerInterface* DestContainer = ParentInventory->GetInventoryContainer().GetInterface();
    IInventoryContainerInterface* SourceContainer = ItemDragDrop->SourceContainer.GetInterface();
    
    if (!DestContainer || !SourceContainer)
    {
        return false;
    }
    
    // Handle the drop based on container type
    if (USlottedContainerComponent* SlottedContainer = Cast<USlottedContainerComponent>(DestContainer))
    {
        // Handle drop on a slotted container
        if (ItemDragDrop->bIsSplitOperation)
        {
            // Create a copy of the item with the split count
            FItemInstance SplitItem = ItemDragDrop->DraggedItem;
            SplitItem.StackCount = ItemDragDrop->SplitCount;
            
            // Remove from source
            FItemInstance RemovedItem;
            SourceContainer->RemoveItem(ItemDragDrop->DraggedItem.ItemGuid, ItemDragDrop->SplitCount, RemovedItem);
            
            // Add to destination slot
            int32 RemainingCount = 0;
            SlottedContainer->AddItemToSlot(SplitItem, SlotIndex, RemainingCount);
        }
        else
        {
            // Handle full stack move or swap
            // ...
        }
    }
    else
    {
        // Handle drop on a regular container
        // ...
    }
    
    // Broadcast the drop event
    UInventorySlotWidget* SourceSlot = nullptr;
    if (UGridInventoryWidget* GridWidget = Cast<UGridInventoryWidget>(ParentInventory))
    {
        SourceSlot = GridWidget->GetSlotWidgetAtIndex(ItemDragDrop->SourceSlotIndex);
    }
    
    if (SourceSlot)
    {
        OnSlotDrop.Broadcast(this, SourceSlot);
    }
    
    return true;
}
```

### Creating a Context Menu

```cpp
// In your InventorySlotWidget subclass

// Handle right-click
FReply UYourInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && !IsEmpty())
    {
        // Show context menu
        ShowContextMenu();
        return FReply::Handled();
    }
    
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

// Show context menu
void UYourInventorySlotWidget::ShowContextMenu()
{
    if (IsEmpty() || !ItemInstance.ItemData)
    {
        return;
    }
    
    // Create the context menu
    UYourContextMenuWidget* ContextMenu = CreateWidget<UYourContextMenuWidget>(GetOwningPlayer(), ContextMenuWidgetClass);
    if (!ContextMenu)
    {
        return;
    }
    
    // Add actions based on item type
    TArray<FName> Actions;
    
    // Common actions
    Actions.Add(FName("Examine"));
    Actions.Add(FName("Drop"));
    
    // Item-specific actions
    if (ItemInstance.ItemData->CanBeUsed())
    {
        Actions.Add(FName("Use"));
    }
    
    if (ItemInstance.ItemData->CanBeEquipped())
    {
        Actions.Add(FName("Equip"));
    }
    
    // Set up the context menu
    ContextMenu->SetActions(Actions);
    ContextMenu->SetItem(ItemInstance);
    
    // Subscribe to action selection
    ContextMenu->OnActionSelected.AddDynamic(this, &UYourInventorySlotWidget::OnContextMenuActionSelected);
    
    // Show the menu at mouse position
    FVector2D MousePosition = GetOwningPlayer()->GetMousePosition();
    ContextMenu->AddToViewport();
    ContextMenu->SetPositionInViewport(MousePosition, false);
}

// Handle context menu action selection
void UYourInventorySlotWidget::OnContextMenuActionSelected(const FItemInstance& Item, const FName& ActionName)
{
    // Get the parent inventory widget
    UInventoryWidgetBase* ParentInventory = Cast<UInventoryWidgetBase>(GetParent());
    if (ParentInventory)
    {
        // Broadcast the action
        ParentInventory->PerformItemAction(Item, ActionName);
    }
}
```

## AI Prompt Templates

### Creating Custom Inventory UI Components

```
Create a custom inventory UI component for [UI_COMPONENT_TYPE] in Unreal Engine 5 C++.
It should:
- Extend [BASE_WIDGET_CLASS]
- Support [FEATURE_1] and [FEATURE_2]
- Include visual feedback for [INTERACTION_TYPE]
- Handle [EDGE_CASE]
Include any necessary UI components, helper functions, and override appropriate widget methods.
```

### Implementing Specialized Item Displays

```
Implement a specialized item display widget for [ITEM_CATEGORY] items in Unreal Engine 5 C++ that:
- Shows [SPECIFIC_PROPERTY_1] and [SPECIFIC_PROPERTY_2]
- Provides visual feedback for [ITEM_STATE]
- Supports [INTERACTION_TYPE]
- Integrates with the existing inventory widget system
```

## Integration Tips

1. **Model-View Separation**: Keep a clean separation between inventory data and UI representation.

2. **Responsive Design**: Design UI components to scale properly across different screen sizes.

3. **Input Methods**: Support both mouse/keyboard and gamepad input methods.

4. **Visual Feedback**: Provide clear visual feedback for all user interactions.

5. **Localization**: Design with text localization in mind from the start.

## Performance Considerations

1. **Widget Pooling**: Use widget pooling for large inventories to reduce instantiation costs.

2. **Lazy Loading**: Only create detailed item tooltips when needed.

3. **Throttle Updates**: Throttle UI updates for rapidly changing inventory data.

4. **Texture Atlases**: Use texture atlases for item icons to reduce draw calls.

## Common Pitfalls

1. **Input Focus**: Properly manage input focus when showing/hiding inventory UI.

2. **Z-Order Issues**: Be careful with widget Z-order for tooltips and context menus.

3. **Gamepad Navigation**: Don't forget to implement proper gamepad navigation.

4. **UI Scale**: Test UI at different scales and resolutions to ensure readability.

5. **Drag-Drop Edge Cases**: Handle all edge cases in drag-drop operations (invalid drops, partial stacks, etc.). 