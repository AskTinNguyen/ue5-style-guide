# Global Best Practices

This document centralizes common best practices that apply across multiple systems in our UE5 project. Rather than repeating these guidelines in each system's documentation, we reference this central resource.

## Table of Contents
- [Performance Guidelines](#performance-guidelines)
- [Networking Best Practices](#networking-best-practices)
- [Blueprint vs. C++ Guidelines](#blueprint-vs-c-guidelines)
- [Asset Organization](#asset-organization)
- [Testing Procedures](#testing-procedures)

## Performance Guidelines

### Memory Management
- **Object Pooling**: Use object pooling for frequently spawned/destroyed actors (projectiles, effects, enemies)
- **Asset Loading**: Use async loading for large assets; avoid loading assets during gameplay
- **LOD Setup**: All meshes should have proper LOD setups with at least 3 levels
- **Memory Budgets**: Follow the [Memory Budget Guidelines](UE_Pattern_Libraries/General/MemoryBudgetGuidelines.md) for specific memory targets by component

### CPU Optimization
- **Tick Functions**: Minimize tick function usage; use timers or event-driven approaches when possible
- **Collision**: Use simplified collision for gameplay, complex collision only for visuals when necessary
- **AI Optimization**: Stagger AI updates, limit perception checks based on distance/relevance

### GPU Optimization
- **Material Complexity**: Keep material instruction counts under 200 for standard materials
- **Draw Calls**: Merge static meshes where appropriate to reduce draw calls
- **Texture Sizes**: Follow the texture size guidelines in the [Memory Budget Guidelines](UE_Pattern_Libraries/General/MemoryBudgetGuidelines.md#component-level-budgets)

## Networking Best Practices

### Replication
- **Replication Frequency**: Only replicate what's necessary; use NetUpdateFrequency appropriately
- **RPC Usage**: Use reliable RPCs only when necessary; prefer unreliable for frequent updates
- **Authority**: Always check authority before making gameplay-critical decisions

### Bandwidth Optimization
- **Quantization**: Use quantized values for transforms and vectors when full precision isn't needed
- **Relevancy**: Implement NetCullDistanceSquared for actors that don't need long-distance replication
- **Prioritization**: Set appropriate NetPriority values based on gameplay importance

### Latency Handling
- **Client Prediction**: Implement client prediction for player movement and immediate feedback actions
- **Server Reconciliation**: Always reconcile client predictions with server authority
- **Lag Compensation**: Use lag compensation for hit detection and other time-sensitive actions

## Blueprint vs. C++ Guidelines

### When to Use C++
- Core gameplay systems
- Performance-critical code
- Base classes that will be extended
- Complex algorithms
- Systems requiring low-level engine access

### When to Use Blueprints
- UI implementation
- Simple game logic
- Rapid prototyping
- Designer-tunable behaviors
- Visual effects logic

### Hybrid Approach
- Create C++ base classes with Blueprint-exposed functions and properties
- Implement core functionality in C++
- Allow for Blueprint extension and customization

## Asset Organization

### Naming Conventions
- Follow the [UE5 naming convention](UE_Pattern_Libraries/UE5-CPP-Style-Guide.md#naming-convention)
- Use prefixes to indicate asset type (SM_ for Static Mesh, T_ for Texture, etc.)
- Use suffixes to indicate asset purpose (_Diffuse, _Normal, etc.)

### Folder Structure
- Organize by feature rather than asset type
- Keep related assets together (materials with their textures)
- Use consistent folder naming across the project

## Testing Procedures

### Performance Testing
- Run performance tests on target hardware configurations
- Profile CPU and GPU usage regularly
- Test with varying numbers of actors/effects

### Multiplayer Testing
- Test with simulated latency and packet loss
- Verify behavior with multiple clients
- Check edge cases (player disconnection, host migration)

### Content Validation
- Verify all assets meet technical specifications
- Check for missing references or broken dependencies
- Validate against platform-specific requirements

---

**Note**: When documenting specific systems, reference these global best practices rather than repeating them. For example: "Follow the [Global Networking Best Practices](Global_Best_Practices.md#networking-best-practices) when implementing replication for this system." 