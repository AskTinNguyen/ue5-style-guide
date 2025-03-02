Bug Analysis Prompt
I'm experiencing the following issue:
[Description of issue]

Here's the relevant code:
[Code snippet]

Error/log output:
[Error messages or logs]

Please help identify the potential causes and suggest solutions that follow our coding standards.
```

## Purpose
This prompt helps you identify and fix bugs in your UE5 code by providing a structured format for describing issues and getting solutions.

## When to Use
- When encountering runtime errors or crashes
- When behavior differs from expectations
- When compile errors are difficult to understand
- When logs show warnings or errors you need help interpreting

## Example Usage
```
I'm experiencing the following issue:
Characters are taking damage twice when hit by a single projectile.

Here's the relevant code:
```cpp
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, DamageType);
        
        if (OtherActor->Implements<UDamageable>())
        {
            IDamageable::Execute_TakeDamage(OtherActor, Damage, DamageType, GetInstigatorController(), this);
        }
    }
    
    Destroy();
}
```

Error/log output:
```
LogCombat: Warning: Actor PlayerCharacter_2 received 50 damage from Projectile_7
LogCombat: Warning: Actor PlayerCharacter_2 received 50 damage from Projectile_7
```

Please help identify the potential causes and suggest solutions that follow our coding standards.