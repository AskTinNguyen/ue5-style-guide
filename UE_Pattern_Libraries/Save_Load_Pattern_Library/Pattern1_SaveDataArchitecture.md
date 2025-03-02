# Pattern 1: Save Data Architecture

## Problem Statement
Game save systems need a flexible and maintainable data structure that can evolve with the game's development while maintaining backward compatibility. The architecture must handle complex game states, support versioning, and ensure data integrity.

## Solution Overview
Implement a hierarchical save data structure using UE5's serialization framework with version control and migration paths.

## Implementation

### Core Save Game Class
```cpp
// SaveGameBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameBase.generated.h"

USTRUCT()
struct FSaveMetadata
{
    GENERATED_BODY()

    UPROPERTY()
    int32 SaveVersion;

    UPROPERTY()
    FDateTime SaveTimestamp;

    UPROPERTY()
    FString SaveDescription;
};

UCLASS()
class YOURGAME_API USaveGameBase : public USaveGame
{
    GENERATED_BODY()

public:
    USaveGameBase();

    // Current version of the save format
    static const int32 CURRENT_SAVE_VERSION = 1;

    UPROPERTY()
    FSaveMetadata Metadata;

    // Virtual function for version migration
    virtual bool MigrateVersion(int32 FromVersion);

protected:
    // Helper function to check if migration is needed
    bool RequiresMigration() const;
};
```

### Save Data Container
```cpp
// GameSaveData.h
#pragma once

#include "SaveGameBase.h"
#include "GameSaveData.generated.h"

USTRUCT()
struct FPlayerData
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Location;

    UPROPERTY()
    FRotator Rotation;

    UPROPERTY()
    float Health;

    UPROPERTY()
    TArray<FName> Inventory;
};

USTRUCT()
struct FWorldState
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<FName, bool> Flags;

    UPROPERTY()
    TArray<FTransform> DynamicObjectTransforms;
};

UCLASS()
class YOURGAME_API UGameSaveData : public USaveGameBase
{
    GENERATED_BODY()

public:
    UGameSaveData();

    UPROPERTY()
    FPlayerData PlayerData;

    UPROPERTY()
    FWorldState WorldState;

    UPROPERTY()
    TMap<FName, FString> CustomData;

    // Override migration function
    virtual bool MigrateVersion(int32 FromVersion) override;
};
```

## Usage Example
```cpp
// Saving game data
void AYourGameMode::SaveGame()
{
    UGameSaveData* SaveData = Cast<UGameSaveData>(UGameplayStatics::CreateSaveGameObject(UGameSaveData::StaticClass()));
    if (SaveData)
    {
        // Fill save data
        SaveData->Metadata.SaveVersion = UGameSaveData::CURRENT_SAVE_VERSION;
        SaveData->Metadata.SaveTimestamp = FDateTime::Now();
        
        // Save player data
        APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (PlayerChar)
        {
            SaveData->PlayerData.Location = PlayerChar->GetActorLocation();
            SaveData->PlayerData.Rotation = PlayerChar->GetActorRotation();
            SaveData->PlayerData.Health = PlayerChar->GetHealth();
        }

        // Save to disk
        if (UGameplayStatics::SaveGameToSlot(SaveData, TEXT("SaveSlot"), 0))
        {
            UE_LOG(LogTemp, Log, TEXT("Game saved successfully"));
        }
    }
}

// Loading game data
void AYourGameMode::LoadGame()
{
    UGameSaveData* SaveData = Cast<UGameSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot"), 0));
    if (SaveData)
    {
        // Check version and migrate if necessary
        if (SaveData->RequiresMigration())
        {
            if (!SaveData->MigrateVersion(SaveData->Metadata.SaveVersion))
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to migrate save data"));
                return;
            }
        }

        // Apply save data
        APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (PlayerChar)
        {
            PlayerChar->SetActorLocation(SaveData->PlayerData.Location);
            PlayerChar->SetActorRotation(SaveData->PlayerData.Rotation);
            PlayerChar->SetHealth(SaveData->PlayerData.Health);
        }
    }
}
```

## Best Practices

1. **Versioning**
   - Always increment `CURRENT_SAVE_VERSION` when making breaking changes
   - Implement migration paths for each version change
   - Document version changes and migration logic

2. **Data Organization**
   - Use structs to group related data
   - Keep the save data structure modular
   - Use appropriate UPROPERTY() specifiers

3. **Error Handling**
   - Validate data during load
   - Handle missing or corrupt data gracefully
   - Log migration and load/save operations

4. **Performance**
   - Only save necessary data
   - Use efficient data structures
   - Consider data size and serialization cost

## Integration Tips

1. Create a save game subsystem for centralized save/load management
2. Implement auto-save functionality using the architecture
3. Add save data compression for large save files
4. Include save data validation and checksums
5. Consider implementing save data encryption for sensitive information

## Common Pitfalls

1. Not handling version migration properly
2. Saving unnecessary or redundant data
3. Not validating data during load
4. Ignoring platform-specific save locations
5. Not handling save failures gracefully

## Related Patterns

- [Pattern 2: Serialization Framework](Pattern2_SerializationFramework.md)
- [Pattern 3: Async Save System](Pattern3_AsyncSaveSystem.md)
- [Pattern 4: Save State Management](Pattern4_SaveStateManagement.md)
- [Pattern 5: Cross-Platform Persistence](Pattern5_CrossPlatformPersistence.md) 