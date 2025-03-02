# Global Best Practices

This document centralizes common best practices that apply across multiple systems in our UE5 project. Rather than repeating these guidelines in each system's documentation, we reference this central resource.

## Table of Contents
- [Performance Guidelines](#performance-guidelines)
- [Networking Best Practices](#networking-best-practices)
- [Blueprint vs. C++ Guidelines](#blueprint-vs-c-guidelines)
- [Asset Organization](#asset-organization)
- [Testing Procedures](#testing-procedures)
- [AI-Assisted Development](#ai-assisted-development)

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

## AI-Assisted Development

### Cursor AI IDE Integration

#### Code Generation Best Practices
- **Header File Design**: Design headers to minimize rebuild time when changing implementation
- **Interface Stability**: Create stable interfaces that can be extended without header changes
- **Container-Based Properties**: Use TMap, TArray, and other containers to allow flexible implementation details
- **Data-Driven Design**: Prefer data-driven approaches that can be modified without recompiling

#### Minimizing Rebuilds
- **Live Coding Rules**: Follow the [UE5 Live Coding Rules](General/LiveCoding/UE5_LiveCoding_Rules.md) to maximize live coding compatibility
- **Implementation Location**: Keep logic in .cpp files, interfaces and declarations in .h files
- **Module Organization**: Organize code into modules with stable interfaces and implementation separation
- **Forward Declarations**: Use forward declarations instead of including full headers where possible
- **PCH Management**: Properly manage precompiled headers to reduce build times

#### Making C++ Accessible for Non-Technical Team Members
- **Template Patterns**: Create reusable template patterns that can be copied and modified
- **Commenting Standards**: Add clear, concise comments explaining intent, not just implementation
- **Working Examples**: Provide working examples for each pattern in its own folder
- **AI Prompting Guide**: Maintain an AI prompting guide with effective prompts for common tasks

### AI-Assisted Workflow
- **Session Structure**: Begin each Cursor AI coding session by referencing the MasterProjectTracker.md
- **Incremental Changes**: Make small, targeted changes that can be validated quickly
- **Documentation First**: Document intent and design before coding to guide AI assistance
- **Code Review**: Use AI to review code for style guide compliance before submitting

### Collaborative Development
- **Prompt Sharing**: Create a repository of effective AI prompts for common development tasks
- **Pair Programming**: Use AI as a pair programming partner, especially for exploring new patterns
- **Knowledge Transfer**: Document AI-assisted solutions for future reference
- **Educational Opportunities**: Use AI to explain complex code to non-technical team members

---

**Note**: When documenting specific systems, reference these global best practices rather than repeating them. For example: "Follow the [Global Networking Best Practices](Global_Best_Practices.md#networking-best-practices) when implementing replication for this system." 