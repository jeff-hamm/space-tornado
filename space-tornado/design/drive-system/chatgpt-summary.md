# ChatGPT Engineering Summary

> Key takeaways from ChatGPT consultation on drive system geometry

---

## 🎯 CRITICAL NUMBERS (Memorize These)

| Measurement | Value |
|-------------|-------|
| **Axle center height above floor** | **4.75"** |
| **Axle center above underside of deck** | **1.00"** |
| **Axle center above top of deck** | **0.25"** |
| **Drive wheel protrusion below casters** | **0.25"** (for traction) |

---

## 📐 Confirmed Dimensions

| Part | Dimension |
|------|-----------|
| Overall width | 28" |
| Overall length | 53" (4'5") |
| Deck thickness | 0.75" |
| Drive wheel diameter | 10" |
| Caster overall height | 3.75" |
| Center hole | ≥12" (14" recommended) |

---

## 🔧 HEIGHT GEOMETRY EXPLAINED

```
                    ┌─────────────────────┐
                    │   TOP OF DECK       │ ← 4.50" from floor
    Axle center ────┼───────●─────────────┤ ← 4.75" from floor (0.25" above deck top)
                    │   DECK (0.75")      │
                    └─────────────────────┘ ← 3.75" from floor (underside)
                           │
                           │  Drive wheel
                           │  protrudes
                           ▼  0.25" below
    ═══════════════════════════════════════  FLOOR
         ▲                 ▲
         │                 │
      Caster            Drive wheel
      contact           contact
```

---

## 🛠️ BRACKET RECOMMENDATIONS

### Option A: Top-Mount (RECOMMENDED - Easiest)
- Motor/gearbox sits **on top** of deck
- Wheel drops through center hole
- Easiest to adjust and shim

### Option B: Under-Mount (Cleaner look)
- Motor mounts **under** deck
- U-bracket wraps around gearbox
- Harder to adjust

### Bracket Materials (Good → Bad)
✅ 3/16" steel plate  
✅ 1/4" aluminum plate  
✅ Laminated 3/4" plywood (doubled to 1.5")  
❌ Thin angle brackets  
❌ Single layer plywood  
❌ Wood screws alone  

---

## ⚠️ CRITICAL BUILD TIPS

### 1. Use Through-Bolts, Not Screws
- Through-bolts with fender washers
- Steel backing plate on opposite side
- Lock nuts or nylocs

### 2. Reinforce the Center Hole
- Add 3/4" plywood doubler around opening
- Center hole is a stress riser!

### 3. Build in Adjustment
- **Vertical slots** (at least 3/8" tall)
- Large washers or backing plate
- Allows ±3/16" fine-tuning for traction

### 4. Caster Placement for Stability
- Brakes on **diagonal corners** (front-left + rear-right)
- Add plywood doubler blocks under each caster
- Through-bolts, not wood screws

---

## ✅ SHOP TEST PROCEDURE

1. Put platform on installed casters on flat floor
2. Adjust drive mount until axle center = **4.75" above floor**
3. Verify drive wheel sits **~0.25" lower** than casters when loaded
4. Test with actual rider weight

---

## ❓ STILL NEED TO DETERMINE

- [ ] Does drive unit have **side mounting ears** or **flat bottom flange**?
- [ ] Final bracket geometry based on motor mount style

---

## 💡 ChatGPT's Key Insight

> "A round opening with an adapter ring is the best solution. You want flexibility to shim the drive wheel up or down relative to the caster height."

Cut a 14" circular hole now, fine-tune with a bolt-on adapter ring later.
