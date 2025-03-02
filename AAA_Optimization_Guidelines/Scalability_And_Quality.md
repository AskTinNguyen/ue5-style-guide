# Scalability and Quality Settings in UE5

This document provides comprehensive guidelines for implementing and managing scalability and quality settings in Unreal Engine 5 for AAA game development. Effective scalability systems are essential for delivering consistent performance across a wide range of hardware while maintaining the best possible visual quality for each target platform.

## Table of Contents
- [Scalability System Overview](#scalability-system-overview)
- [Quality Presets](#quality-presets)
- [Dynamic Resolution](#dynamic-resolution)
- [Platform-Specific Scalability](#platform-specific-scalability)
- [Content-Aware Scalability](#content-aware-scalability)
- [Performance Metrics and Adaptation](#performance-metrics-and-adaptation)
- [Best Practices](#best-practices)

## Scalability System Overview

Unreal Engine 5's scalability system provides a framework for adjusting visual quality and performance across different hardware capabilities.

### Core Scalability Groups

```cpp
// Configure core scalability groups
void UScalabilityManager::ConfigureScalabilityGroups()
{
    // Define core scalability groups
    TArray<FString> ScalabilityGroups = {
        TEXT("ViewDistance"),
        TEXT("AntiAliasing"),
        TEXT("PostProcessing"),
        TEXT("Shadows"),
        TEXT("GlobalIllumination"),
        TEXT("Reflections"),
        TEXT("Effects"),
        TEXT("Foliage"),
        TEXT("Lumen"),
        TEXT("Nanite"),
        TEXT("VirtualShadows")
    };
    
    // Register custom scalability groups
    ScalabilityGroups.Add(TEXT("CharacterLOD"));
    ScalabilityGroups.Add(TEXT("EnvironmentLOD"));
    ScalabilityGroups.Add(TEXT("VFXQuality"));
    
    // Initialize all groups
    for (const FString& Group : ScalabilityGroups)
    {
        InitializeScalabilityGroup(Group);
    }
}
```

### Custom Scalability Settings

```cpp
// Define custom scalability settings
void UScalabilityManager::DefineCustomScalabilitySettings()
{
    // Register custom console variables
    IConsoleManager::Get().RegisterConsoleVariable(
        TEXT("r.CustomScalability.CharacterLOD"),
        2,
        TEXT("Character LOD quality (0=Low, 1=Medium, 2=High, 3=Epic)"),
        ECVF_Scalability | ECVF_RenderThreadSafe
    );
    
    IConsoleManager::Get().RegisterConsoleVariable(
        TEXT("r.CustomScalability.EnvironmentLOD"),
        2,
        TEXT("Environment LOD quality (0=Low, 1=Medium, 2=High, 3=Epic)"),
        ECVF_Scalability | ECVF_RenderThreadSafe
    );
    
    IConsoleManager::Get().RegisterConsoleVariable(
        TEXT("r.CustomScalability.VFXQuality"),
        2,
        TEXT("VFX quality (0=Low, 1=Medium, 2=High, 3=Epic)"),
        ECVF_Scalability | ECVF_RenderThreadSafe
    );
}
```

## Quality Presets

### Standard Quality Presets

| Setting | Low | Medium | High | Epic |
|---------|-----|--------|------|------|
| View Distance | 0 | 1 | 2 | 3 |
| Anti-Aliasing | 0 | 1 | 2 | 3 |
| Post-Processing | 0 | 1 | 2 | 3 |
| Shadows | 0 | 1 | 2 | 3 |
| Global Illumination | 0 | 1 | 2 | 3 |
| Reflections | 0 | 1 | 2 | 3 |
| Effects | 0 | 1 | 2 | 3 |
| Foliage | 0 | 1 | 2 | 3 |
| Lumen | Off | Low | Medium | High |
| Nanite | Off | On | On | On |
| Virtual Shadows | Off | Low | Medium | High |

### Custom Quality Presets

```cpp
// Define custom quality presets
void UScalabilityManager::DefineCustomQualityPresets()
{
    // Define custom presets
    QualityPresets.Add(TEXT("Performance"), FQualityPreset{
        ViewDistance: 0,
        AntiAliasing: 0,
        PostProcessing: 0,
        Shadows: 0,
        GlobalIllumination: 0,
        Reflections: 0,
        Effects: 0,
        Foliage: 0,
        Lumen: 0,
        Nanite: 0,
        VirtualShadows: 0,
        CharacterLOD: 0,
        EnvironmentLOD: 0,
        VFXQuality: 0
    });
    
    QualityPresets.Add(TEXT("Balanced"), FQualityPreset{
        ViewDistance: 1,
        AntiAliasing: 1,
        PostProcessing: 1,
        Shadows: 1,
        GlobalIllumination: 1,
        Reflections: 1,
        Effects: 1,
        Foliage: 1,
        Lumen: 1,
        Nanite: 1,
        VirtualShadows: 1,
        CharacterLOD: 1,
        EnvironmentLOD: 1,
        VFXQuality: 1
    });
    
    QualityPresets.Add(TEXT("Quality"), FQualityPreset{
        ViewDistance: 2,
        AntiAliasing: 2,
        PostProcessing: 2,
        Shadows: 2,
        GlobalIllumination: 2,
        Reflections: 2,
        Effects: 2,
        Foliage: 2,
        Lumen: 2,
        Nanite: 1,
        VirtualShadows: 2,
        CharacterLOD: 2,
        EnvironmentLOD: 2,
        VFXQuality: 2
    });
    
    QualityPresets.Add(TEXT("Ultra"), FQualityPreset{
        ViewDistance: 3,
        AntiAliasing: 3,
        PostProcessing: 3,
        Shadows: 3,
        GlobalIllumination: 3,
        Reflections: 3,
        Effects: 3,
        Foliage: 3,
        Lumen: 3,
        Nanite: 1,
        VirtualShadows: 3,
        CharacterLOD: 3,
        EnvironmentLOD: 3,
        VFXQuality: 3
    });
}
```

### Applying Quality Presets

```cpp
// Apply quality preset
void UScalabilityManager::ApplyQualityPreset(const FString& PresetName)
{
    // Find preset
    if (QualityPresets.Contains(PresetName))
    {
        const FQualityPreset& Preset = QualityPresets[PresetName];
        
        // Apply core settings
        Scalability::FQualityLevels QualityLevels;
        QualityLevels.SetFromSingleQualityLevel(Preset.ViewDistance);
        
        // Apply individual settings
        Scalability::SetQualityLevels(QualityLevels);
        
        // Apply custom settings
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomScalability.CharacterLOD"))->Set(Preset.CharacterLOD);
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomScalability.EnvironmentLOD"))->Set(Preset.EnvironmentLOD);
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomScalability.VFXQuality"))->Set(Preset.VFXQuality);
        
        // Apply Lumen settings
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Quality"))->Set(Preset.Lumen);
        
        // Apply Nanite settings
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"))->Set(Preset.Nanite == 0 ? 4.0f : 8.0f);
        
        // Apply Virtual Shadow settings
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Virtual.Enable"))->Set(Preset.VirtualShadows > 0 ? 1 : 0);
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Virtual.Quality"))->Set(Preset.VirtualShadows);
        
        // Save settings
        Scalability::SaveState(GGameUserSettingsIni);
        
        // Notify system
        OnQualityPresetApplied.Broadcast(PresetName);
    }
}
```

## Dynamic Resolution

### Dynamic Resolution Configuration

```cpp
// Configure dynamic resolution
void UScalabilityManager::ConfigureDynamicResolution()
{
    // Enable dynamic resolution
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.Enable"))->Set(1);
    
    // Configure frame time target (33.33ms for 30 FPS, 16.67ms for 60 FPS)
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.FrameTimeBudget"))->Set(16.67f);
    
    // Configure resolution percentage limits
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.MinScreenPercentage"))->Set(70.0f);
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.MaxScreenPercentage"))->Set(100.0f);
    
    // Configure adaptation speed
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.ChangePercentageThreshold"))->Set(5.0f);
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.ChangePercentageRate"))->Set(2.0f);
}
```

### Custom Dynamic Resolution Implementation

```cpp
// Custom dynamic resolution implementation
void UDynamicResolutionManager::Initialize()
{
    // Register for frame time updates
    FCoreDelegates::OnEndFrame.AddUObject(this, &UDynamicResolutionManager::UpdateDynamicResolution);
    
    // Configure settings
    Settings.TargetFrameTimeMS = 16.67f; // 60 FPS
    Settings.MinScreenPercentage = 70.0f;
    Settings.MaxScreenPercentage = 100.0f;
    Settings.AdjustmentSpeed = 0.05f;
    Settings.StabilityThreshold = 0.1f;
    
    // Initialize current percentage
    CurrentScreenPercentage = Settings.MaxScreenPercentage;
}

// Update dynamic resolution based on performance
void UDynamicResolutionManager::UpdateDynamicResolution()
{
    // Get current frame time
    float CurrentFrameTimeMS = FApp::GetDeltaTime() * 1000.0f;
    
    // Calculate target adjustment
    float FrameTimeRatio = CurrentFrameTimeMS / Settings.TargetFrameTimeMS;
    float TargetAdjustment = 0.0f;
    
    // Determine if adjustment is needed
    if (FrameTimeRatio > (1.0f + Settings.StabilityThreshold))
    {
        // Frame time too high, reduce resolution
        TargetAdjustment = -Settings.AdjustmentSpeed * (FrameTimeRatio - 1.0f);
    }
    else if (FrameTimeRatio < (1.0f - Settings.StabilityThreshold))
    {
        // Frame time low, increase resolution
        TargetAdjustment = Settings.AdjustmentSpeed * (1.0f - FrameTimeRatio);
    }
    
    // Apply adjustment
    if (FMath::Abs(TargetAdjustment) > KINDA_SMALL_NUMBER)
    {
        // Calculate new percentage
        float NewPercentage = FMath::Clamp(
            CurrentScreenPercentage + TargetAdjustment * 100.0f,
            Settings.MinScreenPercentage,
            Settings.MaxScreenPercentage
        );
        
        // Apply if changed significantly
        if (FMath::Abs(NewPercentage - CurrentScreenPercentage) >= 1.0f)
        {
            CurrentScreenPercentage = NewPercentage;
            IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"))->Set(CurrentScreenPercentage);
        }
    }
}
```

## Platform-Specific Scalability

### Platform Detection and Configuration

```cpp
// Configure platform-specific scalability
void UScalabilityManager::ConfigurePlatformScalability()
{
    // Detect platform
    FString PlatformName = UGameplayStatics::GetPlatformName();
    
    // Configure based on platform
    if (PlatformName == TEXT("PS5"))
    {
        ConfigurePS5Scalability();
    }
    else if (PlatformName == TEXT("XSX"))
    {
        ConfigureXSXScalability();
    }
    else if (PlatformName == TEXT("Switch"))
    {
        ConfigureSwitchScalability();
    }
    else if (PlatformName.Contains(TEXT("Android")))
    {
        ConfigureAndroidScalability();
    }
    else if (PlatformName.Contains(TEXT("IOS")))
    {
        ConfigureIOSScalability();
    }
    else
    {
        // PC platform
        ConfigurePCScalability();
    }
}

// Configure PS5 scalability
void UScalabilityManager::ConfigurePS5Scalability()
{
    // PS5-specific settings
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Quality"))->Set(2); // Medium Lumen
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"))->Set(8.0f);
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Virtual.Enable"))->Set(1);
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Virtual.Quality"))->Set(2);
    
    // Configure dynamic resolution for 60 FPS target
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.Enable"))->Set(1);
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.FrameTimeBudget"))->Set(16.67f);
    IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.MinScreenPercentage"))->Set(80.0f);
}
```

### Hardware Detection and Auto-Configuration

```cpp
// Auto-configure PC scalability based on hardware
void UScalabilityManager::AutoConfigurePCScalability()
{
    // Detect GPU
    FString GPUBrand = FPlatformMisc::GetPrimaryGPUBrand();
    int32 GPUVendorID = FPlatformMisc::GetGPUVendorId();
    
    // Detect CPU
    int32 CPUCores = FPlatformMisc::NumberOfCores();
    int32 CPUThreads = FPlatformMisc::NumberOfWorkerThreadsToSpawn();
    
    // Detect memory
    uint64 TotalPhysicalGB = FPlatformMemory::GetPhysicalGBRam();
    
    // Determine quality preset based on hardware
    FString TargetPreset;
    
    // High-end hardware
    if (TotalPhysicalGB >= 16 && CPUCores >= 8 && 
        (GPUBrand.Contains(TEXT("RTX 30")) || GPUBrand.Contains(TEXT("RTX 40")) || 
         GPUBrand.Contains(TEXT("RX 6")) || GPUBrand.Contains(TEXT("RX 7"))))
    {
        TargetPreset = TEXT("Ultra");
    }
    // Mid-range hardware
    else if (TotalPhysicalGB >= 12 && CPUCores >= 6 && 
             (GPUBrand.Contains(TEXT("RTX 20")) || GPUBrand.Contains(TEXT("GTX 16")) || 
              GPUBrand.Contains(TEXT("RX 5"))))
    {
        TargetPreset = TEXT("Quality");
    }
    // Low-end hardware
    else if (TotalPhysicalGB >= 8 && CPUCores >= 4)
    {
        TargetPreset = TEXT("Balanced");
    }
    // Minimum spec
    else
    {
        TargetPreset = TEXT("Performance");
    }
    
    // Apply preset
    ApplyQualityPreset(TargetPreset);
}
```

## Content-Aware Scalability

### Performance Budget System

```cpp
// Performance budget system
void UPerformanceBudgetManager::Initialize()
{
    // Configure budgets
    PerformanceBudgets.Add(TEXT("Combat"), FPerformanceBudget{
        MaxDrawCalls: 2500,
        MaxTriangles: 8000000,
        MaxLights: 12,
        MaxParticleSystems: 20,
        MaxAudioSources: 32
    });
    
    PerformanceBudgets.Add(TEXT("Exploration"), FPerformanceBudget{
        MaxDrawCalls: 3000,
        MaxTriangles: 10000000,
        MaxLights: 16,
        MaxParticleSystems: 15,
        MaxAudioSources: 24
    });
    
    PerformanceBudgets.Add(TEXT("Cinematic"), FPerformanceBudget{
        MaxDrawCalls: 3500,
        MaxTriangles: 12000000,
        MaxLights: 20,
        MaxParticleSystems: 30,
        MaxAudioSources: 48
    });
    
    // Register for updates
    FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &UPerformanceBudgetManager::UpdateBudgets),
        1.0f // Check every second
    );
}

// Update performance budgets
bool UPerformanceBudgetManager::UpdateBudgets(float DeltaTime)
{
    // Get current gameplay state
    FString CurrentGameState = GetCurrentGameplayState();
    
    // Apply appropriate budget if available
    if (PerformanceBudgets.Contains(CurrentGameState))
    {
        ApplyBudget(PerformanceBudgets[CurrentGameState]);
    }
    
    return true; // Continue ticking
}
```

### Context-Aware Quality Adjustments

```cpp
// Context-aware quality adjustment system
void UContextAwareScalability::Initialize()
{
    // Configure context rules
    ContextRules.Add(TEXT("Combat"), FContextRule{
        ScalabilityAdjustments: {
            {TEXT("Effects"), 1},
            {TEXT("PostProcessing"), 1},
            {TEXT("Shadows"), -1}
        },
        DynamicResolutionAdjustment: -5.0f // Reduce by 5%
    });
    
    ContextRules.Add(TEXT("Exploration"), FContextRule{
        ScalabilityAdjustments: {
            {TEXT("ViewDistance"), 1},
            {TEXT("Foliage"), 1}
        },
        DynamicResolutionAdjustment: 0.0f // No change
    });
    
    ContextRules.Add(TEXT("Cinematic"), FContextRule{
        ScalabilityAdjustments: {
            {TEXT("PostProcessing"), 1},
            {TEXT("Shadows"), 1},
            {TEXT("Reflections"), 1}
        },
        DynamicResolutionAdjustment: 5.0f // Increase by 5%
    });
    
    // Register for context changes
    OnGameplayContextChanged.AddUObject(this, &UContextAwareScalability::HandleContextChange);
}

// Handle context changes
void UContextAwareScalability::HandleContextChange(const FString& NewContext)
{
    // Store original settings if first change
    if (OriginalSettings.IsEmpty())
    {
        CaptureOriginalSettings();
    }
    
    // Reset to original settings
    ResetToOriginalSettings();
    
    // Apply context-specific adjustments
    if (ContextRules.Contains(NewContext))
    {
        ApplyContextAdjustments(ContextRules[NewContext]);
    }
}
```

## Performance Metrics and Adaptation

### Performance Monitoring and Adaptation

```cpp
// Performance monitoring and adaptation system
void UAdaptivePerformanceSystem::Initialize()
{
    // Configure metrics to track
    TrackedMetrics.Add(TEXT("FPS"), FMetricSettings{
        TargetValue: 60.0f,
        MinAcceptableValue: 30.0f,
        Weight: 1.0f
    });
    
    TrackedMetrics.Add(TEXT("FrameTime"), FMetricSettings{
        TargetValue: 16.67f,
        MaxAcceptableValue: 33.33f,
        Weight: 1.0f
    });
    
    TrackedMetrics.Add(TEXT("GPUTime"), FMetricSettings{
        TargetValue: 12.0f,
        MaxAcceptableValue: 25.0f,
        Weight: 0.8f
    });
    
    // Configure adaptation settings
    AdaptationSettings.SamplingWindow = 60; // 60 frames
    AdaptationSettings.StabilityThreshold = 0.1f; // 10% deviation
    AdaptationSettings.AdaptationDelay = 5.0f; // 5 seconds between adaptations
    
    // Register for updates
    FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &UAdaptivePerformanceSystem::UpdateAdaptation),
        0.5f // Check twice per second
    );
}

// Update adaptation
bool UAdaptivePerformanceSystem::UpdateAdaptation(float DeltaTime)
{
    // Skip if adaptation is on cooldown
    if (AdaptationCooldown > 0.0f)
    {
        AdaptationCooldown -= DeltaTime;
        return true;
    }
    
    // Calculate performance score
    float PerformanceScore = CalculatePerformanceScore();
    
    // Determine if adaptation is needed
    if (PerformanceScore < -AdaptationSettings.StabilityThreshold)
    {
        // Performance is below target, reduce quality
        ReduceQuality();
        AdaptationCooldown = AdaptationSettings.AdaptationDelay;
    }
    else if (PerformanceScore > AdaptationSettings.StabilityThreshold)
    {
        // Performance is above target, increase quality
        IncreaseQuality();
        AdaptationCooldown = AdaptationSettings.AdaptationDelay;
    }
    
    return true; // Continue ticking
}
```

### User Experience Metrics

```cpp
// User experience metrics system
void UUserExperienceMetrics::Initialize()
{
    // Configure UX metrics
    UXMetrics.Add(TEXT("FrameTimeVariance"), FUXMetricSettings{
        TargetValue: 0.0f,
        MaxAcceptableValue: 5.0f,
        Weight: 0.7f
    });
    
    UXMetrics.Add(TEXT("StutterFrequency"), FUXMetricSettings{
        TargetValue: 0.0f,
        MaxAcceptableValue: 0.05f, // 5% of frames
        Weight: 1.0f
    });
    
    UXMetrics.Add(TEXT("LoadingTime"), FUXMetricSettings{
        TargetValue: 0.0f,
        MaxAcceptableValue: 5.0f, // 5 seconds
        Weight: 0.5f
    });
    
    // Register for updates
    FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &UUserExperienceMetrics::UpdateMetrics),
        1.0f // Check every second
    );
}

// Update UX metrics
bool UUserExperienceMetrics::UpdateMetrics(float DeltaTime)
{
    // Calculate frame time variance
    float FrameTimeVariance = CalculateFrameTimeVariance();
    UXMetricValues.Add(TEXT("FrameTimeVariance"), FrameTimeVariance);
    
    // Calculate stutter frequency
    float StutterFrequency = CalculateStutterFrequency();
    UXMetricValues.Add(TEXT("StutterFrequency"), StutterFrequency);
    
    // Calculate loading time (if applicable)
    if (IsLoading())
    {
        CurrentLoadingTime += DeltaTime;
    }
    else if (WasLoading())
    {
        UXMetricValues.Add(TEXT("LoadingTime"), CurrentLoadingTime);
        CurrentLoadingTime = 0.0f;
    }
    
    // Calculate overall UX score
    float UXScore = CalculateUXScore();
    
    // Log UX issues if score is low
    if (UXScore < 0.7f) // 70% threshold
    {
        LogUXIssues();
    }
    
    return true; // Continue ticking
}
```

## Best Practices

### General Scalability Guidelines

1. **Design for Scalability from the Start**
   - Plan scalability systems early in development
   - Define quality levels for all visual systems
   - Create fallbacks for all high-end features
   - Test on target hardware throughout development

2. **User-Friendly Settings**
   - Provide clear, understandable quality presets
   - Use descriptive names for settings
   - Show visual examples of quality differences
   - Provide performance impact indicators

3. **Automated Quality Selection**
   - Implement hardware detection and benchmarking
   - Auto-select appropriate default settings
   - Allow user overrides of automatic settings
   - Provide guided setup for optimal configuration

4. **Dynamic Adaptation**
   - Implement context-aware quality adjustments
   - Use dynamic resolution for frame rate stability
   - Adjust LOD biases based on performance
   - Scale particle and effect counts dynamically

### Common Pitfalls

1. **Over-Reliance on Resolution Scaling**
   - Balance resolution scaling with other techniques
   - Consider perceptual quality, not just performance
   - Use temporal upscaling where appropriate
   - Combine with other scalability options

2. **Inconsistent Quality Levels**
   - Maintain consistent visual hierarchy across settings
   - Ensure settings have meaningful visual differences
   - Avoid settings with minimal performance impact
   - Test all combinations of settings

3. **Poor Default Settings**
   - Set conservative defaults for unknown hardware
   - Benchmark before finalizing presets
   - Consider different user priorities (quality vs. performance)
   - Validate defaults across hardware spectrum

4. **Ignoring Platform Differences**
   - Customize settings for each platform
   - Account for platform-specific limitations
   - Leverage platform-specific optimizations
   - Test thoroughly on all target platforms

## Integration with Other Systems

The scalability and quality guidelines should be considered alongside:
- [GPU Optimization](GPU_Optimization.md)
- [Content Optimization](Content_Optimization.md)
- [Platform-Specific Optimization](Platform_Specific.md)
- [Tools and Profiling](Tools_And_Profiling.md)
- [Memory Management](Memory_Management.md) 