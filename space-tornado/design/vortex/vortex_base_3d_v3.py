"""
Vortex Base Collar - 3D Model Generator V3
Creates a passive convection-driven fire vortex intake base with angled ramp-slots.

Simplified approach using trimesh primitives and boolean operations.
"""

import numpy as np
import trimesh
from pathlib import Path

# All measurements in inches, converted to mm for STL
INCH_TO_MM = 25.4

# Design parameters
INNER_DIAMETER = 4.0    # tube fits inside
OUTER_DIAMETER = 6.0    
HEIGHT = 2.5            
FLOOR_THICKNESS = 0.3   

# Ramp-slot parameters  
NUM_SLOTS = 6
SLOT_WIDTH = 0.6        # inches
SLOT_HEIGHT = 1.8       # inches - most of the wall height
RAMP_ANGLE = 22         # degrees from radial

# Torch entry (bottom center)
TORCH_HOLE_DIAMETER = 0.5


def mm(inches):
    """Convert inches to mm"""
    return inches * INCH_TO_MM


def generate_vortex_base():
    """Generate the complete vortex base model using simple CSG"""
    
    print("Creating vortex base collar V3...")
    
    # Dimensions in mm
    outer_r = mm(OUTER_DIAMETER / 2)
    inner_r = mm(INNER_DIAMETER / 2)
    height = mm(HEIGHT)
    floor_t = mm(FLOOR_THICKNESS)
    slot_w = mm(SLOT_WIDTH)
    slot_h = mm(SLOT_HEIGHT)
    torch_r = mm(TORCH_HOLE_DIAMETER / 2)
    ramp_rad = np.radians(RAMP_ANGLE)
    
    # Step 1: Create outer cylinder
    print("  - Creating outer cylinder...")
    outer_cyl = trimesh.creation.cylinder(radius=outer_r, height=height, sections=72)
    outer_cyl.apply_translation([0, 0, height/2])
    
    # Step 2: Create inner cylinder (hollow core) - leave floor
    print("  - Hollowing interior...")
    inner_height = height - floor_t + 0.1  # extend past top for clean cut
    inner_cyl = trimesh.creation.cylinder(radius=inner_r, height=inner_height, sections=72)
    inner_cyl.apply_translation([0, 0, floor_t + inner_height/2])
    
    # Boolean subtract
    base = outer_cyl.difference(inner_cyl)
    
    # Step 3: Cut slots and add ramps
    print("  - Creating slots and ramps...")
    
    slot_bottom_z = floor_t + 0.5  # slight offset from floor
    slot_length = outer_r - inner_r + 10  # extend past both walls
    
    for i in range(NUM_SLOTS):
        angle = (2 * np.pi / NUM_SLOTS) * i
        
        # Create slot as a box
        slot_box = trimesh.creation.box(extents=[slot_length, slot_w, slot_h])
        
        # Position at mid-radius height
        slot_center_r = (outer_r + inner_r) / 2
        slot_center_z = slot_bottom_z + slot_h / 2
        
        # Apply ramp angle rotation (rotate around Z before translating)
        rot_matrix = trimesh.transformations.rotation_matrix(ramp_rad, [0, 0, 1])
        slot_box.apply_transform(rot_matrix)
        
        # Translate to mid-radius
        slot_box.apply_translation([slot_center_r, 0, slot_center_z])
        
        # Rotate to slot position around origin
        pos_matrix = trimesh.transformations.rotation_matrix(angle, [0, 0, 1])
        slot_box.apply_transform(pos_matrix)
        
        # Cut slot from base
        base = base.difference(slot_box)
        
        # Create ramp (angled deflector surface)
        ramp_width = slot_w * 0.35
        ramp_length = (outer_r - inner_r) * 0.65
        ramp_height = slot_h * 0.85
        
        ramp_box = trimesh.creation.box(extents=[ramp_length, ramp_width, ramp_height])
        
        # Position ramp at inner edge of slot, offset to one side
        ramp_r = inner_r + ramp_length / 2 + 3
        ramp_offset = slot_w * 0.4
        
        # Apply same ramp angle
        ramp_box.apply_transform(rot_matrix)
        
        # Translate - offset in Y direction before rotation
        ramp_box.apply_translation([ramp_r, ramp_offset, slot_center_z])
        
        # Rotate to position
        ramp_box.apply_transform(pos_matrix)
        
        # Add ramp to base
        base = base.union(ramp_box)
    
    # Step 4: Cut torch hole through floor (bottom center)
    print("  - Adding torch hole...")
    torch_cyl = trimesh.creation.cylinder(radius=torch_r, height=floor_t * 3, sections=32)
    torch_cyl.apply_translation([0, 0, floor_t / 2])
    base = base.difference(torch_cyl)
    
    return base


def main():
    output_dir = Path(__file__).parent / "output"
    output_dir.mkdir(exist_ok=True)
    
    # Generate
    vortex_base = generate_vortex_base()
    
    # Ensure mesh is valid
    print("  - Validating mesh...")
    if not vortex_base.is_watertight:
        print("    Mesh not watertight, attempting repair...")
        vortex_base.fill_holes()
    
    # Export
    stl_path = output_dir / "vortex_base_collar_v3.stl"
    vortex_base.export(str(stl_path))
    print(f"\nExported: {stl_path}")
    
    obj_path = output_dir / "vortex_base_collar_v3.obj"
    vortex_base.export(str(obj_path))
    print(f"Exported: {obj_path}")
    
    glb_path = output_dir / "vortex_base_collar_v3.glb"
    vortex_base.export(str(glb_path))
    print(f"Exported: {glb_path}")
    
    # Stats
    print(f"\nModel Statistics:")
    print(f"  Vertices: {len(vortex_base.vertices)}")
    print(f"  Faces: {len(vortex_base.faces)}")
    print(f"  Watertight: {vortex_base.is_watertight}")
    
    bounds = vortex_base.bounds
    dims = bounds[1] - bounds[0]
    print(f"\nDimensions:")
    print(f"  X: {dims[0]:.1f} mm ({dims[0]/INCH_TO_MM:.2f}\")")
    print(f"  Y: {dims[1]:.1f} mm ({dims[1]/INCH_TO_MM:.2f}\")")  
    print(f"  Z: {dims[2]:.1f} mm ({dims[2]/INCH_TO_MM:.2f}\")")
    
    print(f"\n=== DESIGN SUMMARY ===")
    print(f"  Inner Diameter: {INNER_DIAMETER}\" (polycarbonate tube fits inside)")
    print(f"  Outer Diameter: {OUTER_DIAMETER}\"")
    print(f"  Height: {HEIGHT}\"")
    print(f"  Slots: {NUM_SLOTS} evenly spaced")
    print(f"  Ramp Angle: {RAMP_ANGLE}° (creates tangential airflow)")
    print(f"  Torch Entry: {TORCH_HOLE_DIAMETER}\" hole in bottom center")
    print(f"\nView the .glb file in VS Code or any 3D viewer!")


if __name__ == "__main__":
    main()
