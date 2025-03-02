# Data-Binding Widget Pattern

## Problem
Creating UI widgets that automatically update when game data changes without manual refreshing and complex dependency management.

## Solution Pattern
Implement a data-binding system that connects UI elements to data models and updates the UI when the underlying data changes through a centralized notification system.

## Code Example

```cpp
// UDataBindingWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataBindingWidget.generated.h"

// Forward declarations
class UDataModelBase;

/**
 * Delegate that is triggered when a bound data field changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDataFieldChanged, const FString&, FieldName, const FString&, NewValueAsString);

/**
 * Base class for data models that will be bound to UI
 */
UCLASS(BlueprintType, Blueprintable)
class YOURGAME_API UDataModelBase : public UObject
{
    GENERATED_BODY()
    
public:
    UDataModelBase();
    
    // Event fired when any data changes
    UPROPERTY(BlueprintAssignable, Category = "UI|Data Binding")
    FOnDataFieldChanged OnDataFieldChanged;
    
    // Call this to notify binding system that a field has changed
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    void NotifyFieldChanged(const FString& FieldName);
    
    // Get a field value as string (implement in child classes)
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    virtual FString GetFieldValueAsString(const FString& FieldName) const;
    
    // Get a field value as text (for UI elements)
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    virtual FText GetFieldValueAsText(const FString& FieldName) const;
    
    // Check if a field exists
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    virtual bool HasField(const FString& FieldName) const;
    
protected:
    // Helper function to broadcast field changes
    virtual void BroadcastFieldChanged(const FString& FieldName);
};

/**
 * Widget that can bind to a data model and update when model changes
 */
UCLASS()
class YOURGAME_API UDataBindingWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UDataBindingWidget(const FObjectInitializer& ObjectInitializer);
    
    // Begin UUserWidget interface
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    // End UUserWidget interface
    
    // Set the data model this widget is bound to
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    void SetDataModel(UDataModelBase* InDataModel);
    
    // Get the bound data model
    UFUNCTION(BlueprintPure, Category = "UI|Data Binding")
    UDataModelBase* GetDataModel() const { return DataModel; }
    
    // Manually refresh all bindings
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    void RefreshAllBindings();
    
    // Bind a specific UI element to a data field
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    void BindWidgetToField(UWidget* Widget, const FString& FieldName);
    
    // Unbind a widget from all fields
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    void UnbindWidget(UWidget* Widget);
    
protected:
    // The data model this widget is bound to
    UPROPERTY(BlueprintReadOnly, Category = "UI|Data Binding")
    UDataModelBase* DataModel;
    
    // Map of field names to bound widgets
    UPROPERTY()
    TMap<FString, TArray<UWidget*>> FieldBindings;
    
    // Handler for data field changes
    UFUNCTION()
    void OnDataFieldChangedHandler(const FString& FieldName, const FString& NewValueAsString);
    
    // Apply the field value to a bound widget
    virtual void ApplyValueToWidget(UWidget* Widget, const FString& FieldName);
    
    // Auto-bind widgets with the same name as data fields
    UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
    void AutoBindWidgetsByName();
    
    // Blueprint event called when a bound field changes (for custom handling)
    UFUNCTION(BlueprintImplementableEvent, Category = "UI|Data Binding")
    void OnBindingValueChanged(const FString& FieldName, UWidget* BoundWidget);
};
```

### Implementation Example

```cpp
// UDataModelBase.cpp
#include "DataModelBase.h"

UDataModelBase::UDataModelBase()
{
    // Default initialization
}

void UDataModelBase::NotifyFieldChanged(const FString& FieldName)
{
    if (HasField(FieldName))
    {
        BroadcastFieldChanged(FieldName);
    }
}

FString UDataModelBase::GetFieldValueAsString(const FString& FieldName) const
{
    // Base implementation returns empty string
    // Override in child classes to provide actual values
    return FString();
}

FText UDataModelBase::GetFieldValueAsText(const FString& FieldName) const
{
    return FText::FromString(GetFieldValueAsString(FieldName));
}

bool UDataModelBase::HasField(const FString& FieldName) const
{
    // Base implementation returns false
    // Override in child classes to check if field exists
    return false;
}

void UDataModelBase::BroadcastFieldChanged(const FString& FieldName)
{
    FString ValueAsString = GetFieldValueAsString(FieldName);
    OnDataFieldChanged.Broadcast(FieldName, ValueAsString);
}

// UDataBindingWidget.cpp
#include "DataBindingWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/RichTextBlock.h"

UDataBindingWidget::UDataBindingWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Default initialization
}

void UDataBindingWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // If we have a data model, bind to it
    if (DataModel)
    {
        DataModel->OnDataFieldChanged.AddDynamic(this, &UDataBindingWidget::OnDataFieldChangedHandler);
        RefreshAllBindings();
    }
}

void UDataBindingWidget::NativeDestruct()
{
    // Clean up bindings when widget is destroyed
    if (DataModel)
    {
        DataModel->OnDataFieldChanged.RemoveDynamic(this, &UDataBindingWidget::OnDataFieldChangedHandler);
    }
    
    Super::NativeDestruct();
}

void UDataBindingWidget::SetDataModel(UDataModelBase* InDataModel)
{
    // If we already have a data model, unbind from it
    if (DataModel)
    {
        DataModel->OnDataFieldChanged.RemoveDynamic(this, &UDataBindingWidget::OnDataFieldChangedHandler);
    }
    
    // Set the new data model
    DataModel = InDataModel;
    
    // Bind to the new data model if it exists
    if (DataModel)
    {
        DataModel->OnDataFieldChanged.AddDynamic(this, &UDataBindingWidget::OnDataFieldChangedHandler);
        
        // Refresh all bindings with the new data
        RefreshAllBindings();
    }
}

void UDataBindingWidget::RefreshAllBindings()
{
    if (!DataModel)
    {
        return;
    }
    
    // Apply current values to all bound widgets
    for (const auto& Binding : FieldBindings)
    {
        const FString& FieldName = Binding.Key;
        
        for (UWidget* Widget : Binding.Value)
        {
            if (Widget && !Widget->IsA<UUserWidget>())
            {
                ApplyValueToWidget(Widget, FieldName);
            }
        }
    }
}

void UDataBindingWidget::BindWidgetToField(UWidget* Widget, const FString& FieldName)
{
    if (!Widget || FieldName.IsEmpty() || !DataModel || !DataModel->HasField(FieldName))
    {
        return;
    }
    
    // Add the widget to the bindings for this field
    if (!FieldBindings.Contains(FieldName))
    {
        FieldBindings.Add(FieldName, TArray<UWidget*>());
    }
    
    FieldBindings[FieldName].AddUnique(Widget);
    
    // Apply the current value to the widget
    ApplyValueToWidget(Widget, FieldName);
}

void UDataBindingWidget::UnbindWidget(UWidget* Widget)
{
    if (!Widget)
    {
        return;
    }
    
    // Remove the widget from all field bindings
    for (auto& Binding : FieldBindings)
    {
        Binding.Value.Remove(Widget);
    }
}

void UDataBindingWidget::OnDataFieldChangedHandler(const FString& FieldName, const FString& NewValueAsString)
{
    if (!FieldBindings.Contains(FieldName))
    {
        return;
    }
    
    // Update all widgets bound to this field
    for (UWidget* Widget : FieldBindings[FieldName])
    {
        if (Widget)
        {
            ApplyValueToWidget(Widget, FieldName);
            OnBindingValueChanged(FieldName, Widget);
        }
    }
}

void UDataBindingWidget::ApplyValueToWidget(UWidget* Widget, const FString& FieldName)
{
    if (!Widget || !DataModel)
    {
        return;
    }
    
    // Handle different widget types
    if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
    {
        TextBlock->SetText(DataModel->GetFieldValueAsText(FieldName));
    }
    else if (UProgressBar* ProgressBar = Cast<UProgressBar>(Widget))
    {
        // Convert string to float for progress bar
        float Value = FCString::Atof(*DataModel->GetFieldValueAsString(FieldName));
        ProgressBar->SetPercent(Value);
    }
    else if (URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget))
    {
        RichTextBlock->SetText(DataModel->GetFieldValueAsText(FieldName));
    }
    // Additional widget types can be handled here
}

void UDataBindingWidget::AutoBindWidgetsByName()
{
    if (!DataModel)
    {
        return;
    }
    
    // Get all widgets in the widget tree
    TArray<UWidget*> AllWidgets;
    WidgetTree->GetAllWidgets(AllWidgets);
    
    // For each widget, check if its name matches a field name
    for (UWidget* Widget : AllWidgets)
    {
        if (Widget)
        {
            FString WidgetName = Widget->GetName();
            
            // If the data model has a field with this name, bind it
            if (DataModel->HasField(WidgetName))
            {
                BindWidgetToField(Widget, WidgetName);
            }
        }
    }
}
```

### Example of a Concrete Data Model

```cpp
// UPlayerDataModel.h
#pragma once

#include "CoreMinimal.h"
#include "DataModelBase.h"
#include "PlayerDataModel.generated.h"

/**
 * Data model for player information
 */
UCLASS(BlueprintType)
class YOURGAME_API UPlayerDataModel : public UDataModelBase
{
    GENERATED_BODY()
    
public:
    UPlayerDataModel();
    
    // Player properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    FString PlayerName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    int32 PlayerLevel;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    float HealthCurrent;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    float HealthMax;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    float ExperiencePercent;
    
    // Setters that automatically notify UI
    UFUNCTION(BlueprintCallable, Category = "Player Data")
    void SetPlayerName(const FString& NewName);
    
    UFUNCTION(BlueprintCallable, Category = "Player Data")
    void SetPlayerLevel(int32 NewLevel);
    
    UFUNCTION(BlueprintCallable, Category = "Player Data")
    void SetHealth(float NewHealth);
    
    UFUNCTION(BlueprintCallable, Category = "Player Data")
    void SetHealthMax(float NewHealthMax);
    
    UFUNCTION(BlueprintCallable, Category = "Player Data")
    void SetExperiencePercent(float NewExpPercent);
    
    // Override required functions from base class
    virtual FString GetFieldValueAsString(const FString& FieldName) const override;
    virtual bool HasField(const FString& FieldName) const override;
};

// UPlayerDataModel.cpp
#include "PlayerDataModel.h"

UPlayerDataModel::UPlayerDataModel()
{
    // Default values
    PlayerName = "Player";
    PlayerLevel = 1;
    HealthCurrent = 100.0f;
    HealthMax = 100.0f;
    ExperiencePercent = 0.0f;
}

void UPlayerDataModel::SetPlayerName(const FString& NewName)
{
    PlayerName = NewName;
    NotifyFieldChanged("PlayerName");
}

void UPlayerDataModel::SetPlayerLevel(int32 NewLevel)
{
    PlayerLevel = NewLevel;
    NotifyFieldChanged("PlayerLevel");
}

void UPlayerDataModel::SetHealth(float NewHealth)
{
    HealthCurrent = FMath::Clamp(NewHealth, 0.0f, HealthMax);
    NotifyFieldChanged("HealthCurrent");
    
    // Also notify about health percentage which might be displayed
    NotifyFieldChanged("HealthPercent");
}

void UPlayerDataModel::SetHealthMax(float NewHealthMax)
{
    HealthMax = FMath::Max(0.0f, NewHealthMax);
    
    // Clamp current health to new max
    HealthCurrent = FMath::Clamp(HealthCurrent, 0.0f, HealthMax);
    
    NotifyFieldChanged("HealthMax");
    NotifyFieldChanged("HealthCurrent");
    NotifyFieldChanged("HealthPercent");
}

void UPlayerDataModel::SetExperiencePercent(float NewExpPercent)
{
    ExperiencePercent = FMath::Clamp(NewExpPercent, 0.0f, 1.0f);
    NotifyFieldChanged("ExperiencePercent");
}

FString UPlayerDataModel::GetFieldValueAsString(const FString& FieldName) const
{
    // Handle all the fields we support
    if (FieldName == "PlayerName")
    {
        return PlayerName;
    }
    else if (FieldName == "PlayerLevel")
    {
        return FString::FromInt(PlayerLevel);
    }
    else if (FieldName == "HealthCurrent")
    {
        return FString::SanitizeFloat(HealthCurrent);
    }
    else if (FieldName == "HealthMax")
    {
        return FString::SanitizeFloat(HealthMax);
    }
    else if (FieldName == "HealthPercent")
    {
        // Calculate health percentage
        float HealthPercent = (HealthMax > 0.0f) ? (HealthCurrent / HealthMax) : 0.0f;
        return FString::SanitizeFloat(HealthPercent);
    }
    else if (FieldName == "ExperiencePercent")
    {
        return FString::SanitizeFloat(ExperiencePercent);
    }
    
    // Field not found
    return Super::GetFieldValueAsString(FieldName);
}

bool UPlayerDataModel::HasField(const FString& FieldName) const
{
    return FieldName == "PlayerName" ||
           FieldName == "PlayerLevel" ||
           FieldName == "HealthCurrent" ||
           FieldName == "HealthMax" ||
           FieldName == "HealthPercent" ||
           FieldName == "ExperiencePercent";
}
```

### Example Usage

Here's how to use the data binding system in a player HUD:

```cpp
// UPlayerHUDWidget.h
#pragma once

#include "CoreMinimal.h"
#include "DataBindingWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "PlayerHUDWidget.generated.h"

class UPlayerDataModel;

UCLASS()
class YOURGAME_API UPlayerHUDWidget : public UDataBindingWidget
{
    GENERATED_BODY()
    
public:
    UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer);
    
    virtual void NativeConstruct() override;
    
    // Update HUD with latest player data
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdatePlayerData(const class APlayerCharacter* PlayerCharacter);
    
protected:
    // UI Components bound to data model
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerName;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerLevel;
    
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthText;
    
    UPROPERTY(meta = (BindWidget))
    UProgressBar* ExperienceBar;
    
    // Player data model
    UPROPERTY()
    UPlayerDataModel* PlayerData;
    
    // Create data bindings
    void SetupBindings();
};

// UPlayerHUDWidget.cpp
#include "PlayerHUDWidget.h"
#include "PlayerDataModel.h"
#include "PlayerCharacter.h"

UPlayerHUDWidget::UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Create player data model
    PlayerData = CreateDefaultSubobject<UPlayerDataModel>(TEXT("PlayerData"));
}

void UPlayerHUDWidget::NativeConstruct()
{
    // Set this widget's data model before calling super (which will refresh bindings)
    SetDataModel(PlayerData);
    
    Super::NativeConstruct();
    
    // Set up manual bindings for widgets that need custom formatting or different field names
    SetupBindings();
}

void UPlayerHUDWidget::SetupBindings()
{
    // Bind widgets to data fields
    if (PlayerName)
    {
        BindWidgetToField(PlayerName, "PlayerName");
    }
    
    if (PlayerLevel)
    {
        BindWidgetToField(PlayerLevel, "PlayerLevel");
    }
    
    if (HealthBar)
    {
        BindWidgetToField(HealthBar, "HealthPercent");
    }
    
    if (HealthText)
    {
        // We'll handle this one specially in OnBindingValueChanged
        BindWidgetToField(HealthText, "HealthCurrent");
    }
    
    if (ExperienceBar)
    {
        BindWidgetToField(ExperienceBar, "ExperiencePercent");
    }
}

void UPlayerHUDWidget::UpdatePlayerData(const APlayerCharacter* PlayerCharacter)
{
    if (!PlayerCharacter || !PlayerData)
    {
        return;
    }
    
    // Update our data model with the latest player data
    // This will automatically update the UI through the bindings
    PlayerData->SetPlayerName(PlayerCharacter->GetPlayerName());
    PlayerData->SetPlayerLevel(PlayerCharacter->GetLevel());
    PlayerData->SetHealthMax(PlayerCharacter->GetMaxHealth());
    PlayerData->SetHealth(PlayerCharacter->GetCurrentHealth());
    PlayerData->SetExperiencePercent(PlayerCharacter->GetExperiencePercent());
}

// Override the blueprint implementable event with a C++ implementation
void UPlayerHUDWidget::OnBindingValueChanged(const FString& FieldName, UWidget* BoundWidget)
{
    Super::OnBindingValueChanged(FieldName, BoundWidget);
    
    // Custom formatting for health text
    if (FieldName == "HealthCurrent" && BoundWidget == HealthText && PlayerData)
    {
        // Format as "Current / Max"
        FString HealthString = FString::SanitizeFloat(PlayerData->HealthCurrent) + " / " + 
                              FString::SanitizeFloat(PlayerData->HealthMax);
        HealthText->SetText(FText::FromString(HealthString));
    }
}
```

## AI Prompt Template

```
I need to implement a data-binding system for UI widgets in UE5 that will automatically update UI elements when the underlying data changes. This should follow the Model-View pattern.

Please provide C++ code for:

1. A base data model class (UDataModelBase) that:
   - Defines a notification system for when data changes
   - Provides methods for getting field values as strings/text
   - Has a flexible field system to support different types of data

2. A data-binding widget class (UDataBindingWidget) that:
   - Inherits from UUserWidget
   - Connects to a data model
   - Maps UI widgets to data model fields
   - Updates UI elements automatically when bound fields change
   - Supports common UMG widgets like TextBlock, ProgressBar, etc.

3. A sample implementation of a specific data model (UPlayerDataModel) with:
   - Properties for player name, health, level, etc.
   - Setter methods that automatically notify the UI
   - Proper implementation of the base class methods

4. A usage example showing how to:
   - Create a player HUD that uses the data-binding system
   - Bind UI elements to data fields
   - Update the data model from gameplay code

Please focus on making this system flexible, efficient, and easy to use for both programmers and designers.
```

## Performance Considerations
- Only update UI elements when their bound data actually changes
- Batch updates for rapid data changes rather than updating UI immediately
- Consider using a dirty flag system for complex updates
- Use string caching for values that don't change frequently
- Implement view recycling for list widgets with many bound data items

## Integration Tips
- Create a UI manager to handle data model creation and ownership
- Use a factory pattern to create common data models
- Implement data formatters for complex data representation
- Connect with game subsystems to automatically receive updates
- Consider using weak references to avoid circular dependencies

## Advanced Implementation

For more complex applications, consider these extensions:

### 1. Two-Way Data Binding
Allow UI changes to update the data model:

```cpp
// In UDataBindingWidget.h
// Bind a widget to a field with two-way binding
UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
void BindWidgetToFieldTwoWay(UWidget* Widget, const FString& FieldName);

// In UDataBindingWidget.cpp implementation for EditableTextBox
void UDataBindingWidget::SetupTwoWayBinding(UEditableTextBox* TextBox, const FString& FieldName)
{
    // Bind the widget to update when model changes (one-way)
    BindWidgetToField(TextBox, FieldName);
    
    // Bind the widget's text changed event to update the model (the other way)
    FScriptDelegate Delegate;
    Delegate.BindUFunction(this, FName("OnTextBoxChanged"));
    TextBox->OnTextChanged.Add(Delegate);
    
    // Store which field this widget is editing
    TextBoxFieldMap.Add(TextBox, FieldName);
}

// Event handler for text box changes
UFUNCTION()
void UDataBindingWidget::OnTextBoxChanged(const FText& Text)
{
    if (!DataModel)
        return;
    
    UEditableTextBox* TextBox = Cast<UEditableTextBox>(UWidget::GetWidgetFromName(TEXT("TextBoxBound")));
    if (TextBox && TextBoxFieldMap.Contains(TextBox))
    {
        FString FieldName = TextBoxFieldMap[TextBox];
        // Update the model, implementation depends on your data model
        DataModel->SetFieldValueFromString(FieldName, Text.ToString());
    }
}
```

### 2. Collection Data Binding
Support binding lists of data to container widgets:

```cpp
// Add to UDataModelBase.h
// Get collection size
UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
virtual int32 GetCollectionSize(const FString& CollectionName) const;

// Get item at index
UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
virtual UDataModelBase* GetCollectionItemAt(const FString& CollectionName, int32 Index) const;

// Add to UDataBindingWidget.h
// Bind a list widget to a collection
UFUNCTION(BlueprintCallable, Category = "UI|Data Binding")
void BindListToCollection(UListView* ListView, const FString& CollectionName, 
                         TSubclassOf<UUserWidget> ItemWidgetClass);

// Handler for collection changes
UFUNCTION()
void OnCollectionChanged(const FString& CollectionName);
```

### 3. Data Formatters
Add support for custom formatting:

```cpp
// Create a formatter interface
UINTERFACE(MinimalAPI, Blueprintable)
class UDataFormatter : public UInterface
{
    GENERATED_BODY()
};

class IDataFormatter
{
    GENERATED_BODY()
    
public:
    // Format a value for display
    virtual FText Format(const FString& Value) const = 0;
};

// Add formatter support to bindings
void UDataBindingWidget::BindWidgetToFieldFormatted(UWidget* Widget, const FString& FieldName, 
                                                  TScriptInterface<IDataFormatter> Formatter)
{
    // Store formatter with binding
}