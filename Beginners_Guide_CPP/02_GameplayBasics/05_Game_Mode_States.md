# Game Mode and States

## Overview

This guide covers setting up game modes and states for your hack and slash game. We'll implement a robust state management system that handles different game phases, player spawning, and game rules.

## Prerequisites

Before starting this guide, you should:
- Have completed [Movement and Animation Fundamentals](04_Movement_Animation_Basics.md)
- Understand basic UE5 game framework concepts
- Be familiar with UE5's replication system

## Basic Game Mode Setup

### 1. Create Game Mode Class

```cpp
// HackAndSlashGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HackAndSlashGameMode.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    PreGame,
    Playing,
    Paused,
    PostGame
};

UCLASS()
class MYGAME_API AHackAndSlashGameMode : public AGameModeBase
{
    GENERATED_BODY()
    
public:
    AHackAndSlashGameMode();
    
    virtual void StartPlay() override;
    virtual void HandleMatchHasStarted() override;
    
protected:
    UPROPERTY(BlueprintReadOnly, Category = "Game")
    EGamePhase CurrentPhase;
    
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    float PreGameDuration;
    
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    TSubclassOf<class AHackAndSlashCharacter> DefaultCharacterClass;
    
    // Game flow functions
    void StartPreGamePhase();
    void StartPlayingPhase();
    void PauseGame();
    void ResumeGame();
    void EndGame();
};
```

### 2. Implement Game Mode

```cpp
// HackAndSlashGameMode.cpp
#include "HackAndSlashGameMode.h"
#include "HackAndSlashCharacter.h"
#include "HackAndSlashPlayerController.h"
#include "HackAndSlashGameState.h"

AHackAndSlashGameMode::AHackAndSlashGameMode()
{
    // Set default classes
    DefaultPawnClass = AHackAndSlashCharacter::StaticClass();
    PlayerControllerClass = AHackAndSlashPlayerController::StaticClass();
    GameStateClass = AHackAndSlashGameState::StaticClass();
    
    // Set default values
    PreGameDuration = 3.0f;
    CurrentPhase = EGamePhase::PreGame;
}

void AHackAndSlashGameMode::StartPlay()
{
    Super::StartPlay();
    StartPreGamePhase();
}

void AHackAndSlashGameMode::StartPreGamePhase()
{
    CurrentPhase = EGamePhase::PreGame;
    
    // Start countdown to game start
    FTimerHandle PreGameTimer;
    GetWorldTimerManager().SetTimer(
        PreGameTimer,
        this,
        &AHackAndSlashGameMode::StartPlayingPhase,
        PreGameDuration,
        false
    );
}

void AHackAndSlashGameMode::StartPlayingPhase()
{
    CurrentPhase = EGamePhase::Playing;
    HandleMatchHasStarted();
}
```

## Game State Management

### 1. Create Game State Class

```cpp
// HackAndSlashGameState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HackAndSlashGameState.generated.h"

UCLASS()
class MYGAME_API AHackAndSlashGameState : public AGameStateBase
{
    GENERATED_BODY()
    
public:
    AHackAndSlashGameState();
    
    // Game state properties
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    float GameTime;
    
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    int32 EnemiesDefeated;
    
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    bool bIsBossFightActive;
    
    // Game state functions
    void UpdateGameTime(float DeltaTime);
    void IncrementEnemiesDefeated();
    void SetBossFightActive(bool bActive);
    
protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION()
    void OnRep_BossFightActive();
};
```

### 2. Implement Game State

```cpp
// HackAndSlashGameState.cpp
#include "HackAndSlashGameState.h"
#include "Net/UnrealNetwork.h"

void AHackAndSlashGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AHackAndSlashGameState, GameTime);
    DOREPLIFETIME(AHackAndSlashGameState, EnemiesDefeated);
    DOREPLIFETIME(AHackAndSlashGameState, bIsBossFightActive);
}

void AHackAndSlashGameState::UpdateGameTime(float DeltaTime)
{
    if (HasAuthority())
    {
        GameTime += DeltaTime;
    }
}

void AHackAndSlashGameState::IncrementEnemiesDefeated()
{
    if (HasAuthority())
    {
        EnemiesDefeated++;
    }
}

void AHackAndSlashGameState::SetBossFightActive(bool bActive)
{
    if (HasAuthority())
    {
        bIsBossFightActive = bActive;
        OnRep_BossFightActive();
    }
}
```

## Player State Management

### 1. Create Player State Class

```cpp
// HackAndSlashPlayerState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HackAndSlashPlayerState.generated.h"

UCLASS()
class MYGAME_API AHackAndSlashPlayerState : public APlayerState
{
    GENERATED_BODY()
    
public:
    AHackAndSlashPlayerState();
    
    // Player statistics
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    int32 EnemiesKilled;
    
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    int32 DeathCount;
    
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float DamageDealt;
    
    // Player state functions
    void AddEnemyKill();
    void IncrementDeathCount();
    void AddDamageDealt(float Damage);
    
protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

## Save Game System

### 1. Create Save Game Class

```cpp
// HackAndSlashSaveGame.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HackAndSlashSaveGame.generated.h"

UCLASS()
class MYGAME_API UHackAndSlashSaveGame : public USaveGame
{
    GENERATED_BODY()
    
public:
    UHackAndSlashSaveGame();
    
    UPROPERTY()
    FString PlayerName;
    
    UPROPERTY()
    FTransform PlayerTransform;
    
    UPROPERTY()
    int32 TotalEnemiesKilled;
    
    UPROPERTY()
    float PlayTime;
    
    UPROPERTY()
    TArray<FString> UnlockedAbilities;
};
```

### 2. Implement Save/Load System

```cpp
// HackAndSlashGameMode.cpp
void AHackAndSlashGameMode::SaveGame()
{
    if (UHackAndSlashSaveGame* SaveGameInstance = Cast<UHackAndSlashSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UHackAndSlashSaveGame::StaticClass())))
    {
        // Get player data
        AHackAndSlashCharacter* PlayerCharacter = Cast<AHackAndSlashCharacter>(
            UGameplayStatics::GetPlayerCharacter(this, 0));
            
        if (PlayerCharacter)
        {
            SaveGameInstance->PlayerTransform = PlayerCharacter->GetActorTransform();
            
            if (AHackAndSlashPlayerState* PS = PlayerCharacter->GetPlayerState<AHackAndSlashPlayerState>())
            {
                SaveGameInstance->TotalEnemiesKilled = PS->EnemiesKilled;
            }
        }
        
        // Save the game
        if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MainSave"), 0))
        {
            UE_LOG(LogTemp, Log, TEXT("Game saved successfully"));
        }
    }
}

void AHackAndSlashGameMode::LoadGame()
{
    if (UHackAndSlashSaveGame* LoadedGame = Cast<UHackAndSlashSaveGame>(
        UGameplayStatics::LoadGameFromSlot(TEXT("MainSave"), 0)))
    {
        // Apply loaded data
        AHackAndSlashCharacter* PlayerCharacter = Cast<AHackAndSlashCharacter>(
            UGameplayStatics::GetPlayerCharacter(this, 0));
            
        if (PlayerCharacter)
        {
            PlayerCharacter->SetActorTransform(LoadedGame->PlayerTransform);
            
            if (AHackAndSlashPlayerState* PS = PlayerCharacter->GetPlayerState<AHackAndSlashPlayerState>())
            {
                PS->EnemiesKilled = LoadedGame->TotalEnemiesKilled;
            }
        }
    }
}
```

## Game Flow Control

### 1. Implement Game Flow Functions

```cpp
// HackAndSlashGameMode.cpp
void AHackAndSlashGameMode::PauseGame()
{
    if (CurrentPhase == EGamePhase::Playing)
    {
        CurrentPhase = EGamePhase::Paused;
        UGameplayStatics::SetGamePaused(GetWorld(), true);
        
        // Notify all players
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (AHackAndSlashPlayerController* PC = Cast<AHackAndSlashPlayerController>(*It))
            {
                PC->OnGamePaused();
            }
        }
    }
}

void AHackAndSlashGameMode::ResumeGame()
{
    if (CurrentPhase == EGamePhase::Paused)
    {
        CurrentPhase = EGamePhase::Playing;
        UGameplayStatics::SetGamePaused(GetWorld(), false);
        
        // Notify all players
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (AHackAndSlashPlayerController* PC = Cast<AHackAndSlashPlayerController>(*It))
            {
                PC->OnGameResumed();
            }
        }
    }
}

void AHackAndSlashGameMode::EndGame()
{
    CurrentPhase = EGamePhase::PostGame;
    
    // Save final game state
    SaveGame();
    
    // Notify all players
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AHackAndSlashPlayerController* PC = Cast<AHackAndSlashPlayerController>(*It))
        {
            PC->OnGameEnded();
        }
    }
}
```

## Common Issues and Solutions

### State Synchronization

1. Ensure proper replication setup
2. Validate authority checks
3. Handle network latency

### Save Game Corruption

1. Implement save data validation
2. Add backup save system
3. Handle failed save/load operations

### Game Flow Issues

1. Add state transition validation
2. Implement proper error handling
3. Add logging for debugging

## Next Steps

Now that you have a solid game mode and state system, you can:

1. Implement combat mechanics in [Basic Attack System](../03_CombatSystems/01_Basic_Attack_System.md)
2. Add UI elements in [HUD and UI Setup](../05_UI/01_HUD_Setup.md)
3. Implement multiplayer features in [Multiplayer Basics](../06_Multiplayer/01_Multiplayer_Setup.md) 