# Game Vision

## Core Statement

`Last Input` is a **shooter with rail mechanics**.

This does **not** mean literal train rails, tracks, or a vehicle on a fixed line.
It means the combat is structured around a **guided flow**:

- the player still feels agency;
- the level pushes the player through directed combat beats;
- enemy pressure, space, and encounter pacing create a clear forward path;
- the interface and systems must constantly reinforce momentum, target pressure, and progression.

If another system sees the word `rail`, it must interpret it as:

> a controlled combat trajectory, not physical rails.

## What the Game Is

- First-person shooter
- Arcadey and fast
- Built around wave pressure and encounter pacing
- Uses rail logic to guide movement and combat beats
- Chaos systems make the battlefield feel unstable, reactive, and escalating

## What the Game Is Not

- Not a literal rail shooter with automatic movement on tracks
- Not a slow tactical sim
- Not an open sandbox
- Not a menu-heavy or inventory-heavy game

## Design Pillars

1. **Readable combat**
   - The player must understand threat, ammo, health, and direction instantly.

2. **Guided flow**
   - The level should lead the player through a clear sequence of combat spaces.
   - Movement can be free, but encounter design is directed.

3. **Pressure first**
   - Enemies, chaos, and visual feedback should create urgency.
   - The game should feel like it is pushing back.

4. **Minimal HUD**
   - Only the information needed to survive the fight should stay on screen.

5. **Escalation**
   - Waves, chaos state, lighting, shake, and glitch effects should rise together.

## Rail Mechanics Meaning

`Rail mechanics` in this project means:

- encounter pacing is scripted or semi-scripted;
- the player is moved through combat zones in a deliberate order;
- camera framing, spawn logic, and level layout all support that order;
- the player has freedom inside each zone, but the game still feels directed.

This is a **design constraint**, not just a visual label.

## Interface Language

The UI should communicate:

- current wave or phase;
- current objective;
- active threat level;
- health and ammo;
- chaos pressure;
- weapon state.

The UI must stay aggressive, compact, and readable.

## Role Boundary

For this project:

- **ChatGPT** owns process, structure, architecture, and gameplay definition.
- **Codex** owns code files and implementation tasks.
- **Claude** owns UE5 / Blueprint / engine-side setup.

I am not the implementation coder here.
I am the process owner that keeps the work split correctly and makes sure every system gets a clear target.

## Practical Interpretation for Other Systems

If you are another assistant, use this interpretation:

> Build a shooter where movement is free enough to feel responsive, but level flow, enemy spawns, and UI create a rail-like combat path. The player is not riding literal rails; the game is guiding them through directed firefights.

