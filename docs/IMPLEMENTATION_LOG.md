# Implementation Log

Chronological record of what was implemented and **where**, so any agent (ChatGPT / Codex / Claude) or human can pick up the project without re-reading the whole history. Newest entry on top.

See role boundaries in [`design/CLAUDE_NOTES.md`](../design/CLAUDE_NOTES.md) and process rules in [`RULES.md`](RULES.md).

---

## 2026-06-24 — Shooting System + Damage Interface + Rail Movement (core) + level wiring

> Note on roles for this entry: Claude temporarily acted as the **Codex (C++)** agent for the C++ files below (at the user's explicit request), and as the **Claude (UE5)** agent for the Blueprint/level/test work. Normal domain split resumes after this.

### 1. Damage interface (C++)

- **Files:** `Source/LastInput/Public/Interfaces/Damageable.h`
- **What:** `UDamageable` / `IDamageable` interface, `UFUNCTION(BlueprintNativeEvent, BlueprintCallable)`.
- **Contract:** `void ApplyPointDamage(float Damage, const FHitResult& HitInfo)`.
  - Originally named `TakeDamage`; **renamed to `ApplyPointDamage`** to avoid a hard collision with the built-in `AActor::TakeDamage` UFUNCTION (which would break compilation for any AActor implementing the interface). The build confirms there is no collision now.
- Implementable by both C++ and Blueprint actors. No `.cpp` needed (BlueprintNativeEvent provides the default impl).

### 2. Shooting component (C++)

- **Files:** `Source/LastInput/Public/Components/ShootingComponent.h`, `Source/LastInput/Private/Components/ShootingComponent.cpp`
- **Class:** `UShootingComponent : UActorComponent` (BlueprintSpawnable).
- **Public API:** `void Fire()` (BlueprintCallable) — the only entry point. Hitscan, no projectiles, no weapon abstraction.
- **`Fire()` steps:** (1) cooldown check `WorldTime < NextFireTime` → abort; (2) get `UCameraComponent` from owner via `FindComponentByClass`; (3) camera forward vector; (4) `LineTraceSingleByChannel`, `Start = camera location`, `End = Start + Forward * TraceDistance`, channel `ECC_Visibility`, owner ignored; (5) if hit and `HitActor->Implements<UDamageable>()` → `IDamageable::Execute_ApplyPointDamage(HitActor, Damage, HitResult)`; (6) `NextFireTime = WorldTime + FireRate`.
- **Variables:** `Damage` (20), `TraceDistance` (10000 cm), `FireRate` (0.2 s), `NextFireTime` (private state), `bCanFire` (master enable; when false `Fire()` no-ops). Tick disabled (event-driven).
- **Input:** intentionally NOT handled here. `Fire()` is exposed for Blueprint to call; input binding is set up in the editor separately.
- **Not done (out of scope, belongs to architecture/gameplay):** no enemies, no weapon/ammo systems. `IDamageable` still has no implementers (nothing to damage yet).

#### 2a. Shooting — UE integration (engine-side, added later in the session)

- `UShootingComponent` attached to **`BP_RailPlayer`** as component variable **`Shooting`** (instance defaults: `Damage 20`, `TraceDistance 10000`, `FireRate 0.2`, `bCanFire true`).
- **Input binding (LMB):** the project uses **Enhanced Input** (`DefaultInputComponentClass = EnhancedInputComponent`). Rather than author IA/IMC assets, `Fire()` is driven from `BP_RailPlayer` **EventGraph**:
  `EventTick → if GetPlayerController(0).WasInputKeyJustPressed("LeftMouseButton") → Shooting.Fire()`.
  `WasInputKeyJustPressed` is edge-triggered and works independently of Enhanced Input; the FireRate cooldown lives in the component. (If the team later prefers proper Enhanced Input, swap this for an `IA_Fire` action — the `Shooting.Fire()` call stays the same.)
- **Verified:** `BP_RailPlayer` compiles clean, PIE smoke test shows MapCheck 0/0 and no Accessed-None / Blueprint runtime errors; rail movement still works.

#### 2b. End-to-end shooting verification (with a temporary test target)

- **Temp test actor (C++):** `ATestTarget` (`Source/LastInput/Public/TestTarget.h`, `.../Private/TestTarget.cpp`) — `AActor` + `IDamageable`, a `UStaticMeshComponent` root, and `ApplyPointDamage_Implementation` that increments `HitCount` / `TotalDamage` and logs. **Temporary** — remove when a real damageable (enemy) exists.
- **Placement:** one instance `TestTarget_0` in `Maps/RailTest` at `(4200, 0, 150)`, scale `(1, 12, 6)`, mesh `/Engine/BasicShapes/Cube.Cube` (a wide wall past the spline end, in the player's forward line of fire). Cube blocks `ECC_Visibility` by default.
- **Test method:** temporarily changed `BP_RailPlayer` EventTick to call `Shooting.Fire()` unconditionally (component cooldown still gates it), ran PIE, then reverted to the LMB-gated version.
- **Result ✅:** `HitCount` climbed 25 → 124 → 223 → 322 with `TotalDamage = HitCount × 20` exactly (`Damage = 20`), at ~5 hits/s (matches `FireRate = 0.2`). Confirms the full path: camera-forward line trace → `Implements<UDamageable>` → `IDamageable::Execute_ApplyPointDamage`. The player graph is back on **LMB → Fire**.

### 3. Rail movement (C++) — rewritten to the new core spec

- **Files:** `Source/LastInput/Public/Components/RailMovementComponent.h`, `Source/LastInput/Private/Components/RailMovementComponent.cpp`
- **Class:** `URailMovementComponent : UActorComponent`.
- **State:** `RailSpline` (TObjectPtr), `CurrentDistanceAlongSpline`, `Speed`, `MinSpeed`, `MaxSpeed`, `bMovementEnabled`, `SnapThreshold`, `MaxStepDeltaTime` (added by the §7 fix).
- **Functions:**
  - `TickComponent` — if `!bMovementEnabled` do nothing; clamp `Speed` to [MinSpeed, MaxSpeed]; `CurrentDistanceAlongSpline += Speed * DeltaTime`; sample `GetLocationAtDistanceAlongSpline` / `GetRotationAtDistanceAlongSpline` (World); apply via `SetActorLocation` + `SetActorRotation`.
  - `SnapToSpline()` — if owner deviates from the spline beyond `SnapThreshold`, snap to `FindLocationClosestToWorldLocation`. Public/BlueprintCallable; not auto-called from Tick.
  - `SetSpeed(float)` — clamps to [MinSpeed, MaxSpeed].
  - `SetSpline(USplineComponent*)` — assigns `RailSpline`.
- **⚠️ API BREAK vs the previous version:** the old methods `SetSplineComponent` / `SetCurrentDistanceOnSpline` / getters and the `BeginPlay` auto-find were **removed**. Anything that called `SetSplineComponent` had to be updated to `SetSpline` (see §5).
- No physics, no CharacterMovement, no input handling, no gameplay logic.

### 4. Build

- Rebuilt the module from the command line (editor was closed; structural C++ change is not safe for Live Coding):
  ```
  E:\UnrealEngine\UE_5.8\Engine\Build\BatchFiles\Build.bat LastInputEditor Win64 Development -Project="E:\last-input-game\LastInput.uproject" -waitmutex
  ```
- **Result: Succeeded.** All three C++ units compile and link cleanly (`UnrealEditor-LastInput.dll`).

### 5. Blueprint / level wiring (UE5)

- **`Content/Blueprints/BP_RailPlayer`** — `EventBeginPlay` rewritten:
  `RailMovement.SetSpline( GetComponentByClass(SplinePathActor, SplineComponent) )`
  (replaces the now-removed `SetSplineComponent` node). Compiled + saved.
- `URailMovementComponent` is attached to `BP_RailPlayer` as component variable **`RailMovement`** (already present from MVP-01).
- **`Content/Maps/RailTest`** — verified placed-instance setup:
  - `BP_RailPlayer_C_0.SplinePathActor` → `BP_SplinePath_C_0` ✅
  - `BP_RailPlayer_C_0.AutoPossessPlayer` → `Player0` ✅
  - `WorldSettings.DefaultGameMode` → `BP_GameMode_C` ✅

### 6. Test (PIE in RailTest)

- Player **follows the spline geometry correctly**: position tracks the path through the bend (y rises to ~587 near the mid control point at 600, then back to 0), and **rotation follows the spline tangent** (yaw 21° → −11° → −26.57°).
- Steady-state speed verified: `CurrentDistanceAlongSpline` accumulates at **exactly the set `Speed`** (measured 80.3 cm/s for `Speed = 80`). Movement math is correct.

### 7. ✅ FIXED — first-frame DeltaTime spike

- **Symptom (was):** on PIE/level start the player was instantly thrown to the **end** of the spline.
- **Root cause (confirmed):** the first frame after level load + shader compilation produces a huge `DeltaTime`. `CurrentDistanceAlongSpline += Speed * DeltaTime` advanced by ~12,000+ cm in that single tick (spline length is only ~3,800 cm), overshooting the end. Steady-state movement was always correct (measured exactly `Speed` cm/s); only the first tick was the problem.
- **Fix applied (C++, `RailMovementComponent`):** added `UPROPERTY float MaxStepDeltaTime = 0.1f` and clamp the step in `TickComponent`:
  `const float StepDeltaTime = FMath::Min(DeltaTime, MaxStepDeltaTime); CurrentDistanceAlongSpline += Speed * StepDeltaTime;`
  A load/compile hitch can now advance the owner by at most `Speed * 0.1s` in one tick.
- **Status:** compiled (build Succeeded) and **PIE-verified**. The player now starts at the beginning of the spline; `CurrentDistanceAlongSpline` grows linearly (e.g. 400 → 1184 cm at `Speed = 80`) instead of spiking to ~14,600 cm on frame 1.

### 8. Shooting visual feedback layer (C++) — separate from ShootingComponent

- **Files:** `Source/LastInput/Public/Components/ShootingVisualsComponent.h`, `.../Private/Components/ShootingVisualsComponent.cpp`
- **Class:** `UShootingVisualsComponent : UActorComponent`. Pure feedback — **does not deal damage and does not touch `UShootingComponent`** (rule: visual layer only).
- **API:**
  - `bool AimTrace(FHitResult& OutHit, FVector& OutAimPoint)` — short-range (`AimRange` 5000) camera-forward trace, NOT damage; for crosshair feedback. Exposed for a future HUD (no HUD built).
  - `void PlayShotVisuals()` — A) muzzle flash at camera origin, B) beam from camera to hit/end, C) impact effect at hit point.
- **FX implementation:** `DrawDebug` primitives (line + spheres), `VisualLifetime` 0.05 s — zero content dependencies, visible in PIE. Swap for Niagara/light later without changing the call sites.
- **UE wiring:** `ShootingVisuals` component added to `BP_RailPlayer`; EventTick LMB branch now runs **`Shooting.Fire()` → (then) `ShootingVisuals.PlayShotVisuals()`** (verified by node-connection inspection: `Branch.True → Fire → PlayShotVisuals`, else empty).

### 9. Enemy base (C++)

- **Files:** `Source/LastInput/Public/EnemyBase.h`, `.../Private/EnemyBase.cpp`
- **Class:** `AEnemyBase : AActor, IDamageable`. Minimal — **no AI, no movement, no behavior**.
- **Behavior:** `UStaticMeshComponent` root (so the trace can hit it), `float HP = 100`. `ApplyPointDamage_Implementation` → `HP -= Damage`, logs, `Destroy()` when `HP <= 0`.
- **Verified (PIE, auto-fire):** placed `EnemyBase_0` in `Maps/RailTest` (cube mesh, in the line of fire); log shows `HP` 300→280→…→20→**0** at exactly −20/hit, then the actor is destroyed. Player graph restored to LMB afterward; enemy HP reset to default 100.
- This replaces the throwaway `ATestTarget` (§2b), which was removed from `RailTest`. The `ATestTarget` class files still exist and can be deleted.

---

## Project file map (current systems)

| System | Header | Source | Engine-side asset |
| --- | --- | --- | --- |
| Rail movement | `Source/LastInput/Public/Components/RailMovementComponent.h` | `.../Private/Components/RailMovementComponent.cpp` | `BP_RailPlayer` (`RailMovement` component), `BP_SplinePath`, map `Maps/RailTest` |
| Shooting | `Source/LastInput/Public/Components/ShootingComponent.h` | `.../Private/Components/ShootingComponent.cpp` | `BP_RailPlayer` (`Shooting` component, `Fire()` bound to LMB in EventTick) |
| Shooting visuals | `Source/LastInput/Public/Components/ShootingVisualsComponent.h` | `.../Private/Components/ShootingVisualsComponent.cpp` | `BP_RailPlayer` (`ShootingVisuals` component, `PlayShotVisuals()` chained after `Fire()` on LMB) |
| Enemy base | `Source/LastInput/Public/EnemyBase.h` | `.../Private/EnemyBase.cpp` | `EnemyBase_0` placed in `Maps/RailTest` (cube mesh, HP 100) |
| Damage interface | `Source/LastInput/Public/Interfaces/Damageable.h` | (none) | implemented by `AEnemyBase` |
