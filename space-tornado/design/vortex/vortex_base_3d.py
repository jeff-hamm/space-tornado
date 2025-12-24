"""
Vortex Base Collar - 3D Model Generator
Creates a passive convection-driven fire vortex intake base with angled ramp-slots.

Key features:
- 6 angled ramp-slots for tangential air entry
- Torch entry from the BOTTOM center
- 4" inner diameter (tube sits inside)
- ~6" outer diameter
- ~2.5" height
"""

import numpy as np
import trimesh
from pathlib import Path

# All measurements in inches (will convert to mm for STL)
INCH_TO_MM = 25.4

# Design parameters
INNER_DIAMETER = 4.0  # inches - tube fits inside
OUTER_DIAMETER = 6.0  # inches
HEIGHT = 2.5  # inches
WALL_THICKNESS = 0.25  # inches
FLOOR_THICKNESS = 0.25  # inches

# Ramp-slot parameters
NUM_SLOTS = 6
SLOT_WIDTH = 0.5  # inches
SLOT_HEIGHT = 1.5  # inches
RAMP_ANGLE = 22  # degrees from radial

# Torch entry (from bottom)
TORCH_HOLE_DIAMETER = 0.5  # inches (for 1/4" NPT fitting)

# Resolution
SEGMENTS = 64  # circular resolution


def create_base_cylinder():
    """Create the main body - a hollow cylinder"""
    outer_r = (OUTER_DIAMETER / 2) * INCH_TO_MM
    inner_r = (INNER_DIAMETER / 2) * INCH_TO_MM
    height = HEIGHT * INCH_TO_MM
    floor_t = FLOOR_THICKNESS * INCH_TO_MM
    
    # Outer cylinder
    outer = trimesh.creation.cylinder(
        radius=outer_r,
        height=height,
        sections=SEGMENTS
    )
    # Move so bottom is at z=0
    outer.apply_translation([0, 0, height/2])
    
    # Inner cylinder (hollow out, but leave floor)
    inner = trimesh.creation.cylinder(
        radius=inner_r,
        height=height - floor_t + 1,  # +1 to ensure clean cut through top
        sections=SEGMENTS
    )
    inner.apply_translation([0, 0, (height - floor_t)/2 + floor_t])
    
    # Subtract inner from outer
    base = outer.difference(inner)
    
    return base


def create_single_slot(slot_index):
    """
    Create a single ramp-slot.
    The slot is an angled channel that redirects air tangentially.
    """
    outer_r = (OUTER_DIAMETER / 2) * INCH_TO_MM
    inner_r = (INNER_DIAMETER / 2) * INCH_TO_MM
    height = HEIGHT * INCH_TO_MM
    floor_t = FLOOR_THICKNESS * INCH_TO_MM
    slot_w = SLOT_WIDTH * INCH_TO_MM
    slot_h = SLOT_HEIGHT * INCH_TO_MM
    ramp_angle_rad = np.radians(RAMP_ANGLE)
    
    # Position angle for this slot (evenly distributed)
    base_angle = (2 * np.pi / NUM_SLOTS) * slot_index
    
    # The slot is a wedge-shaped channel
    # It starts at the outer wall and angles toward tangential direction
    
    # Create a box for the slot opening
    slot_length = (outer_r - inner_r) * 1.2  # extend past walls for clean cut
    
    # Create slot as a box
    slot_box = trimesh.creation.box(
        extents=[slot_length, slot_w, slot_h]
    )
    
    # Position: centered at mid-radius, angled
    mid_r = (outer_r + inner_r) / 2
    slot_z = floor_t + slot_h / 2
    
    # Rotate to create the tangential angle (ramp effect)
    # First rotate by ramp angle around Z
    rotation_z = trimesh.transformations.rotation_matrix(
        ramp_angle_rad, [0, 0, 1]
    )
    slot_box.apply_transform(rotation_z)
    
    # Position at the correct radius
    slot_box.apply_translation([mid_r, 0, slot_z])
    
    # Rotate to final position around the circle
    final_rotation = trimesh.transformations.rotation_matrix(
        base_angle, [0, 0, 1]
    )
    slot_box.apply_transform(final_rotation)
    
    return slot_box


def create_all_slots():
    """Create all ramp-slots"""
    slots = []
    for i in range(NUM_SLOTS):
        slot = create_single_slot(i)
        slots.append(slot)
    
    # Combine all slots
    if len(slots) > 1:
        combined = slots[0]
        for s in slots[1:]:
            combined = combined.union(s)
        return combined
    return slots[0]


def create_torch_hole():
    """Create the torch entry hole in the bottom center"""
    torch_r = (TORCH_HOLE_DIAMETER / 2) * INCH_TO_MM
    floor_t = FLOOR_THICKNESS * INCH_TO_MM
    
    # Cylinder that goes through the floor
    hole = trimesh.creation.cylinder(
        radius=torch_r,
        height=floor_t * 3,  # extra length for clean boolean
        sections=32
    )
    hole.apply_translation([0, 0, floor_t / 2])
    
    return hole


def create_ramp_surface(slot_index):
    """
    Create the angled ramp surface inside each slot.
    This is what deflects the air tangentially.
    """
    outer_r = (OUTER_DIAMETER / 2) * INCH_TO_MM
    inner_r = (INNER_DIAMETER / 2) * INCH_TO_MM
    floor_t = FLOOR_THICKNESS * INCH_TO_MM
    slot_h = SLOT_HEIGHT * INCH_TO_MM
    ramp_angle_rad = np.radians(RAMP_ANGLE)
    
    base_angle = (2 * np.pi / NUM_SLOTS) * slot_index
    
    # The ramp is a wedge/prism shape
    ramp_length = (outer_r - inner_r) * 0.8
    ramp_width = SLOT_WIDTH * INCH_TO_MM * 0.4
    ramp_height = slot_h * 0.9
    
    # Create as a box then cut at angle
    ramp = trimesh.creation.box(
        extents=[ramp_length, ramp_width, ramp_height]
    )
    
    # Position at inner edge of slot area
    ramp_r = inner_r + ramp_length / 2 + 5
    ramp_z = floor_t + ramp_height / 2
    
    # Angle the ramp
    rotation_z = trimesh.transformations.rotation_matrix(
        ramp_angle_rad + 0.1, [0, 0, 1]
    )
    ramp.apply_transform(rotation_z)
    
    ramp.apply_translation([ramp_r, 0, ramp_z])
    
    # Rotate to slot position
    final_rotation = trimesh.transformations.rotation_matrix(
        base_angle, [0, 0, 1]
    )
    ramp.apply_transform(final_rotation)
    
    return ramp


def generate_vortex_base():
    """Generate the complete vortex base model"""
    print("Creating vortex base collar...")
    
    # Step 1: Create main body
    print("  - Creating main cylinder body...")
    base = create_base_cylinder()
    
    # Step 2: Cut slots
    print("  - Cutting ramp-slots...")
    slots = create_all_slots()
    base = base.difference(slots)
    
    # Step 3: Cut torch hole
    print("  - Adding torch hole (bottom entry)...")
    torch_hole = create_torch_hole()
    base = base.difference(torch_hole)
    
    # Step 4: Add ramp surfaces inside slots
    print("  - Adding ramp surfaces...")
    for i in range(NUM_SLOTS):
        ramp = create_ramp_surface(i)
        base = base.union(ramp)
    
    return base


def main():
    output_dir = Path(__file__).parent / "output"
    output_dir.mkdir(exist_ok=True)
    
    # Generate the base
    vortex_base = generate_vortex_base()
    
    # Export STL
    stl_path = output_dir / "vortex_base_collar.stl"
    vortex_base.export(str(stl_path))
    print(f"\nExported: {stl_path}")
    
    # Export OBJ for better compatibility
    obj_path = output_dir / "vortex_base_collar.obj"
    vortex_base.export(str(obj_path))
    print(f"Exported: {obj_path}")
    
    # Also export GLB for web viewing
    glb_path = output_dir / "vortex_base_collar.glb"
    vortex_base.export(str(glb_path))
    print(f"Exported: {glb_path}")
    
    # Print stats
    print(f"\nModel Statistics:")
    print(f"  Vertices: {len(vortex_base.vertices)}")
    print(f"  Faces: {len(vortex_base.faces)}")
    print(f"  Watertight: {vortex_base.is_watertight}")
    
    # Dimensions in mm
    bounds = vortex_base.bounds
    dims = bounds[1] - bounds[0]
    print(f"\nDimensions (mm):")
    print(f"  X: {dims[0]:.1f} mm ({dims[0]/INCH_TO_MM:.2f} in)")
    print(f"  Y: {dims[1]:.1f} mm ({dims[1]/INCH_TO_MM:.2f} in)")
    print(f"  Z: {dims[2]:.1f} mm ({dims[2]/INCH_TO_MM:.2f} in)")


if __name__ == "__main__":
    main()
