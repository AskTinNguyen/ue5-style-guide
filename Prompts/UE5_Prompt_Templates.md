# UE5 C++ Prompt Templates for Cursor AI

This document contains a collection of effective prompt templates for UE5 C++ development with Cursor AI. Use these templates as starting points and customize them for your specific needs.

## Table of Contents
- [Project Navigation](#project-navigation)
- [Code Generation](#code-generation)
- [Refactoring](#refactoring)
- [Optimization](#optimization)
- [Debugging](#debugging)
- [Documentation](#documentation)
- [Learning](#learning)

## Project Navigation

### Project Overview Prompt
```
Please review our MasterProjectTracker.md to understand the current state of our UE5 project. Then summarize the key components, current focus, and recent updates.
```

### Codebase Exploration Prompt
```
I'm new to this codebase. Please help me understand the structure of our [System Name] system. Identify the key classes, their relationships, and the overall architecture.
```

### Feature Discovery Prompt
```
Please help me find where the [feature name] functionality is implemented in our codebase. Look for relevant classes and functions related to [specific details].
```

## Code Generation

### New Actor Class Prompt
```
Create a new UE5 Actor class named [ClassName] that will [description of functionality]. The class should:
1. Inherit from [ParentClass]
2. Support [specific feature]
3. Be optimized for [performance goal]
4. Follow our C++ Style Guide
5. Minimize header dependencies for faster rebuilds

Please generate both the header and implementation files.
```

### New Component Class Prompt
```
Create a new UE5 Component class named [ComponentName] that will [description of functionality]. The component should:
1. Be attachable to [ActorType]
2. Provide [specific feature]
3. Be accessible from Blueprints
4. Follow our C++ Style Guide
5. Use a data-driven approach with minimal header dependencies

Please generate both the header and implementation files.
```

### Interface Creation Prompt
```
Design a UE5 Interface named [InterfaceName] for [description of purpose]. The interface should:
1. Define methods for [functionality]
2. Be implementable by both C++ and Blueprint classes
3. Follow our C++ Style Guide
4. Support [specific use case]

Please generate the interface header file.
```

### System Architecture Prompt
```
Design a flexible [system type] system for our UE5 project. The system should:
1. Handle [specific functionality]
2. Be extensible without modifying existing code
3. Follow our C++ Style Guide
4. Use a data-driven approach
5. Minimize rebuild times by isolating implementation details from headers

Please provide the high-level architecture including key classes, their relationships, and example usage.
```

## Refactoring

### Code Structure Improvement Prompt
```
Review this [class/system] code and suggest refactoring to:
1. Reduce header dependencies
2. Improve hot reload compatibility
3. Better follow our C++ Style Guide
4. Enhance performance
5. Increase maintainability

Please provide a step-by-step refactoring plan followed by the implementation.
```

### Implementation Migration Prompt
```
Help me migrate implementation details from this header file to its corresponding .cpp file to reduce rebuild times. The header should maintain the same public interface but with minimal implementation details.
```

### Blueprint Exposure Prompt
```
Modify this C++ class to properly expose its functionality to Blueprints. Make sure:
1. Relevant properties have proper UPROPERTY specifiers
2. Functions that should be callable have proper UFUNCTION specifiers
3. The class can be extended in Blueprints if appropriate
4. Documentation is added for Blueprint users
```

## Optimization

### Performance Review Prompt
```
Review this UE5 C++ code for performance issues, focusing on:
1. CPU usage (algorithms, loops, etc.)
2. Memory usage (allocations, data structures)
3. UE5-specific optimizations (tick functions, delegates)
4. Threading opportunities

Please suggest specific improvements.
```

### Memory Optimization Prompt
```
Analyze the memory usage in this code and suggest optimizations for:
1. Reducing allocations
2. Improving cache coherence
3. Using appropriate UE5 container types
4. Minimizing garbage collection impact
```

### Hot Reload Optimization Prompt
```
Review this code to identify issues that would prevent effective hot reloading. Suggest changes to:
1. Minimize header dependencies
2. Move implementation details to .cpp files
3. Design stable interfaces
4. Use data-driven approaches
```

## Debugging

### Bug Analysis Prompt
```
I'm experiencing the following issue:
[Description of issue]

Here's the relevant code:
[Code snippet]

Error/log output:
[Error messages or logs]

Please help identify the potential causes and suggest solutions that follow our coding standards.
```

### Crash Investigation Prompt
```
We're experiencing crashes in our UE5 game. Here's the crash log:
[Crash log]

Related code:
[Code snippet]

Please analyze the crash, identify potential causes, and suggest fixes.
```

### Race Condition Analysis Prompt
```
I suspect a race condition in our multiplayer code. Here's the relevant implementation:
[Code snippet]

Symptoms:
[Description of issue]

Please analyze for potential race conditions or networking issues and suggest solutions.
```

## Documentation

### Class Documentation Prompt
```
Generate comprehensive documentation for this UE5 C++ class, including:
1. Overall purpose and responsibility
2. Public API usage examples
3. Blueprint interaction guidelines
4. Important implementation details
5. Performance considerations
```

### System Documentation Prompt
```
Create documentation for our [system name] system, including:
1. System overview and purpose
2. Architecture and key components
3. Flow diagrams or sequence explanations
4. Integration guidelines for other systems
5. Best practices for using and extending the system
```

### Code Comment Enhancement Prompt
```
Review these code files and suggest improved comments that:
1. Explain the "why" not just the "what"
2. Document non-obvious behavior
3. Note performance considerations
4. Highlight potential pitfalls
5. Follow our documentation standards
```

## Learning

### Concept Explanation Prompt
```
Explain how [UE5 concept] works in simple terms. Include:
1. Basic description
2. When and why it's used
3. Simple code examples
4. Best practices
5. Common pitfalls
```

### Pattern Recommendation Prompt
```
What UE5 patterns or approaches would you recommend for implementing [functionality]? Please explain:
1. The recommended approach
2. Why it's appropriate for this case
3. Implementation examples
4. Advantages over alternative approaches
5. Potential limitations
```

### Code Review Learning Prompt
```
Review this code for educational purposes and explain:
1. What patterns or techniques are being used
2. How the code interacts with UE5 systems
3. Any UE5-specific optimizations
4. How it could be improved following best practices
5. What I can learn from this implementation
```

---

## How to Use These Templates

1. Copy the appropriate template from this document
2. Customize the placeholders in [brackets] with your specific details
3. Add or remove requirements as needed for your specific case
4. Paste the modified prompt into Cursor AI
5. Review and refine the AI's response

Remember that providing clear, specific prompts with sufficient context will yield the best results. Don't hesitate to follow up with clarification questions if the initial response doesn't fully address your needs. 