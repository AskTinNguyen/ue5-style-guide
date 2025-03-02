# Pattern 3: Async Save System

## Problem Statement
Saving and loading game data can be time-consuming operations that may cause frame rate drops and poor user experience when performed on the game thread. An asynchronous save system is needed to handle these operations without blocking gameplay.

## Solution Overview
Implement an asynchronous save system using UE5's task graph system and async file I/O operations, with progress tracking and error handling capabilities.

## Implementation

### Async Save Manager
```cpp
// AsyncSaveManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncSaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSaveProgress, float, Progress, const FString&, Description);

UENUM()
enum class ESaveOperationState : uint8
{
    Idle,
    Saving,
    Loading,
    Error
};

UCLASS()
class YOURGAME_API UAsyncSaveManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UAsyncSaveManager();

    // Initialize and shutdown
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Async save operations
    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    void SaveGameAsync(const FString& SlotName, const FString& Description);

    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    void LoadGameAsync(const FString& SlotName);

    // Cancel current operation
    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    void CancelCurrentOperation();

    // Delegates for progress and completion
    UPROPERTY(BlueprintAssignable, Category = "SaveSystem")
    FOnSaveComplete OnSaveComplete;

    UPROPERTY(BlueprintAssignable, Category = "SaveSystem")
    FOnLoadComplete OnLoadComplete;

    UPROPERTY(BlueprintAssignable, Category = "SaveSystem")
    FOnSaveProgress OnSaveProgress;

    // State queries
    UFUNCTION(BlueprintPure, Category = "SaveSystem")
    ESaveOperationState GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category = "SaveSystem")
    float GetCurrentProgress() const { return CurrentProgress; }

protected:
    // Internal save/load implementation
    void ExecuteSaveOperation(const FString& SlotName, const FString& Description);
    void ExecuteLoadOperation(const FString& SlotName);

    // Progress tracking
    void UpdateProgress(float Progress, const FString& Description);

private:
    ESaveOperationState CurrentState;
    float CurrentProgress;
    FString CurrentSlotName;
    FCriticalSection StateLock;

    // Async task handles
    FGraphEventRef CurrentTask;
    FAsyncTask<class FSaveGameTask>* CurrentAsyncTask;

    // Helper functions
    void ResetState();
    bool IsOperationInProgress() const;
};
```

### Async Save Task
```cpp
// SaveGameTask.h
#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"

class FSaveGameTask : public FNonAbandonableTask
{
public:
    FSaveGameTask(const FString& InSlotName, const FString& InDescription, bool bInIsSave);

    // Required by UE5 async task system
    FORCEINLINE TStatId GetStatId() const
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(FSaveGameTask, STATGROUP_ThreadPoolAsyncTasks);
    }

    void DoWork();
    
    // Progress callback
    TFunction<void(float, const FString&)> ProgressCallback;

private:
    FString SlotName;
    FString Description;
    bool bIsSave;

    // Internal helper functions
    bool SaveGameData();
    bool LoadGameData();
    void UpdateProgress(float Progress, const FString& Status);
};
```

### Implementation File
```cpp
// AsyncSaveManager.cpp
#include "AsyncSaveManager.h"
#include "SaveGameTask.h"
#include "Async/AsyncFileHandle.h"

UAsyncSaveManager::UAsyncSaveManager()
    : CurrentState(ESaveOperationState::Idle)
    , CurrentProgress(0.0f)
    , CurrentAsyncTask(nullptr)
{
}

void UAsyncSaveManager::SaveGameAsync(const FString& SlotName, const FString& Description)
{
    if (IsOperationInProgress())
    {
        UE_LOG(LogTemp, Warning, TEXT("Save operation already in progress"));
        return;
    }

    FScopeLock Lock(&StateLock);
    CurrentState = ESaveOperationState::Saving;
    CurrentSlotName = SlotName;
    CurrentProgress = 0.0f;

    // Create and launch async task
    CurrentAsyncTask = new FAsyncTask<FSaveGameTask>(SlotName, Description, true);
    CurrentAsyncTask->GetTask().ProgressCallback = [this](float Progress, const FString& Description)
    {
        AsyncTask(ENamedThreads::GameThread, [this, Progress, Description]()
        {
            UpdateProgress(Progress, Description);
        });
    };

    CurrentAsyncTask->StartBackgroundTask();

    // Setup completion callback
    CurrentTask = FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
    {
        bool bSuccess = CurrentAsyncTask->GetTask().DoWork();
        AsyncTask(ENamedThreads::GameThread, [this, bSuccess]()
        {
            OnSaveComplete.Broadcast(bSuccess);
            ResetState();
        });
    }, TStatId(), nullptr, ENamedThreads::AnyBackgroundThreadNormalTask);
}

void UAsyncSaveManager::LoadGameAsync(const FString& SlotName)
{
    if (IsOperationInProgress())
    {
        UE_LOG(LogTemp, Warning, TEXT("Load operation already in progress"));
        return;
    }

    FScopeLock Lock(&StateLock);
    CurrentState = ESaveOperationState::Loading;
    CurrentSlotName = SlotName;
    CurrentProgress = 0.0f;

    // Create and launch async task
    CurrentAsyncTask = new FAsyncTask<FSaveGameTask>(SlotName, TEXT(""), false);
    CurrentAsyncTask->GetTask().ProgressCallback = [this](float Progress, const FString& Description)
    {
        AsyncTask(ENamedThreads::GameThread, [this, Progress, Description]()
        {
            UpdateProgress(Progress, Description);
        });
    };

    CurrentAsyncTask->StartBackgroundTask();

    // Setup completion callback
    CurrentTask = FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
    {
        bool bSuccess = CurrentAsyncTask->GetTask().DoWork();
        AsyncTask(ENamedThreads::GameThread, [this, bSuccess]()
        {
            OnLoadComplete.Broadcast(bSuccess);
            ResetState();
        });
    }, TStatId(), nullptr, ENamedThreads::AnyBackgroundThreadNormalTask);
}

void UAsyncSaveManager::CancelCurrentOperation()
{
    if (CurrentAsyncTask)
    {
        CurrentAsyncTask->EnsureCompletion();
        delete CurrentAsyncTask;
        CurrentAsyncTask = nullptr;
    }

    ResetState();
}

void UAsyncSaveManager::UpdateProgress(float Progress, const FString& Description)
{
    CurrentProgress = Progress;
    OnSaveProgress.Broadcast(Progress, Description);
}

void UAsyncSaveManager::ResetState()
{
    FScopeLock Lock(&StateLock);
    CurrentState = ESaveOperationState::Idle;
    CurrentProgress = 0.0f;
    CurrentSlotName.Empty();

    if (CurrentAsyncTask)
    {
        delete CurrentAsyncTask;
        CurrentAsyncTask = nullptr;
    }
}

bool UAsyncSaveManager::IsOperationInProgress() const
{
    return CurrentState != ESaveOperationState::Idle;
}
```

### Usage Example
```cpp
// GameMode implementation
void AYourGameMode::InitiateAsyncSave()
{
    UAsyncSaveManager* SaveManager = GetGameInstance()->GetSubsystem<UAsyncSaveManager>();
    if (SaveManager)
    {
        // Bind to delegates
        SaveManager->OnSaveComplete.AddDynamic(this, &AYourGameMode::HandleSaveComplete);
        SaveManager->OnSaveProgress.AddDynamic(this, &AYourGameMode::HandleSaveProgress);

        // Start async save
        SaveManager->SaveGameAsync(TEXT("SaveSlot1"), TEXT("Quick Save"));
    }
}

void AYourGameMode::HandleSaveComplete(bool bSuccess)
{
    if (bSuccess)
    {
        // Show success notification
        ShowNotification(TEXT("Game saved successfully"));
    }
    else
    {
        // Show error notification
        ShowNotification(TEXT("Failed to save game"));
    }
}

void AYourGameMode::HandleSaveProgress(float Progress, const FString& Description)
{
    // Update save progress UI
    UpdateSaveProgressUI(Progress, Description);
}
```

## Best Practices

1. **Thread Safety**
   - Use proper synchronization mechanisms
   - Avoid data races
   - Handle concurrent operations safely

2. **Progress Tracking**
   - Provide meaningful progress updates
   - Use appropriate progress granularity
   - Handle progress UI updates on game thread

3. **Error Handling**
   - Provide detailed error information
   - Handle cancellation gracefully
   - Clean up resources properly

4. **Memory Management**
   - Clean up async tasks properly
   - Handle large save files efficiently
   - Manage temporary buffers

## Integration Tips

1. Create a UI widget for save progress
2. Implement auto-save using the async system
3. Add save verification after completion
4. Implement save compression in the background
5. Add save corruption recovery

## Common Pitfalls

1. Not handling thread safety properly
2. Blocking the game thread unnecessarily
3. Memory leaks in async operations
4. Not cleaning up resources
5. Poor error handling

## Performance Considerations

1. **Threading**
   - Use appropriate thread priority
   - Avoid thread contention
   - Balance thread usage

2. **Memory**
   - Manage buffer sizes
   - Handle large saves efficiently
   - Clean up temporary data

3. **I/O Operations**
   - Use appropriate chunk sizes
   - Handle disk operations efficiently
   - Consider platform limitations

## Related Patterns

- [Pattern 1: Save Data Architecture](Pattern1_SaveDataArchitecture.md)
- [Pattern 2: Serialization Framework](Pattern2_SerializationFramework.md)
- [Pattern 4: Save State Management](Pattern4_SaveStateManagement.md)
- [Pattern 5: Cross-Platform Persistence](Pattern5_CrossPlatformPersistence.md) 