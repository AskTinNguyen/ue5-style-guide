# Enemy Creator Tool - Quick Start Guide

## Getting Started

### 1. Opening the Tool
1. Open Unreal Editor
2. Go to Window > Enemy Creator Tool
3. The tool will open in a new window with three main sections:
   - Template Browser (left)
   - Preview Viewport (center)
   - Property Editor (right)

### 2. Creating Your First Enemy (5-Minute Guide)

#### Step 1: Create a Template
1. Click "New Template" in the Template Browser
2. Choose a template type:
   - Standard (regular enemies)
   - Elite (stronger variants)
   - Boss (unique boss enemies)
3. Enter a name for your enemy

#### Step 2: Quick Setup with Presets
1. Choose a preset to start with:
   - Balanced (well-rounded stats)
   - Aggressive (high damage, lower health)
   - Defensive (high health, lower damage)
2. Customize the preset values in the Property Editor

#### Step 3: AI Behavior Setup
1. Create a new Behavior Tree
2. Apply behavior patterns:
   - Patrol Pattern
   - Aggressive Pattern
   - Defensive Pattern
   - Support Pattern
3. Customize the behavior tree nodes as needed

#### Step 4: Add Abilities
1. Open the Ability Editor
2. Choose from common ability types:
   - Melee Attacks
   - Ranged Attacks
   - Movement Abilities
   - Special Abilities
3. Configure ability parameters

#### Step 5: Preview and Test
1. Use the Preview Viewport to:
   - View the enemy model
   - Play animations
   - Test AI behavior
2. Enable debug visualization:
   - Show combat radius
   - Display ability ranges
   - View AI debug info

### 3. Quick Tips

#### Behavior Pattern Tips
- Start with a base pattern
- Customize for your needs
- Test in different scenarios
- Combine patterns for complexity

#### Template Management
- Start with base templates
- Create variants for different difficulties
- Use inheritance for similar enemies
- Document your templates

#### Testing Checklist
- [ ] Basic movement works
- [ ] Animations play correctly
- [ ] AI behaves as expected
- [ ] Abilities function properly
- [ ] Performance is acceptable

### 4. Common Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+N | New Template |
| Ctrl+S | Save Template |
| Ctrl+P | Preview Mode |
| Ctrl+D | Toggle Debug View |
| Space | Play/Stop Animation |
| R | Reset Preview |

### 5. Getting Help

- Press F1 for full documentation
- Right-click any property for quick help
- Use the "Validate" button to check for issues
- Check template examples in the content browser

### 6. Next Steps

After mastering the basics:
1. Create custom behavior patterns
2. Design complex ability combinations
3. Build multi-phase boss battles
4. Create enemy families with shared traits
5. Use data tables for batch enemy creation

## Quick Reference

### Common Behavior Patterns
```
Patrol: Basic patrolling and area control
Aggressive: Direct combat and pursuit
Defensive: Strategic positioning and counter-attacks
Support: Team-based tactics and buffs
```

### Useful Property Ranges
- Health: 50-1000
- Damage: 10-100
- Speed: 300-600
- Attack Range: 100-1000
- Aggression: 0.0-1.0

### Template Categories
- **Standard**: Basic enemies (70% of encounters)
- **Elite**: Stronger variants (25% of encounters)
- **Boss**: Unique bosses (5% of encounters)

### Debug Options
- AI State
- Combat Range
- Ability Ranges
- Path Finding
- Performance Stats

Remember: The Enemy Creator Tool is designed for direct development in the Cursor IDE. Take advantage of the preset systems and behavior patterns to quickly create and iterate on enemy designs. 