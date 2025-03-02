# Visual System Diagrams

This document contains visual diagrams for major systems in our UE5 project. These diagrams are designed to help new team members quickly understand complex systems without having to read extensive documentation.

## AI Flow Diagram

```mermaid
flowchart TD
    A[Enemy Spawned] --> B{Has Target?}
    B -->|Yes| C[Engage Target]
    B -->|No| D[Patrol/Idle]
    D --> E{Detect Player?}
    E -->|Yes| B
    E -->|No| D
    C --> F{In Attack Range?}
    F -->|Yes| G[Attack]
    F -->|No| H[Move to Target]
    H --> F
    G --> I{Target Dead?}
    I -->|Yes| J[Return to Patrol]
    I -->|No| C
    J --> D
```

## Combat State Transitions

```mermaid
stateDiagram-v2
    [*] --> Idle
    Idle --> Combat: Player Detected
    Combat --> Attacking: In Range
    Combat --> Pursuing: Out of Range
    Attacking --> Cooldown: Attack Complete
    Cooldown --> Attacking: Cooldown Complete
    Pursuing --> Attacking: Reached Target
    Attacking --> Fleeing: Health Low
    Fleeing --> Combat: Health Recovered
    Combat --> Idle: Target Lost
    Fleeing --> Idle: Escape Successful
```

## Item Creation Pipeline

```mermaid
flowchart LR
    A[Design Item] --> B[Create Base Asset]
    B --> C[Configure Properties]
    C --> D[Add Visual Components]
    D --> E[Implement Special Behavior]
    E --> F[Test in Game]
    F --> G{Issues?}
    G -->|Yes| H[Revise]
    H --> F
    G -->|No| I[Release to Content]
```

## Replication Architecture

```mermaid
flowchart TD
    A[Server] --> B[Authority Game State]
    B --> C{Replicate To Clients}
    C --> D[Client 1]
    C --> E[Client 2]
    C --> F[Client n]
    D --> G[Input]
    G --> H[Client Prediction]
    H --> I[Send to Server]
    I --> A
```

## How to Use These Diagrams

These diagrams are meant to provide a high-level overview of complex systems. For detailed implementation guidelines, refer to the specific documentation for each system:

- [AI System Documentation](UE_Pattern_Libraries/AI_Pattern_Library)
- [Combat System Documentation](UE_Pattern_Libraries/Combat_Pattern_Library)
- [Itemization Documentation](Itemization_System/AAA_Itemization_System.md)
- [Networking Best Practices](Global_Best_Practices.md#networking)

## Adding New Diagrams

When adding new features or systems to the project, consider creating a visual diagram to help other team members understand the system quickly. You can use [Mermaid](https://mermaid-js.github.io/) syntax as shown above, or add image files to the repository. 