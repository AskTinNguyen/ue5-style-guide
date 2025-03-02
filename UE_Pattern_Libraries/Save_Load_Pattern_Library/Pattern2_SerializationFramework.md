# Pattern 2: Serialization Framework

## Problem Statement
Games need a robust and flexible system to serialize complex game objects, components, and custom data types while maintaining type safety and handling circular references. The framework must support custom serialization rules and handle versioning of serialized data.

## Solution Overview
Implement a comprehensive serialization framework that leverages UE5's built-in serialization capabilities while providing extensions for custom types and complex object graphs.

## Implementation

### Custom Serialization Interface
```cpp
// ISerializableInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISerializableInterface.generated.h"

UINTERFACE(MinimalAPI)
class USerializableInterface : public UInterface
{
    GENERATED_BODY()
};

class ISerializableInterface
{
    GENERATED_BODY()

public:
    // Serialize object state to binary array
    virtual void Serialize(FMemoryWriter& Ar) = 0;
    
    // Deserialize object state from binary array
    virtual void Deserialize(FMemoryReader& Ar) = 0;
    
    // Get serialization version
    virtual uint32 GetSerializationVersion() const = 0;
};
```

### Serialization Helper
```cpp
// SerializationHelper.h
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SerializationHelper.generated.h"

UCLASS()
class YOURGAME_API USerializationHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Serialize UObject to binary array
    UFUNCTION(BlueprintCallable, Category = "Serialization")
    static bool SerializeObject(UObject* Object, TArray<uint8>& OutData);
    
    // Deserialize binary array to UObject
    UFUNCTION(BlueprintCallable, Category = "Serialization")
    static bool DeserializeObject(const TArray<uint8>& InData, UObject*& OutObject);
    
    // Serialize custom struct
    template<typename T>
    static bool SerializeStruct(const T& InStruct, TArray<uint8>& OutData);
    
    // Deserialize custom struct
    template<typename T>
    static bool DeserializeStruct(const TArray<uint8>& InData, T& OutStruct);

protected:
    // Helper functions for handling complex types
    static void SerializeMap(FMemoryWriter& Ar, const TMap<FName, FString>& Map);
    static void DeserializeMap(FMemoryReader& Ar, TMap<FName, FString>& Map);
};

// Implementation of template functions
template<typename T>
bool USerializationHelper::SerializeStruct(const T& InStruct, TArray<uint8>& OutData)
{
    FMemoryWriter Ar(OutData);
    InStruct.Serialize(Ar);
    return true;
}

template<typename T>
bool USerializationHelper::DeserializeStruct(const TArray<uint8>& InData, T& OutStruct)
{
    FMemoryReader Ar(InData);
    OutStruct.Serialize(Ar);
    return true;
}
```

### Custom Type Serialization Example
```cpp
// CustomGameState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ISerializableInterface.h"
#include "CustomGameState.generated.h"

USTRUCT()
struct FCustomData
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FString> StringArray;

    UPROPERTY()
    TMap<FName, float> FloatMap;

    // Custom serialization
    void Serialize(FArchive& Ar)
    {
        Ar << StringArray;
        
        if (Ar.IsSaving())
        {
            int32 MapSize = FloatMap.Num();
            Ar << MapSize;
            for (const auto& Pair : FloatMap)
            {
                FName Key = Pair.Key;
                float Value = Pair.Value;
                Ar << Key;
                Ar << Value;
            }
        }
        else if (Ar.IsLoading())
        {
            int32 MapSize;
            Ar << MapSize;
            FloatMap.Empty(MapSize);
            for (int32 i = 0; i < MapSize; ++i)
            {
                FName Key;
                float Value;
                Ar << Key;
                Ar << Value;
                FloatMap.Add(Key, Value);
            }
        }
    }
};

UCLASS()
class YOURGAME_API ACustomGameState : public AGameStateBase, public ISerializableInterface
{
    GENERATED_BODY()

public:
    ACustomGameState();

    // ISerializableInterface implementation
    virtual void Serialize(FMemoryWriter& Ar) override;
    virtual void Deserialize(FMemoryReader& Ar) override;
    virtual uint32 GetSerializationVersion() const override { return 1; }

    UPROPERTY()
    FCustomData CustomData;

private:
    void SerializeGameState(FArchive& Ar);
};
```

### Usage Example
```cpp
// Example of using the serialization framework
void AGameMode::SaveGameState()
{
    ACustomGameState* GameState = Cast<ACustomGameState>(GetGameState());
    if (GameState)
    {
        TArray<uint8> SerializedData;
        if (USerializationHelper::SerializeObject(GameState, SerializedData))
        {
            // Save to disk or send over network
            FFileHelper::SaveArrayToFile(SerializedData, *FPaths::ProjectSavedDir() / TEXT("GameState.sav"));
        }
    }
}

void AGameMode::LoadGameState()
{
    TArray<uint8> SerializedData;
    if (FFileHelper::LoadFileToArray(SerializedData, *FPaths::ProjectSavedDir() / TEXT("GameState.sav")))
    {
        ACustomGameState* GameState = Cast<ACustomGameState>(GetGameState());
        if (GameState)
        {
            USerializationHelper::DeserializeObject(SerializedData, GameState);
        }
    }
}
```

## Best Practices

1. **Type Safety**
   - Use strong typing for serialized data
   - Implement version checking
   - Handle null/invalid data gracefully

2. **Performance**
   - Minimize serialization overhead
   - Use appropriate container types
   - Consider data compression for large objects

3. **Maintainability**
   - Document serialization formats
   - Keep serialization logic centralized
   - Use consistent naming conventions

4. **Error Handling**
   - Validate data during deserialization
   - Handle version mismatches
   - Provide detailed error information

## Integration Tips

1. Create wrapper functions for common serialization tasks
2. Implement custom archive types for specific needs
3. Use compression when appropriate
4. Add checksums for data validation
5. Consider network replication requirements

## Common Pitfalls

1. Not handling version changes properly
2. Ignoring endianness in cross-platform games
3. Serializing unnecessary data
4. Not handling circular references
5. Insufficient error checking

## Performance Considerations

1. **Memory Usage**
   - Use appropriate buffer sizes
   - Clear temporary buffers
   - Handle large data sets efficiently

2. **CPU Usage**
   - Batch serialization operations
   - Use async operations when possible
   - Profile serialization performance

3. **Storage Impact**
   - Implement data compression
   - Use appropriate data formats
   - Consider storage space limitations

## Related Patterns

- [Pattern 1: Save Data Architecture](Pattern1_SaveDataArchitecture.md)
- [Pattern 3: Async Save System](Pattern3_AsyncSaveSystem.md)
- [Pattern 4: Save State Management](Pattern4_SaveStateManagement.md)
- [Pattern 5: Cross-Platform Persistence](Pattern5_CrossPlatformPersistence.md) 