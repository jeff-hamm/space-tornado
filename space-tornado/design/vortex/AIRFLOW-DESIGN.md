# Fire Vortex Intake Cap - Airflow Design Document

## Overview

This document describes how airflow works in a **passive, convection-driven fire vortex** system and the critical design requirements for the intake cap/base.

---

## The Core Physics

### 1. Convection as the Driving Force

The fire vortex requires **no fans or moving parts**. It's entirely driven by natural convection:

```
Fire → Heats air → Hot air rises (lower density) → Creates LOW PRESSURE at base → 
Ambient cool air rushes in to fill the void → Cycle continues
```

**Key insight**: The fire itself is the engine. The stronger the fire, the more powerful the convection draw, and the faster the vortex spins.

### 2. Tangential Air Entry (The Critical Feature)

Simply having air rush toward the center won't create a vortex. The geometry must force incoming air to enter **tangentially** (at an angle to the radius, not pointed directly at the center).

```
WRONG (radial entry):          RIGHT (tangential entry):
                               
    ↓   ↓   ↓                      ↘   ↓   ↙
    →   ●   ←                   →   ◎   ← 
    ↑   ↑   ↑                      ↗   ↑   ↖
                               (air spirals around center)
```

### 3. Conservation of Angular Momentum

This is why the vortex spins faster as it rises:

```
L = m × v × r  (angular momentum is conserved)

As air is drawn toward center: r decreases → v must INCREASE
(Like an ice skater spinning faster when they pull in their arms)
```

The hot air rises and contracts toward the center of the tube, causing the spin rate to accelerate dramatically.

---

## How the Base/Cap Creates Tangential Entry

### Ramp-Slot Design (What I believe is in vortex_base.png)

The base contains **angled ramp-slots** that act like curved guide vanes:

1. **Slots**: Openings around the perimeter allow ambient air to enter
2. **Ramps**: Angled surfaces (15-30° from radial) redirect the incoming air
3. **Result**: Air enters the chamber with tangential velocity, creating spin

```
    TOP VIEW (showing one ramp):
    
    ┌─────────┐
    │         │ ← OUTER WALL
    │   ╱     │
    │  ╱      │ ← ANGLED RAMP (deflects air)
    │ ╱       │
    └─────────┘
    ↑ AIR ENTERS SLOT
    
    Air hits ramp → Deflected sideways → Enters chamber spinning
```

### Key Geometry Parameters

| Parameter | Recommended Value | Why |
|-----------|-------------------|-----|
| Ramp angle | 15-30° from radial | Too shallow = weak spin. Too steep = blocks airflow |
| Number of slots | 4-8 | Fewer = stronger individual jets. More = smoother spin |
| Slot width | 0.5-1" | Must allow sufficient airflow for convection |
| Slot height | 1-2" | Taller slots = more air volume |

---

## Caveats for Keeping the Flame Lit

### 1. Air Velocity vs. Flame Stability

**Problem**: If air enters too fast, it can blow out the flame.

**Solutions**:
- Ensure slots are not too narrow (would accelerate air like a nozzle)
- Flame source should be slightly below the slot level so air passes OVER the flame initially
- Use a burner/torch with good flame retention (propane torch vs. open fuel cup)

### 2. Fuel Delivery Location

**Problem**: The torch entry point affects flame stability.

**Options analyzed**:

| Entry Point | Pros | Cons |
|-------------|------|------|
| Through cap (top) | Protected from incoming air, flame shoots down into vortex | Must thread hose through cap, flame may not anchor well |
| Through tube wall (side) | Natural flame direction, easy installation | May be disrupted by vortex air, needs precise angle |
| Through base (bottom) | Flame rises naturally with convection, protected from incoming air by ramps, centered in vortex | Fuel line routing under base |

**Recommendation**: **Bottom entry through base collar center**. The flame rises naturally with convection, is centered in the vortex core, and the angled ramps deflect incoming air AROUND the flame rather than directly at it.

### 3. Minimum Fire Power

**Problem**: Weak flame = weak convection = insufficient air draw = no vortex.

**Rule of thumb**: The flame needs to be strong enough to create noticeable convection. A propane torch at ~15-25K BTU should work for a 4" tube. A small candle will NOT work.

---

## Caveats for Maintaining the Vortex

### 1. Tube Height Ratio

**Problem**: Too short = vortex doesn't develop. Too tall = air cools and loses momentum.

**Guideline**: Tube height should be 4-8× the diameter.
- For 4" tube: 16-32" height is ideal
- Our design: ~24" tube height ✓

### 2. Open Top vs. Capped Top

**Problem**: A completely closed cap stops the convection cycle (hot air can't escape).

**Requirements**:
- Top must be open OR have exhaust holes
- If using a cap, leave the center mostly open
- Our "cap" is really just a ring/collar, not a full closure

### 3. All Air Must Enter Tangentially

**Problem**: If there are any radial air paths (straight shots to center), they disrupt the spin.

**Check list**:
- [ ] No gaps in outer wall allowing radial entry
- [ ] Ramps fully redirect air to tangential path  
- [ ] Cap doesn't create downward radial jets

### 4. Symmetric Slot Placement

**Problem**: Uneven slot placement creates wobble/instability in the vortex.

**Requirement**: Slots must be evenly spaced around circumference (e.g., 6 slots at 60° intervals).

---

## Proposed Design: Intake Cap with Integrated Ramps

Based on this analysis, the cap should:

1. **Be a BASE collar, not a top cap** - sits at the bottom of the tube
2. **Have 6-8 angled ramp-slots** around the perimeter
3. **Ramps at 20-25° angle** from radial
4. **Slot dimensions**: ~0.6" wide × 1.8" tall
5. **Inner diameter**: 4" (tube fits inside the collar)
6. **Outer diameter**: ~6" (to accommodate ramp length)
7. **Height**: ~2.5"

**Torch entry**: 0.5" hole through the **bottom center** of the base collar floor. Flame rises naturally with convection and is centered in the vortex.

---

## Summary

| Requirement | Status | Notes |
|-------------|--------|-------|
| Passive (no fans) | ✓ | Convection-driven only |
| Air enters at base only | ✓ | Single intake point via ramp-slots |
| Tangential air entry | ✓ | Angled ramps redirect airflow |
| Flame stability | ⚠️ | Needs proper torch placement above slots |
| Vortex maintenance | ⚠️ | Depends on flame power and slot geometry |

---

*Document created: Dec 23, 2025*
*Version: 3*
