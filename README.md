# MiniRPG

A small single-player RPG slice in Unreal Engine 5: roll around a tiny
arena, fight enemies, level up, and drink potions. Pure C++ — no
Blueprints, no custom content — built entirely from the engine's built-in
basic shapes, same approach as [Orb Collector](https://github.com/brinkbuilder/OrbCollector).

This is a vertical slice of RPG *mechanics*, not a networked/multiplayer
game — no persistent world, no server, no accounts. That's a fundamentally
different (and much larger) kind of project than what's here.

## Play

Open `MiniRPG.uproject` in Unreal Engine 5.8, let it build, then hit
**Play**.

- **WASD / Arrow keys** — move
- **Space** — attack the nearest enemy in range
- **E** — use a potion from your inventory

Defeat enemies for XP, level up for more HP/Attack/Defense, and grab the
glowing cubes for potions and a weapon upgrade.

## How it's built

- `URPGStatsComponent` — HP, Level, XP, Attack, Defense, shared by both the
  player and enemies so combat code treats them identically.
- `UInventoryComponent` — holds picked-up items and applies their effect
  (heal or attack boost) when used.
- `AMiniRPGPlayerPawn` — floating-movement sphere with a follow camera;
  attacks via a sphere overlap query against nearby enemies.
- `AMiniRPGEnemyPawn` — chases the player once in range, attacks on a
  cooldown once adjacent — plain `Tick()` logic, no behavior tree.
- `AMiniRPGPickup` — a rotating cube with an `OverlapAllDynamic` trigger;
  adds itself to whatever inventory it touches and self-destroys.
- `AMiniRPGGameMode` — spawns enemies and pickups in rings around the
  start point at `BeginPlay`.

No `.uasset` content is checked in — meshes are referenced directly from
`/Engine/BasicShapes/`, and the level is the engine's own
`Template_Default` map.
