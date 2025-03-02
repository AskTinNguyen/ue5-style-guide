# Navigation and Pathfinding Pattern

## Problem Definition
AI characters need efficient and flexible navigation capabilities, including pathfinding, obstacle avoidance, and group movement coordination, while maintaining performance with many agents.

## Solution Approach
Implement an enhanced navigation system that extends Unreal Engine's built-in navigation system with custom pathfinding algorithms, dynamic obstacle handling, and group movement features.

## Core Components

### 1. Custom Navigation Data
```cpp
UCLASS()
class GAME_API UCustomNavigationData : public UNavigationData
{
    GENERATED_BODY()

public:
    UCustomNavigationData();

    virtual void RebuildAll() override;
    virtual bool ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& Extent) const override;

protected:
    UPROPERTY(EditAnywhere, Category = "Navigation")
    float TileSize;

    UPROPERTY(EditAnywhere, Category = "Navigation")
    float CellSize;

    virtual void BuildNavMesh();
    virtual void UpdateNavMesh(const FBox& DirtyArea);
};
```

### 2. Custom Path Following Component
```cpp
UCLASS()
class GAME_API UCustomPathFollowingComponent : public UPathFollowingComponent
{
    GENERATED_BODY()

public:
    UCustomPathFollowingComponent();

    virtual void FollowPath(const FAIRequestID RequestID, const FNavPathShared& Path) override;
    
    UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
    void SetCustomMovementParameters(const FCustomMovementParams& Params);

protected:
    UPROPERTY()
    FCustomMovementParams MovementParams;

    virtual void UpdatePathSegment() override;
    virtual void HandlePathObstacle();
};
```

### 3. Group Movement Manager
```cpp
UCLASS()
class GAME_API AGroupMovementManager : public AActor
{
    GENERATED_BODY()

public:
    AGroupMovementManager();

    UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
    void RegisterGroupMember(AAIController* Member, int32 GroupID);

    UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
    void MoveGroupTo(int32 GroupID, const FVector& Destination);

protected:
    UPROPERTY()
    TMap<int32, FGroupData> Groups;

    void UpdateGroupFormation(int32 GroupID);
    void CalculateGroupPositions(const FGroupData& Group, TArray<FVector>& OutPositions);
};
```

## Implementation Guidelines

1. **Pathfinding Implementation**
   - Implement A* pathfinding
   - Handle dynamic obstacles
   - Support different movement types
   - Add path smoothing

2. **Group Movement**
   - Implement formation movement
   - Handle group separation/joining
   - Coordinate group behavior
   - Manage group priorities

3. **Performance Optimization**
   - Use spatial partitioning
   - Implement path caching
   - Optimize update frequency
   - Handle LOD for distant agents

## Usage Example

```cpp
// Example custom pathfinding implementation
UCLASS()
class UCustomPathFinder : public UObject
{
    GENERATED_BODY()

public:
    FNavPathShared FindPath(const FVector& Start, const FVector& End)
    {
        // Initialize path
        FNavPathShared Path = MakeShared<FNavigationPath>();
        
        // Get navigation data
        ANavigationData* NavData = GetNavData();
        if (!NavData) return Path;

        // Perform A* pathfinding
        TArray<FNavPathPoint> PathPoints;
        if (FindPathAStar(Start, End, PathPoints))
        {
            Path->PathPoints = PathPoints;
            SmoothPath(Path);
        }

        return Path;
    }

protected:
    bool FindPathAStar(const FVector& Start, const FVector& End, TArray<FNavPathPoint>& OutPath)
    {
        // A* implementation
        FPathFindingNode* StartNode = CreateNode(Start);
        FPathFindingNode* EndNode = CreateNode(End);
        
        TArray<FPathFindingNode*> OpenSet;
        TArray<FPathFindingNode*> ClosedSet;
        
        OpenSet.Add(StartNode);
        
        while (OpenSet.Num() > 0)
        {
            // A* algorithm implementation
            FPathFindingNode* CurrentNode = GetLowestFScoreNode(OpenSet);
            
            if (CurrentNode == EndNode)
            {
                ReconstructPath(CurrentNode, OutPath);
                return true;
            }
            
            // Process neighbors
            ProcessNeighbors(CurrentNode, EndNode, OpenSet, ClosedSet);
        }
        
        return false;
    }
};
```

## Integration Tips

1. **Navigation Setup**
```cpp
// In your AI Controller
void AAIControllerBase::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // Setup custom path following
    if (UCustomPathFollowingComponent* PathComp = GetPathFollowingComponent())
    {
        FCustomMovementParams Params;
        Params.MovementSpeed = 600.0f;
        Params.AcceptanceRadius = 50.0f;
        Params.bCanStrafe = true;
        
        PathComp->SetCustomMovementParameters(Params);
    }
}
```

2. **Group Movement Setup**
```cpp
// In your AI Controller
void AAIControllerBase::JoinGroup(int32 GroupID)
{
    if (AGroupMovementManager* GroupManager = GetGroupManager())
    {
        GroupManager->RegisterGroupMember(this, GroupID);
        
        // Setup group behavior
        if (UCustomPathFollowingComponent* PathComp = GetPathFollowingComponent())
        {
            PathComp->SetGroupBehavior(true);
        }
    }
}
```

## Debugging Support

1. **Path Visualization**
```cpp
void UCustomPathFollowingComponent::DrawDebugPath()
{
    #if ENABLE_VISUAL_LOG
        if (Path.IsValid())
        {
            const TArray<FNavPathPoint>& Points = Path->GetPathPoints();
            for (int32 i = 0; i < Points.Num() - 1; ++i)
            {
                DrawDebugLine(
                    GetWorld(),
                    Points[i].Location,
                    Points[i + 1].Location,
                    FColor::Green,
                    false,
                    -1.0f,
                    0,
                    2.0f
                );
            }
        }
    #endif
}
```

2. **Group Movement Visualization**
```cpp
void AGroupMovementManager::DrawDebugFormation(int32 GroupID)
{
    #if ENABLE_VISUAL_LOG
        if (const FGroupData* Group = Groups.Find(GroupID))
        {
            TArray<FVector> FormationPositions;
            CalculateGroupPositions(*Group, FormationPositions);
            
            for (const FVector& Pos : FormationPositions)
            {
                DrawDebugSphere(
                    GetWorld(),
                    Pos,
                    20.0f,
                    12,
                    FColor::Blue,
                    false,
                    -1.0f,
                    0,
                    2.0f
                );
            }
        }
    #endif
}
```

## Common Pitfalls and Solutions

1. **Path Following Issues**
   - Implement path smoothing
   - Handle corner cases
   - Add path validation
   - Implement path replanning

2. **Group Movement Problems**
   - Handle group separation
   - Manage formation changes
   - Handle obstacle avoidance
   - Implement group priorities

3. **Performance Issues**
   - Use path caching
   - Implement LOD system
   - Optimize update frequency
   - Use spatial partitioning

## Testing

1. **Unit Tests**
```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNavigationTest, "AI.Navigation.BasicTests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FNavigationTest::RunTest(const FString& Parameters)
{
    // Create test components
    UCustomPathFinder* PathFinder = NewObject<UCustomPathFinder>();
    
    // Test pathfinding
    FVector Start(0.0f, 0.0f, 0.0f);
    FVector End(100.0f, 100.0f, 0.0f);
    FNavPathShared Path = PathFinder->FindPath(Start, End);
    
    // Verify path
    TestTrue("Path Found", Path.IsValid() && Path->GetPathPoints().Num() > 0);
    
    return true;
}
```

## Performance Optimization

1. **Path Caching**
```cpp
USTRUCT()
struct FPathCacheEntry
{
    GENERATED_BODY()

    FVector StartPoint;
    FVector EndPoint;
    FNavPathShared Path;
    float TimeStamp;
};

class FPathCache
{
public:
    FNavPathShared GetCachedPath(const FVector& Start, const FVector& End);
    void AddPath(const FVector& Start, const FVector& End, const FNavPathShared& Path);
    void CleanupCache(float MaxAge);

private:
    TArray<FPathCacheEntry> CachedPaths;
};
```

2. **Spatial Optimization**
```cpp
void UCustomNavigationData::OptimizeNavMesh()
{
    // Build navigation octree
    FOctree NavOctree(GetNavMeshBounds());
    
    // Add nav mesh polygons
    for (const FNavPoly& Poly : NavPolys)
    {
        NavOctree.AddElement(Poly);
    }
    
    // Query optimization
    NavOctree.OptimizeTree();
}
```

## References

- [Unreal Engine Navigation System Documentation](https://docs.unrealengine.com/5.0/en-US/navigation-system-in-unreal-engine/)
- [AI Navigation Best Practices](https://docs.unrealengine.com/5.0/en-US/ai-navigation-in-unreal-engine/) 