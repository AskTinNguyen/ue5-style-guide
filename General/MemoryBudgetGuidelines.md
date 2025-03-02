# Memory Budget Guidelines

This document provides specific memory footprint guidelines for different game elements in UE5 projects. These guidelines help teams set concrete targets and ensure optimal performance across various platforms.

## Table of Contents
- [Overview](#overview)
- [Platform-Specific Budgets](#platform-specific-budgets)
- [Component-Level Budgets](#component-level-budgets)
- [Measurement Methodology](#measurement-methodology)
- [Real-World Examples](#real-world-examples)
- [Performance Benchmarks](#performance-benchmarks)
- [Optimization Techniques](#optimization-techniques)

## Overview

Memory management is critical for maintaining performance in UE5 projects. While the [Global Best Practices](../../Global_Best_Practices.md) document provides general optimization strategies, this document offers specific numeric targets to aim for.

These guidelines should be treated as targets rather than hard limits. Your specific game requirements may necessitate adjustments, but significant deviations should be justified and documented.

## Platform-Specific Budgets

### Total Memory Budgets

| Platform | Recommended Total Memory | Notes |
|----------|--------------------------|-------|
| High-end PC | 8-16 GB | Can scale based on target spec |
| Console (PS5/XSX) | 5-8 GB | Reserve 2-3 GB for system |
| Console (PS4/XB1) | 2-4 GB | Significant optimization required |
| Mobile (High-end) | 1-2 GB | Aggressive streaming required |
| Mobile (Mid-range) | 500-800 MB | Significant visual compromises needed |

### Streaming Budgets

| Platform | Streaming Pool Size | Max Per-Frame Load |
|----------|---------------------|-------------------|
| High-end PC | 1-2 GB | 100-200 MB |
| Console (PS5/XSX) | 800 MB - 1.2 GB | 50-100 MB |
| Console (PS4/XB1) | 300-500 MB | 20-50 MB |
| Mobile (High-end) | 100-200 MB | 10-20 MB |
| Mobile (Mid-range) | 50-100 MB | 5-10 MB |

## Component-Level Budgets

### Character Systems

| Component | Budget | Notes |
|-----------|--------|-------|
| Player Character | 50-100 MB | Includes mesh, textures, animations, materials |
| Main NPC | 30-50 MB | Important story characters |
| Standard Enemy | 10-20 MB | Per enemy type, not instance |
| Enemy Instance | 1-2 MB | Additional memory per spawned instance |
| Character Manager | 5-10 MB | Overhead for character system |

### AI Systems

| Component | Budget | Notes |
|-----------|--------|-------|
| AI Director | 20-30 MB | Central AI coordination system |
| AI Memory | 2-5 MB per 50 enemies | For perception and knowledge representation |
| Behavior Trees | 1-2 MB per active tree | Memory usage scales with complexity |
| Pathfinding | 10-20 MB base + 0.5 MB per active AI | Depends on world size and complexity |
| Group Coordination | 1-2 MB per group | For squad-based AI |

### Visual Effects

| Component | Budget | Notes |
|-----------|--------|-------|
| Particle System | 20-50 MB per scene | Total budget for all active effects |
| Major Effect | 5-10 MB | Large explosions, environmental effects |
| Medium Effect | 2-5 MB | Standard combat effects, weather |
| Minor Effect | 0.5-1 MB | Small impacts, footsteps, etc. |
| Effect Manager | 5-10 MB | System overhead |

### Environment

| Component | Budget | Notes |
|-----------|--------|-------|
| Landscape | 50-200 MB | Varies with terrain size and detail |
| Static Meshes | 100-300 MB | For currently visible area |
| Foliage | 50-150 MB | Depends on density and variety |
| Lighting Data | 50-200 MB | For baked lighting |
| Post-Processing | 10-30 MB | For all post-process effects |

### UI Systems

| Component | Budget | Notes |
|-----------|--------|-------|
| Main HUD | 10-20 MB | Always-visible interface elements |
| Menus | 20-40 MB | For all menu screens combined |
| UI Animations | 5-10 MB | For animated UI elements |
| UI Textures | 20-50 MB | All UI textures combined |

### Audio

| Component | Budget | Notes |
|-----------|--------|-------|
| Active Sounds | 20-50 MB | For all currently playing sounds |
| Sound Banks | 50-100 MB | Loaded but not playing |
| Audio Engine | 10-20 MB | System overhead |

## Measurement Methodology

To ensure consistent measurement of memory usage:

### Tools to Use

1. **UE Memory Report**: Access via `stat memory` console command
2. **Memory Profiler**: Use the built-in UE Memory Profiler (Window → Developer Tools → Memory Profiler)
3. **Platform-Specific Tools**:
   - Windows: Task Manager, RAMMap
   - Console: Platform-specific dev tools
   - Mobile: Platform profiling tools

### Measurement Process

1. **Baseline Measurement**:
   - Measure an empty level with minimal systems active
   - Document this as your baseline

2. **Component Isolation**:
   - Measure individual components in isolation when possible
   - Calculate the delta from baseline

3. **Real-World Scenarios**:
   - Measure during typical gameplay scenarios
   - Document peak memory usage during intensive sequences

4. **Regular Profiling Schedule**:
   - Profile weekly during development
   - Document trends and investigate regressions

## Real-World Examples

### Example 1: Combat Encounter

A typical combat encounter with 10 enemies, effects, and UI elements:

| Component | Memory Usage |
|-----------|--------------|
| Player | 80 MB |
| 10 Standard Enemies | 120 MB (10-12 MB per instance) |
| Combat Effects | 30 MB |
| Combat UI | 15 MB |
| Environment | 200 MB |
| Audio | 40 MB |
| **Total** | **485 MB** |

### Example 2: Open World Exploration

Typical memory usage during open world exploration:

| Component | Memory Usage |
|-----------|--------------|
| Player | 80 MB |
| NPCs (5 visible) | 100 MB |
| Environment | 350 MB |
| Streaming Buffer | 150 MB |
| UI | 25 MB |
| Audio | 50 MB |
| **Total** | **755 MB** |

## Performance Benchmarks

In addition to memory budgets, the following performance benchmarks should be used to guide optimization efforts. These benchmarks work in conjunction with memory guidelines to ensure both memory efficiency and runtime performance.

### Frame Rate Benchmarks

| Target | Budget | Notes |
|--------|--------|-------|
| 30 FPS | 33.3ms per frame | Console/mobile baseline |
| 60 FPS | 16.7ms per frame | Standard for most platforms |
| 120 FPS | 8.3ms per frame | Competitive/high-end PC |

#### Frame Time Distribution

| System | Budget (60 FPS) | Notes |
|--------|----------------|-------|
| Rendering | 8-10ms | Includes all rendering passes |
| Gameplay | 2-3ms | Game logic, AI, etc. |
| Physics | 2-3ms | Collision, rigid bodies |
| Animation | 1-2ms | Skeletal mesh updates |
| Particles/VFX | 1-2ms | All particle systems |
| UI | 0.5-1ms | HUD and menus |

### CPU Performance Benchmarks

| Metric | Target | Notes |
|--------|--------|-------|
| Main Thread Utilization | <80% | Leave headroom for spikes |
| Worker Thread Utilization | <90% | Distributed workload |
| AI Updates | <2ms total | All active AI agents |
| Physics Simulation | <3ms | All physics objects |
| Blueprint Execution | <2ms | All Blueprint scripts |

### GPU Performance Benchmarks

| Metric | Target | Notes |
|--------|--------|-------|
| Draw Calls | 2,000-5,000 | Per frame |
| Triangles | 1-3 million | Visible per frame |
| Material Instructions | <200 | Per standard material |
| <500 | For complex materials |
| Texture Samples | <8 | Per standard material |
| Fill Rate | <70% | Of GPU capability |

### Asset-Specific Benchmarks

| Asset Type | Metric | Target | Notes |
|------------|--------|--------|-------|
| Characters | Triangles | 15,000-50,000 | Main characters |
| | | 5,000-15,000 | NPCs/enemies |
| | Bones | 80-150 | Main characters |
| | | 30-80 | NPCs/enemies |
| Static Meshes | Triangles | 500-10,000 | Per mesh |
| | LODs | 3-5 | Per mesh |
| Textures | Resolution | 2048×2048 | Character albedo |
| | | 1024×1024 | Environment albedo |
| | | 4096×4096 | Terrain |
| Materials | Instruction Count | <200 | Standard |
| | | <500 | Complex |
| | Texture Samples | 4-8 | Per material |

### System-Specific Benchmarks

| System | Metric | Target | Notes |
|--------|--------|--------|-------|
| AI | Update Frequency | 0.1-0.5s | Per agent |
| | Pathfinding | <1ms | Per request |
| | Active Agents | 50-100 | In memory |
| Networking | Bandwidth | <256 KB/s | Per client |
| | Packet Size | <1200 bytes | To avoid fragmentation |
| | Update Frequency | 20-60 Hz | For critical objects |
| Audio | Active Voices | 50-100 | Simultaneous |
| | Memory | 50-100 MB | All active audio |

### Loading Time Benchmarks

| Loading Type | Target | Notes |
|--------------|--------|-------|
| Initial Load | <30 seconds | From launch to gameplay |
| Level Transition | <5-15 seconds | Between major areas |
| Streaming | <100ms | Per asset group |
| | No hitching | During gameplay |

### Measurement Tools

1. **Unreal Insights**: For detailed CPU profiling and event timing
   - Enable via Project Settings → Engine → Performance
   - Analyze with Unreal Insights Trace Viewer

2. **GPU Visualizer**: For shader and rendering performance analysis
   - Access via `Ctrl+Shift+,` in editor
   - Use for material optimization

3. **Stat Commands**: In-game overlay for real-time performance metrics
   - `stat fps` - Frame rate statistics
   - `stat unit` - Frame time breakdown
   - `stat gpu` - GPU timing information
   - `stat game` - Game thread timing
   - `stat memory` - Memory usage statistics
   - `stat particles` - Particle system performance
   - `stat audio` - Audio system performance

4. **Automation Testing**: For consistent benchmarking
   - Create performance test maps
   - Implement automated performance tests
   - Track metrics over time

### Benchmark Sources

These benchmarks are derived from:
- Industry standards for AAA game development
- Unreal Engine documentation and recommendations
- Platform-specific developer guidelines
- Real-world performance analysis of successful titles

These benchmarks should be adjusted based on:
- Your specific game requirements
- Target hardware specifications
- Genre-specific performance expectations
- Player experience priorities

## Optimization Techniques

For each component that exceeds its memory budget, consider these optimization techniques:

### Character Optimization

- [Implement LOD systems](../../Character_Pattern_Library/Pattern4_CharacterCustomization.md)
- Use texture atlasing for character materials
- Share animations between similar character types
- Implement mesh merging for static character parts

### AI Optimization

- [Implement AI LOD systems](../../AI_Pattern_Library/Pattern1_BehaviorTreeFramework.md)
- Use simplified behavior trees for distant enemies
- Share perception data between AI instances
- Implement spatial partitioning for AI perception

### Visual Effects Optimization

- Implement aggressive effect pooling
- Use LOD systems for distant effects
- Share effect textures and materials
- Limit maximum concurrent effects

### Environment Optimization

- Implement hierarchical LOD (HLOD)
- Use texture streaming
- Implement occlusion culling
- Use instanced static meshes for repeated elements

### UI Optimization

- Implement texture atlasing for UI elements
- Unload unused UI screens
- Use vector graphics where appropriate
- Minimize UI animations in memory-constrained scenarios

## Conclusion

These memory budgets provide targets to aim for during development. Regular profiling and optimization should be part of the development workflow to ensure these targets are met.

For additional optimization strategies, refer to the [Global Best Practices](./Global_Best_Practices.md) document. 