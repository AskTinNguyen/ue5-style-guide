# UE5 Quick Start Guide

## Introduction
This guide provides the fastest paths to implement common features in our UE5 project. It's designed for new team members and those who need quick reference without diving into detailed documentation.

## Quick Implementation Paths

### Creating a New Enemy

#### Fastest Path (Designer)
1. Open the UE5 Editor
2. Navigate to **Window → Enemy Tools → Create Enemy**
3. Fill in the basic parameters (Name, Health, Damage)
4. Select a base behavior template
5. Click "Generate Enemy"

> **Note:** If you don't see the Enemy Tools panel, enable the plugin from Project Settings → Plugins → Game Features → Enemy Creation Tools

#### Fastest Path (Engineer)
1. Duplicate an existing enemy from `Content/Enemies/Base`
2. Rename all assets following our [naming convention](UE_Pattern_Libraries/UE5-CPP-Style-Guide.md)
3. Modify the behavior tree in `YourEnemy_BT`
4. Update the enemy data table in `Content/DataTables/EnemyStats`

[Detailed Enemy Creation Documentation](Enemy_Content_Design/AAA_Enemy_System.md)

### Creating a New Item

1. Open the Item Creation Dashboard (Window → Item Tools → Create Item)
2. Select item category and base template
3. Configure properties and visuals
4. Click "Generate Item Assets"

[Detailed Item Creation Documentation](Itemization_System/AAA_Itemization_System.md)

### Implementing Combat Abilities

1. Use the Ability Template Creator (Window → Gameplay → Ability Creator)
2. Select base ability type (Melee, Ranged, AoE)
3. Configure damage, cooldown, and effects
4. Generate ability blueprint

[Detailed Combat System Documentation](UE_Pattern_Libraries/Combat_Pattern_Library)

## Common Development Workflows

### Enemy Creation Pipeline
1. **Designer:** Creates enemy using Enemy Creation Dashboard → sets up stats
2. **Engineer:** Adjusts or expands AI behavior → modifies Behavior Tree
3. **QA:** Runs Balance & Testing Tools → checks metrics
4. **Review:** Team iterates until stable

### Item Creation Pipeline
1. **Designer:** Creates item template and stats
2. **Artist:** Implements visual assets
3. **Engineer:** Adds special functionality if needed
4. **QA:** Tests item in gameplay scenarios

## Where to Go Next
- [Enemy System Documentation](Enemy_Content_Design/AAA_Enemy_System.md)
- [Itemization System](Itemization_System/AAA_Itemization_System.md)
- [UE5 Pattern Libraries](UE_Pattern_Libraries)
- [Style Guides](UE_Pattern_Libraries/UE5-CPP-Style-Guide.md) 