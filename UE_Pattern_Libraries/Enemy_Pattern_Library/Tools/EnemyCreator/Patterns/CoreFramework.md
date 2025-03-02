# Core Framework Pattern

## Problem Statement
Creating a robust and extensible framework for an enemy creation tool that supports template management, preview capabilities, and AI integration while maintaining good performance and user experience.

## Solution Overview
A modular framework combining editor utilities, custom viewports, and property customization to create a flexible enemy creation tool.

## Core Components

### 1. Editor Module
```cpp
// Module declaration
class FEnemyCreatorEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
private:
    void RegisterMenus();
    TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
    
    TSharedPtr<FUICommandList> PluginCommands;
};
```

### 2. Main Tool Window
```cpp
// Main tool window class
UCLASS()
class UEnemyCreatorToolWindow : public UEditorUtilityWidget
{
    GENERATED_BODY()
public:
    // Core interface
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    
    // Template management
    UFUNCTION(BlueprintCallable, Category = "Enemy Creation")
    UEnemyTemplate* CreateNewTemplate(const FString& TemplateName);
    
    // Preview management
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void UpdatePreview(UEnemyTemplate* Template);
    
protected:
    // UI Components
    UPROPERTY()
    class UEnemyPreviewViewport* PreviewViewport;
    
    UPROPERTY()
    class UEnemyPropertyEditor* PropertyEditor;
};
```

### 3. Layout System
```cpp
// Layout creation
TSharedRef<SDockTab> FEnemyCreatorEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
    .TabRole(ETabRole::NomadTab)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            // Toolbar
            CreateToolbar()
        ]
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SNew(SSplitter)
            .Orientation(Orient_Horizontal)
            + SSplitter::Slot()
            .Value(0.2f)
            [
                // Template browser
                CreateTemplateBrowser()
            ]
            + SSplitter::Slot()
            .Value(0.5f)
            [
                // Preview viewport
                CreatePreviewViewport()
            ]
            + SSplitter::Slot()
            .Value(0.3f)
            [
                // Property editor
                CreatePropertyEditor()
            ]
        ]
    ];
}
```

## Usage Example

### 1. Module Registration
```cpp
// In your game's editor module
void FYourGameEditorModule::StartupModule()
{
    // Register enemy creator module
    FEnemyCreatorEditorModule::Get().StartupModule();
    
    // Register menu extension
    FEnemyCreatorEditorModule::Get().RegisterMenuExtension(
        "LevelEditor.MainMenu.Window",
        FMenuExtensionDelegate::CreateRaw(this, &FYourGameEditorModule::AddEnemyCreatorMenu)
    );
}
```

### 2. Tool Window Creation
```cpp
// Creating the tool window
void FYourGameEditorModule::OpenEnemyCreator()
{
    UEnemyCreatorToolWindow* ToolWindow = CreateWidget<UEnemyCreatorToolWindow>(
        GEditor->GetEditorWorldContext().World(),
        UEnemyCreatorToolWindow::StaticClass()
    );
    
    if (ToolWindow)
    {
        ToolWindow->AddToViewport();
    }
}
```

## Best Practices

1. **Module Organization**
   - Keep clear separation of concerns
   - Use proper dependency management
   - Follow UE5 module guidelines

2. **UI Design**
   - Implement proper tab management
   - Support window docking
   - Handle window resizing gracefully

3. **State Management**
   - Maintain clean state transitions
   - Implement proper cleanup
   - Support undo/redo operations

## Common Pitfalls

1. **Resource Management**
   - Memory leaks from unmanaged resources
   - Dangling references to closed windows
   - Unregistered menu extensions

2. **UI Issues**
   - Poor window resizing behavior
   - Unhandled edge cases in layout
   - Missing cleanup on window close

## Performance Considerations

1. **Memory Management**
   - Clean up resources properly
   - Use weak pointers for references
   - Implement proper garbage collection

2. **UI Performance**
   - Optimize layout updates
   - Cache frequently used widgets
   - Implement proper widget recycling

## Testing Strategy

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyCreatorTest, "EnemyCreator.Core",
    EAutomationTestFlags::ApplicationContextMask | 
    EAutomationTestFlags::ProductFilter)

bool FEnemyCreatorTest::RunTest(const FString& Parameters)
{
    // Test module initialization
    TestTrue("Module initialized successfully", 
        FEnemyCreatorEditorModule::Get().IsInitialized());
    
    // Test window creation
    UEnemyCreatorToolWindow* Window = CreateTestWindow();
    TestNotNull("Window created successfully", Window);
    
    // Test basic functionality
    TestTrue("Can create template", 
        Window->CreateNewTemplate("TestEnemy") != nullptr);
    
    return true;
}
```

## Integration Guidelines

1. **With Content Browser**
```cpp
void UEnemyCreatorToolWindow::RegisterContentBrowserCallbacks()
{
    // Register asset context menu extension
    FContentBrowserModule& ContentBrowserModule = 
        FModuleManager::LoadModuleChecked<FContentBrowserModule>
        (TEXT("ContentBrowser"));
    
    TArray<FContentBrowserMenuExtender_SelectedAssets>& 
        MenuExtenderDelegates = ContentBrowserModule
        .GetAllAssetViewContextMenuExtenders();
    
    MenuExtenderDelegates.Add(FContentBrowserMenuExtender_SelectedAssets::
        CreateRaw(this, &UEnemyCreatorToolWindow::OnExtendContentBrowserMenu));
}
```

2. **With Level Editor**
```cpp
void UEnemyCreatorToolWindow::RegisterLevelEditorCallbacks()
{
    // Register level editor callbacks
    FLevelEditorModule& LevelEditorModule = 
        FModuleManager::LoadModuleChecked<FLevelEditorModule>
        ("LevelEditor");
    
    LevelEditorModule.OnMapChanged().AddUObject(
        this, 
        &UEnemyCreatorToolWindow::OnMapChanged
    );
}
```

## Debugging Support

1. **Debug Visualization**
```cpp
void UEnemyCreatorToolWindow::EnableDebugDisplay()
{
    if (PreviewViewport)
    {
        PreviewViewport->ShowDebugInfo(true);
        PreviewViewport->ShowStats(true);
        PreviewViewport->ShowGrid(true);
    }
}
```

2. **Logging System**
```cpp
DEFINE_LOG_CATEGORY(LogEnemyCreator);

void UEnemyCreatorToolWindow::LogDebugInfo()
{
    UE_LOG(LogEnemyCreator, Log, TEXT("Window State: %s"), 
        *GetStateString());
    UE_LOG(LogEnemyCreator, Log, TEXT("Active Template: %s"), 
        *GetActiveTemplateName());
    UE_LOG(LogEnemyCreator, Log, TEXT("Memory Usage: %d KB"), 
        GetMemoryUsageKB());
}
```

## References

- [UE5 Editor Framework](https://docs.unrealengine.com/5.0/en-US/editor-framework-in-unreal-engine/)
- [Slate UI Framework](https://docs.unrealengine.com/5.0/en-US/slate-ui-framework-in-unreal-engine/)
- [Property System](https://docs.unrealengine.com/5.0/en-US/property-system-in-unreal-engine/) 