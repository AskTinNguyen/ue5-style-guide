# Pattern 5: Cross-Platform Persistence

## Problem Statement
Modern games often need to support save data persistence across multiple platforms, including cloud saves, while handling platform-specific storage locations, data formats, and synchronization conflicts. The system must provide a consistent interface while adapting to platform-specific requirements.

## Solution Overview
Implement a platform-agnostic persistence layer that handles cross-platform save data storage, cloud synchronization, and conflict resolution while providing a unified interface for game systems.

## Implementation

### Platform-Agnostic Save Interface
```cpp
// IPlatformPersistence.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPlatformPersistence.generated.h"

UENUM(BlueprintType)
enum class ESaveDataLocation : uint8
{
    Local,
    Cloud,
    Both
};

USTRUCT(BlueprintType)
struct FSaveDataInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "SaveData")
    FString SlotName;

    UPROPERTY(BlueprintReadOnly, Category = "SaveData")
    int64 FileSize;

    UPROPERTY(BlueprintReadOnly, Category = "SaveData")
    FDateTime LastModified;

    UPROPERTY(BlueprintReadOnly, Category = "SaveData")
    ESaveDataLocation Location;

    UPROPERTY(BlueprintReadOnly, Category = "SaveData")
    bool bIsCloudSynced;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UPlatformPersistenceInterface : public UInterface
{
    GENERATED_BODY()
};

class IPlatformPersistenceInterface
{
    GENERATED_BODY()

public:
    // Save operations
    virtual bool SaveData(const FString& SlotName, const TArray<uint8>& Data, ESaveDataLocation Location) = 0;
    virtual bool LoadData(const FString& SlotName, TArray<uint8>& OutData, ESaveDataLocation Location) = 0;
    virtual bool DeleteData(const FString& SlotName, ESaveDataLocation Location) = 0;

    // Cloud operations
    virtual bool SyncCloudData() = 0;
    virtual bool IsCloudAvailable() const = 0;
    virtual void EnableCloudSync(bool bEnable) = 0;

    // Information queries
    virtual TArray<FSaveDataInfo> GetAllSaveDataInfo() const = 0;
    virtual bool DoesSaveExist(const FString& SlotName, ESaveDataLocation Location) const = 0;
    virtual int64 GetAvailableSpace(ESaveDataLocation Location) const = 0;
};
```

### Platform-Specific Implementation
```cpp
// PlatformPersistenceBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "IPlatformPersistence.h"
#include "PlatformPersistenceBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloudSyncComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSaveDataConflict, const FString&, SlotName, const TArray<FSaveDataInfo>&, Versions);

UCLASS()
class YOURGAME_API UPlatformPersistenceBase : public UGameInstanceSubsystem,
    public IPlatformPersistenceInterface
{
    GENERATED_BODY()

public:
    UPlatformPersistenceBase();

    // IPlatformPersistenceInterface implementation
    virtual bool SaveData(const FString& SlotName, const TArray<uint8>& Data, ESaveDataLocation Location) override;
    virtual bool LoadData(const FString& SlotName, TArray<uint8>& OutData, ESaveDataLocation Location) override;
    virtual bool DeleteData(const FString& SlotName, ESaveDataLocation Location) override;
    virtual bool SyncCloudData() override;
    virtual bool IsCloudAvailable() const override;
    virtual void EnableCloudSync(bool bEnable) override;
    virtual TArray<FSaveDataInfo> GetAllSaveDataInfo() const override;
    virtual bool DoesSaveExist(const FString& SlotName, ESaveDataLocation Location) const override;
    virtual int64 GetAvailableSpace(ESaveDataLocation Location) const override;

    // Cloud sync events
    UPROPERTY(BlueprintAssignable, Category = "SaveSystem|Cloud")
    FOnCloudSyncComplete OnCloudSyncComplete;

    UPROPERTY(BlueprintAssignable, Category = "SaveSystem|Cloud")
    FOnSaveDataConflict OnSaveDataConflict;

protected:
    // Platform-specific implementations
    virtual FString GetLocalSavePath() const;
    virtual bool SaveToLocal(const FString& SlotName, const TArray<uint8>& Data);
    virtual bool LoadFromLocal(const FString& SlotName, TArray<uint8>& OutData);
    virtual bool SaveToCloud(const FString& SlotName, const TArray<uint8>& Data);
    virtual bool LoadFromCloud(const FString& SlotName, TArray<uint8>& OutData);
    virtual void HandleCloudConflict(const FString& SlotName, const TArray<FSaveDataInfo>& Versions);

private:
    bool bCloudSyncEnabled;
    FCriticalSection DataAccessLock;
    TMap<FString, FSaveDataInfo> SaveDataCache;

    // Helper functions
    void UpdateSaveDataCache();
    bool ValidateData(const TArray<uint8>& Data) const;
    void MergeCloudData(const FString& SlotName, const TArray<uint8>& LocalData, const TArray<uint8>& CloudData);
};
```

### Platform-Specific Implementation (Steam Example)
```cpp
// SteamPersistenceSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "PlatformPersistenceBase.h"
#include "SteamPersistenceSubsystem.generated.h"

UCLASS()
class YOURGAME_API USteamPersistenceSubsystem : public UPlatformPersistenceBase
{
    GENERATED_BODY()

public:
    USteamPersistenceSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

protected:
    virtual FString GetLocalSavePath() const override;
    virtual bool SaveToCloud(const FString& SlotName, const TArray<uint8>& Data) override;
    virtual bool LoadFromCloud(const FString& SlotName, TArray<uint8>& OutData) override;
    virtual bool IsCloudAvailable() const override;

private:
    bool InitializeSteamCloud();
    void HandleSteamCloudCallback(bool bSuccess, const FString& SlotName);
    
    // Steam-specific members
    bool bSteamCloudInitialized;
    FString SteamCloudPath;
};

// SteamPersistenceSubsystem.cpp
#include "SteamPersistenceSubsystem.h"
// Include Steam Online Subsystem headers

USteamPersistenceSubsystem::USteamPersistenceSubsystem()
    : bSteamCloudInitialized(false)
{
}

void USteamPersistenceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bSteamCloudInitialized = InitializeSteamCloud();
}

bool USteamPersistenceSubsystem::SaveToCloud(const FString& SlotName, const TArray<uint8>& Data)
{
    if (!bSteamCloudInitialized)
    {
        return false;
    }

    // Implement Steam Cloud save logic
    // Use Steam API to write data
    return true;
}

bool USteamPersistenceSubsystem::LoadFromCloud(const FString& SlotName, TArray<uint8>& OutData)
{
    if (!bSteamCloudInitialized)
    {
        return false;
    }

    // Implement Steam Cloud load logic
    // Use Steam API to read data
    return true;
}

bool USteamPersistenceSubsystem::IsCloudAvailable() const
{
    return bSteamCloudInitialized;
}
```

### Usage Example
```cpp
// GameInstance implementation
void UYourGameInstance::InitializeGame()
{
    // Get platform-specific persistence subsystem
    UPlatformPersistenceBase* PersistenceSystem = GetSubsystem<UPlatformPersistenceBase>();
    if (PersistenceSystem)
    {
        // Enable cloud sync if available
        if (PersistenceSystem->IsCloudAvailable())
        {
            PersistenceSystem->EnableCloudSync(true);
            PersistenceSystem->OnCloudSyncComplete.AddDynamic(this, &UYourGameInstance::HandleCloudSyncComplete);
            PersistenceSystem->OnSaveDataConflict.AddDynamic(this, &UYourGameInstance::HandleSaveDataConflict);
        }
    }
}

void UYourGameInstance::SaveGameState(const FString& SlotName)
{
    UPlatformPersistenceBase* PersistenceSystem = GetSubsystem<UPlatformPersistenceBase>();
    if (PersistenceSystem)
    {
        // Serialize game state
        TArray<uint8> SaveData;
        // ... Serialize game state to SaveData ...

        // Save to both local and cloud if available
        ESaveDataLocation SaveLocation = PersistenceSystem->IsCloudAvailable() ? 
            ESaveDataLocation::Both : ESaveDataLocation::Local;

        if (PersistenceSystem->SaveData(SlotName, SaveData, SaveLocation))
        {
            // Handle successful save
            ShowNotification(TEXT("Game saved successfully"));
        }
        else
        {
            // Handle save failure
            ShowNotification(TEXT("Failed to save game"), true);
        }
    }
}

void UYourGameInstance::HandleCloudSyncComplete(bool bSuccess)
{
    if (bSuccess)
    {
        ShowNotification(TEXT("Cloud sync completed"));
    }
    else
    {
        ShowNotification(TEXT("Cloud sync failed"), true);
    }
}

void UYourGameInstance::HandleSaveDataConflict(const FString& SlotName, const TArray<FSaveDataInfo>& Versions)
{
    // Show conflict resolution UI
    ShowConflictResolutionDialog(SlotName, Versions);
}
```

## Best Practices

1. **Platform Independence**
   - Abstract platform-specific code
   - Handle platform differences internally
   - Provide consistent interface

2. **Cloud Integration**
   - Handle connectivity issues
   - Implement proper conflict resolution
   - Manage bandwidth usage

3. **Data Integrity**
   - Validate data across platforms
   - Handle version differences
   - Implement proper error recovery

4. **User Experience**
   - Provide sync status feedback
   - Handle conflicts gracefully
   - Maintain data consistency

## Integration Tips

1. Implement platform detection
2. Handle offline scenarios
3. Add data compression
4. Implement conflict resolution UI
5. Add cloud quota management

## Common Pitfalls

1. Ignoring platform differences
2. Poor conflict resolution
3. Inadequate error handling
4. Not handling quota limits
5. Synchronization race conditions

## Performance Considerations

1. **Storage Efficiency**
   - Optimize data format
   - Implement compression
   - Handle large saves

2. **Network Usage**
   - Batch synchronization
   - Delta updates
   - Bandwidth management

3. **Platform Limitations**
   - Storage quotas
   - API rate limits
   - Platform restrictions

## Related Patterns

- [Pattern 1: Save Data Architecture](Pattern1_SaveDataArchitecture.md)
- [Pattern 2: Serialization Framework](Pattern2_SerializationFramework.md)
- [Pattern 3: Async Save System](Pattern3_AsyncSaveSystem.md)
- [Pattern 4: Save State Management](Pattern4_SaveStateManagement.md) 