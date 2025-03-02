# Pattern 4: Save State Management

## Problem Statement
Games need a robust system to manage multiple save states, handle auto-saves, quick saves, and provide save state validation and recovery mechanisms. The system must also manage save slots, save metadata, and provide a clean interface for save state operations.

## Solution Overview
Implement a comprehensive save state management system that handles save slots, auto-saves, save metadata, and provides validation and recovery mechanisms.

## Implementation

### Save State Manager
```cpp
// SaveStateManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveStateManager.generated.h"

USTRUCT(BlueprintType)
struct FSaveMetadata
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    FString SlotName;

    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    FDateTime SaveTime;

    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    int32 Version;

    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    float PlayTime;

    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    FString LevelName;

    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    bool bIsAutoSave;

    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    bool bIsQuickSave;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutoSaveTriggered, const FString&, SlotName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveStateChanged, const FSaveMetadata&, SaveMetadata);

UCLASS()
class YOURGAME_API USaveStateManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    USaveStateManager();

    // Initialize and shutdown
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Save slot management
    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    bool CreateSaveSlot(const FString& SlotName, const FString& Description);

    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    bool DeleteSaveSlot(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    TArray<FSaveMetadata> GetAllSaveSlots() const;

    // Auto-save functionality
    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    void EnableAutoSave(float IntervalInSeconds = 300.0f);

    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    void DisableAutoSave();

    // Quick save functionality
    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    bool QuickSave();

    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    bool QuickLoad();

    // Save state validation
    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    bool ValidateSaveState(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    bool RepairSaveState(const FString& SlotName);

    // Save metadata
    UFUNCTION(BlueprintPure, Category = "SaveSystem")
    FSaveMetadata GetSaveMetadata(const FString& SlotName) const;

    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    bool UpdateSaveMetadata(const FString& SlotName, const FSaveMetadata& NewMetadata);

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "SaveSystem")
    FOnAutoSaveTriggered OnAutoSaveTriggered;

    UPROPERTY(BlueprintAssignable, Category = "SaveSystem")
    FOnSaveStateChanged OnSaveStateChanged;

protected:
    // Auto-save implementation
    void TriggerAutoSave();
    void ScheduleNextAutoSave();

    // Save state validation helpers
    bool CheckSaveStateIntegrity(const FString& SlotName);
    bool BackupSaveState(const FString& SlotName);
    bool RestoreFromBackup(const FString& SlotName);

private:
    // Auto-save properties
    bool bAutoSaveEnabled;
    float AutoSaveInterval;
    FTimerHandle AutoSaveTimerHandle;

    // Quick save properties
    FString LastQuickSaveSlot;
    static const FString QuickSavePrefix;
    static const int32 MaxQuickSaves;

    // Save slot management
    TMap<FString, FSaveMetadata> SaveSlotCache;
    FCriticalSection SaveSlotLock;

    // Helper functions
    FString GenerateUniqueSlotName() const;
    void UpdateSaveSlotCache();
    bool IsSaveSlotValid(const FString& SlotName) const;
};
```

### Implementation File
```cpp
// SaveStateManager.cpp
#include "SaveStateManager.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

const FString USaveStateManager::QuickSavePrefix = TEXT("QuickSave_");
const int32 USaveStateManager::MaxQuickSaves = 5;

USaveStateManager::USaveStateManager()
    : bAutoSaveEnabled(false)
    , AutoSaveInterval(300.0f)
{
}

void USaveStateManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UpdateSaveSlotCache();
}

bool USaveStateManager::CreateSaveSlot(const FString& SlotName, const FString& Description)
{
    if (IsSaveSlotValid(SlotName))
    {
        return false;
    }

    FSaveMetadata Metadata;
    Metadata.SlotName = SlotName;
    Metadata.SaveTime = FDateTime::Now();
    Metadata.Description = Description;
    Metadata.Version = 1;
    Metadata.PlayTime = 0.0f;
    Metadata.LevelName = UGameplayStatics::GetCurrentLevelName(this);
    Metadata.bIsAutoSave = false;
    Metadata.bIsQuickSave = false;

    FScopeLock Lock(&SaveSlotLock);
    SaveSlotCache.Add(SlotName, Metadata);
    OnSaveStateChanged.Broadcast(Metadata);

    return true;
}

void USaveStateManager::EnableAutoSave(float IntervalInSeconds)
{
    bAutoSaveEnabled = true;
    AutoSaveInterval = IntervalInSeconds;
    ScheduleNextAutoSave();
}

void USaveStateManager::DisableAutoSave()
{
    bAutoSaveEnabled = false;
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(AutoSaveTimerHandle);
    }
}

bool USaveStateManager::QuickSave()
{
    FString SlotName = GenerateUniqueSlotName();
    if (CreateSaveSlot(SlotName, TEXT("Quick Save")))
    {
        FSaveMetadata& Metadata = SaveSlotCache[SlotName];
        Metadata.bIsQuickSave = true;
        LastQuickSaveSlot = SlotName;

        // Maintain max quick saves
        TArray<FString> QuickSaveSlots;
        for (const auto& Pair : SaveSlotCache)
        {
            if (Pair.Value.bIsQuickSave)
            {
                QuickSaveSlots.Add(Pair.Key);
            }
        }

        if (QuickSaveSlots.Num() > MaxQuickSaves)
        {
            QuickSaveSlots.Sort([](const FString& A, const FString& B) { return A > B; });
            for (int32 i = MaxQuickSaves; i < QuickSaveSlots.Num(); ++i)
            {
                DeleteSaveSlot(QuickSaveSlots[i]);
            }
        }

        return true;
    }
    return false;
}

bool USaveStateManager::QuickLoad()
{
    if (!LastQuickSaveSlot.IsEmpty() && IsSaveSlotValid(LastQuickSaveSlot))
    {
        // Implement actual load logic here
        return true;
    }
    return false;
}

bool USaveStateManager::ValidateSaveState(const FString& SlotName)
{
    if (!IsSaveSlotValid(SlotName))
    {
        return false;
    }

    return CheckSaveStateIntegrity(SlotName);
}

bool USaveStateManager::RepairSaveState(const FString& SlotName)
{
    if (!IsSaveSlotValid(SlotName))
    {
        return false;
    }

    if (BackupSaveState(SlotName))
    {
        if (RestoreFromBackup(SlotName))
        {
            return true;
        }
    }
    return false;
}

void USaveStateManager::TriggerAutoSave()
{
    if (!bAutoSaveEnabled)
    {
        return;
    }

    FString AutoSaveSlot = FString::Printf(TEXT("AutoSave_%s"), *FDateTime::Now().ToString());
    if (CreateSaveSlot(AutoSaveSlot, TEXT("Auto Save")))
    {
        FSaveMetadata& Metadata = SaveSlotCache[AutoSaveSlot];
        Metadata.bIsAutoSave = true;
        OnAutoSaveTriggered.Broadcast(AutoSaveSlot);
    }

    ScheduleNextAutoSave();
}

void USaveStateManager::ScheduleNextAutoSave()
{
    if (bAutoSaveEnabled && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoSaveTimerHandle,
            this,
            &USaveStateManager::TriggerAutoSave,
            AutoSaveInterval,
            false
        );
    }
}

FString USaveStateManager::GenerateUniqueSlotName() const
{
    return FString::Printf(TEXT("%s%s"),
        *QuickSavePrefix,
        *FDateTime::Now().ToString());
}

void USaveStateManager::UpdateSaveSlotCache()
{
    FScopeLock Lock(&SaveSlotLock);
    SaveSlotCache.Empty();

    TArray<FString> SaveSlots;
    if (UGameplayStatics::GetSaveGameSlotNames(SaveSlots))
    {
        for (const FString& SlotName : SaveSlots)
        {
            if (USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0))
            {
                // Implement metadata extraction from save game
                FSaveMetadata Metadata;
                // Fill metadata from save game
                SaveSlotCache.Add(SlotName, Metadata);
            }
        }
    }
}

bool USaveStateManager::IsSaveSlotValid(const FString& SlotName) const
{
    return !SlotName.IsEmpty() && UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}
```

### Usage Example
```cpp
// GameMode implementation
void AYourGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    // Get save state manager
    USaveStateManager* SaveManager = GetGameInstance()->GetSubsystem<USaveStateManager>();
    if (SaveManager)
    {
        // Enable auto-save every 5 minutes
        SaveManager->EnableAutoSave(300.0f);

        // Bind to auto-save events
        SaveManager->OnAutoSaveTriggered.AddDynamic(this, &AYourGameMode::HandleAutoSave);
        SaveManager->OnSaveStateChanged.AddDynamic(this, &AYourGameMode::HandleSaveStateChanged);
    }
}

void AYourGameMode::HandleAutoSave(const FString& SlotName)
{
    // Show auto-save notification
    ShowNotification(TEXT("Auto-saving..."));
}

void AYourGameMode::HandleSaveStateChanged(const FSaveMetadata& SaveMetadata)
{
    // Update save game UI
    UpdateSaveGameUI();
}

void AYourGameMode::QuickSaveRequested()
{
    USaveStateManager* SaveManager = GetGameInstance()->GetSubsystem<USaveStateManager>();
    if (SaveManager)
    {
        if (SaveManager->QuickSave())
        {
            ShowNotification(TEXT("Quick save completed"));
        }
        else
        {
            ShowNotification(TEXT("Quick save failed"), true);
        }
    }
}
```

## Best Practices

1. **Save Slot Management**
   - Use unique identifiers for save slots
   - Implement save slot limits
   - Clean up old auto-saves

2. **Auto-Save Strategy**
   - Choose appropriate intervals
   - Consider game state for timing
   - Avoid auto-saving during critical moments

3. **Data Integrity**
   - Implement checksums
   - Create backup saves
   - Validate save data

4. **User Experience**
   - Provide clear feedback
   - Show save progress
   - Handle errors gracefully

## Integration Tips

1. Create a save game UI system
2. Implement save thumbnails
3. Add save state compression
4. Create save state migration tools
5. Implement cloud save integration

## Common Pitfalls

1. Not handling corrupted saves
2. Insufficient error feedback
3. Poor auto-save timing
4. Memory leaks in save management
5. Race conditions in save operations

## Performance Considerations

1. **Save Slot Management**
   - Efficient slot organization
   - Proper cleanup of old saves
   - Optimized metadata storage

2. **Auto-Save Impact**
   - Minimize performance impact
   - Choose appropriate intervals
   - Handle large save states

3. **Memory Usage**
   - Efficient metadata caching
   - Proper resource cleanup
   - Optimized save state storage

## Related Patterns

- [Pattern 1: Save Data Architecture](Pattern1_SaveDataArchitecture.md)
- [Pattern 2: Serialization Framework](Pattern2_SerializationFramework.md)
- [Pattern 3: Async Save System](Pattern3_AsyncSaveSystem.md)
- [Pattern 5: Cross-Platform Persistence](Pattern5_CrossPlatformPersistence.md) 