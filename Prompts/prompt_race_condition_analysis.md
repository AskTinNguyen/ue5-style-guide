# Race Condition Analysis Prompt
I suspect a race condition in our multiplayer code. Here's the relevant implementation:
[Code snippet]

Symptoms:
[Description of issue]

Please analyze for potential race conditions or networking issues and suggest solutions.
```

## When to Use
- When experiencing intermittent multiplayer bugs
- When state synchronization between clients seems unreliable
- When dealing with networked gameplay systems
- When implementing new multiplayer features

## Example Usage
```
I suspect a race condition in our multiplayer code. Here's the relevant implementation:
```cpp
void AMultiplayerCharacter::PickupItem(APickupActor* Item)
{
    if (HasAuthority())
    {
        Inventory.AddItem(Item->GetItemData());
        OnRep_Inventory();
        Item->Destroy();
    }
    else
    {
        Server_PickupItem(Item);
    }
}

void AMultiplayerCharacter::Server_PickupItem_Implementation(APickupActor* Item)
{
    PickupItem(Item);
}

void AMultiplayerCharacter::OnRep_Inventory()
{
    BP_OnInventoryUpdated();
}
```

Symptoms:
- Sometimes multiple players can pick up the same item
- Occasionally items are picked up on the client but don't appear in server inventory
- UI updates inconsistently between clients

Please analyze for potential race conditions or networking issues and suggest solutions.
```