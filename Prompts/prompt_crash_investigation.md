# Crash Investigation Prompt
We're experiencing crashes in our UE5 game. Here's the crash log:
[Crash log]

Related code:
[Code snippet]

Please analyze the crash, identify potential causes, and suggest fixes.
```

## Purpose
This prompt helps you identify and fix crash issues in your UE5 game by analyzing crash logs and relevant code.

## When to Use
- When your game is crashing during development or in production
- When crash reporters provide logs you need help interpreting
- When crashes are difficult to reproduce or diagnose

## Example Usage
```
We're experiencing crashes in our UE5 game. Here's the crash log:
```
Fatal error: [File:D:/BuildFarm/buildmachine_++UE5+Release-5.1/Engine/Source/Runtime/CoreUObject/Private/UObject/GarbageCollection.cpp] [Line: 1024] 
Object OctreeDataAsset /Game/Environment/Data/OctreeData.OctreeData is being referenced by FastIndexingVector but it was marked RF_Unreachable.
ReferencingObject: /Game/Environment/BP_WorldPartitioner.BP_WorldPartitioner_C
UE5 Editor [D:/Program Files/Epic Games/UE_5.1/Engine/Binaries/Win64/UnrealEditor.exe]
0x00007ff68d4d8c9a UnrealEditor.exe!CollectGarbageInternal() [D:\BuildFarm\buildmachine_++UE5+Release-5.1\Engine\Source\Runtime\CoreUObject\Private\UObject\GarbageCollection.cpp:1024]
```

Related code:
```cpp
void AWorldPartitioner::BeginPlay()
{
    Super::BeginPlay();
    
    if (OctreeDataAsset)
    {
        WorldOctree = new FOctreeSystem(OctreeDataAsset);
        // Rest of initialization...
    }
}

void AWorldPartitioner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    delete WorldOctree;
    WorldOctree = nullptr;
    
    Super::EndPlay(EndPlayReason);
}
```

Please analyze the crash, identify potential causes, and suggest fixes.
```

The AI will analyze the crash log and code to identify potential causes of the crash and suggest appropriate fixes that follow best practices. 