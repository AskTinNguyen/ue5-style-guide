# Cursor AI for UE5 C++ Development

This guide provides comprehensive instructions and best practices for using Cursor AI to accelerate and improve C++ development in Unreal Engine 5 projects.

## Table of Contents
- [Introduction](#introduction)
- [Setup](#setup)
- [Optimized Workflow](#optimized-workflow)
- [Best Practices](#best-practices)
- [Effective Prompting Strategies](#effective-prompting-strategies)
- [Common Use Cases](#common-use-cases)
- [Troubleshooting](#troubleshooting)

## Introduction

Cursor AI is a powerful AI-powered development environment that can significantly accelerate UE5 C++ development. This guide focuses on how to effectively use Cursor AI to:

1. Reduce development time for both technical and non-technical team members
2. Minimize unnecessary project rebuilds
3. Maintain code quality and consistency
4. Facilitate knowledge sharing and learning

## Setup

### Prerequisites
- Cursor AI IDE installed
- UE5 project set up with C++ support
- Basic understanding of UE5 C++ structure

### Integrating with UE5
1. **Project Structure**: Ensure your project follows the [Live Coding Rules](../General/LiveCoding/UE5_LiveCoding_Rules.md)
2. **IDE Configuration**:
   - Configure Cursor AI to work with your UE5 project
   - Set up project-specific settings
   - Ensure path mappings are correct

### Custom Instructions Setup
Add this snippet to your custom instructions in Cursor AI:
```
Always refer to the MasterProjectTracker.md at the start of every Composer Session, to have a universal understanding and overview of our project progress for every coding session.
Always check what has already been installed in package.json before triggering any duplicated install commands.
```

## Optimized Workflow

### Starting a Session
1. Begin each session by asking Cursor AI to review MasterProjectTracker.md
2. Familiarize Cursor AI with the current state of relevant systems
3. Clearly define the task and expected outcome

### Minimizing Rebuilds
1. **Header Protection**: Isolate implementation details from headers
   ```cpp
   // ✅ GOOD: Stable header
   UCLASS()
   class MYGAME_API AMyActor : public AActor
   {
       GENERATED_BODY()
       
   public:
       AMyActor();
       
       // Use stable interface methods
       UFUNCTION(BlueprintCallable)
       void PerformAction(const FName ActionName);
       
   private:
       // Use containers instead of explicit members
       UPROPERTY()
       TMap<FName, FActionData> Actions;
   };
   
   // ❌ BAD: Unstable header with implementation details
   UCLASS()
   class MYGAME_API AMyActor : public AActor
   {
       GENERATED_BODY()
       
   public:
       AMyActor();
       
       // Specific methods that may change
       UFUNCTION(BlueprintCallable)
       void Attack();
       
       UFUNCTION(BlueprintCallable)
       void Defend();
       
   private:
       // Explicit members that may need to change
       UPROPERTY()
       float AttackPower;
       
       UPROPERTY()
       float DefensePower;
   };
   ```

2. **Implementation Focus**: Keep changes in .cpp files when possible
3. **Hot Reload Planning**: Plan changes to leverage UE5's hot reload capabilities
4. **Module Structure**: Organize code into modules with stable interfaces

### Incremental Development
1. Make small, targeted changes
2. Test frequently
3. Commit working code often
4. Use branch protection for experimental features

## Best Practices

### Code Generation

#### Template-Based Generation
Provide Cursor AI with templates for common patterns

#### Style Guide Enforcement
Remind Cursor AI to follow the project's style guide

### Making C++ Accessible

#### Non-Technical User Workflow
1. Define clear intent and desired functionality
2. Use natural language to describe the feature
3. Let Cursor AI translate intent into proper UE5 C++ code
4. Review generated code with explanations
5. Implement and test incrementally


## Effective Prompting Strategies

### System Architecture Prompts
```
Design a flexible [system type] system for UE5 using C++ that follows 
our live coding best practices. The system should be able to [requirements].

Key considerations:
1. Minimize header changes
2. Use data-driven approach
3. Support extension without modifying existing code
4. Follow our naming conventions
```

### Implementation Prompts
```
Implement a C++ function that [functionality description]. 
This should follow our UE5 C++ Style Guide and live coding best practices.
The implementation should go in the .cpp file only and not require header changes.
```

### Optimization Prompts
```
Review this UE5 C++ code for performance issues, focusing on:
1. Memory usage
2. CPU optimization
3. UE5-specific optimizations
4. Potential rebuild triggers

Please provide suggestions for improvements that maintain our coding standards.
```

## Common Use Cases

### Implementing Game Features

Example prompt:
```
I need to implement a [feature name] system in our UE5 project. 
The system should [description of functionality].
Please suggest a C++ architecture that follows our live coding best practices 
and minimizes unnecessary rebuilds. Then help me implement it step by step.
```

### Debugging Code
Use this workflow for debugging with Cursor AI:
1. Describe the issue and expected behavior
2. Share relevant code snippets and logs
3. Ask for analysis and potential solutions
4. Implement fixes incrementally

Example prompt:
```
I'm experiencing an issue with [feature/system]. The expected behavior is 
[description], but I'm seeing [actual behavior]. Here's the relevant code 
and error messages. Can you help identify the issue and suggest fixes that
follow our coding standards?
```

### Code Refactoring
For refactoring existing code:
1. Explain the current code structure and issues
2. Define the refactoring goals
3. Ask for a step-by-step refactoring plan
4. Implement changes incrementally to maintain stability

Example prompt:
```
I need to refactor our [system/feature] to improve [specific aspect]. 
The current implementation has [issues/limitations]. Can you help me create 
a refactoring plan that minimizes rebuilds and follows our coding standards?
```

## Troubleshooting

### Common Issues and Solutions

#### AI Generates Inefficient Code
1. Provide more specific requirements
2. Reference existing patterns from your codebase
3. Ask for optimization after getting basic functionality

#### Build Errors After AI-Generated Changes
1. Check header dependencies
2. Verify include order
3. Ensure proper forward declarations
4. Validate UE5 macro usage

#### Hot Reload Failures
1. Check for changes that aren't hot-reload compatible
2. Move changes to .cpp files when possible
3. Split large changes into smaller increments
4. Use Live Coding feature when appropriate

## Conclusion

Cursor AI can significantly accelerate UE5 C++ development while making it more accessible to non-technical team members. By following these guidelines, you'll maximize the benefits while avoiding common pitfalls.

Remember that AI assistance is a tool to enhance your development process, not replace good software engineering practices. Combine AI assistance with proper planning, testing, and code review to achieve the best results.

---

For specific questions or assistance, refer to our team's documentation or reach out to the development leads. 