# MiniRPG

A small single-player RPG slice in Unreal Engine 5, RuneScape-inspired:
point-and-click movement and combat, chop trees and mine rocks for their
own separate skills, fight raiders, level up, and drink potions in a wooded
clearing. Pure C++ — no Blueprints, no imported content.

This is a vertical slice of RPG *mechanics*, not a networked/multiplayer
game — no persistent world, no server, no accounts. That's a fundamentally
different (and much larger) kind of project than what's here.

## Play

Open `MiniRPG.uproject` in Unreal Engine 5.8, let it build, then hit
**Play**.

- **Left-click** the ground to walk there, an enemy to walk up and fight
  it, or a tree/rock to walk up and gather it
- **WASD / Arrow keys** also move, and cancel whatever you clicked on
- **Space** — attack the nearest enemy in range
- **E** — drink a potion from your inventory

Defeat raiders for Combat XP, chop trees for Woodcutting XP and Logs, mine
rocks for Mining XP and Ore (rocks go dark and regrow after a bit — trees
never run out). Defeat every raider in the clearing to win.

## How it's built

- `URPGStatsComponent` — HP, Level, XP, Attack, Defense, shared by both the
  player and enemies so combat code treats them identically.
- `FGatherSkill` — a small reusable Level/XP struct powering the player's
  separate Woodcutting and Mining tracks, independent of combat level.
- `UInventoryComponent` — holds picked-up/gathered items and applies their
  effect (heal or attack boost) when used; raw resources (Logs, Ore) are
  just carried.
- `AMiniRPGPlayerController` — shows and unlocks the mouse cursor for
  point-and-click play alongside normal gameplay input.
- `AMiniRPGPlayerCharacter` — a real animated humanoid on a
  `CharacterMovementComponent`; click-to-move/attack/gather is a small
  target-and-approach state machine driven from `Tick()`, no NavMesh
  required.
- `AMiniRPGEnemyCharacter` — chases the player once in range, attacks on a
  cooldown once adjacent — plain `Tick()` logic, no behavior tree.
- `AMiniRPGProp` — decorative trees and rocks built from basic shapes;
  rocks double as gatherable Ore nodes that deplete and respawn.
- `AMiniRPGPickup` — a rotating, glowing cube with an `OverlapAllDynamic`
  trigger; adds itself to whatever inventory it touches and self-destroys.
- `AMiniRPGGameMode` — spawns the ground, environment dressing, enemies,
  and pickups at `BeginPlay`, and tracks the win condition.

No `.uasset` content is checked in — meshes/animations are referenced
directly from the engine's own `/Engine/BasicShapes/` and
`/Engine/Tutorial/...` content, and the level is the engine's own
`Template_Default` map.
