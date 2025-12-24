"""
Vortex Base Collar - 3D Model Generator V4
Creates a passive convection-driven fire vortex intake base with steep angled ramp-slots.

Key improvements in V4:
- Much steeper ramp angles (45° from radial)
- Ramps extend INSIDE the 4" inner diameter
- Intake floors SLOPE UPWARD on Z axis (spiral ramp effect)
- Torch entry from bottom center
"""

import numpy as np
import trimesh
from pathlib import Path

# All measurements in inches, converted to mm for STL
INCH_TO_MM = 25.4

# Design parameters
INNER_DIAMETER = 4.0    # tube sits on TOP of this, ramps extend inside
OUTER_DIAMETER = 6.0    
HEIGHT = 2.5            
FLOOR_THICKNESS = 0.25  

# Ramp-slot parameters  
NUM_SLOTS = 6
SLOT_ANGULAR_WIDTH = 35  # degrees - wider slots for more airflow
RAMP_ANGLE = 45          # degrees from radial - MUCH steeper for tangential flow
RAMP_SLOPE = 15          # degrees - how much the floor rises as it goes inward

# How far inside the 4" diameter the ramps extend
RAMP_INNER_EXTENSION = 0.75  # inches past the inner wall

# Torch entry (bottom center)
TORCH_HOLE_DIAMETER = 0.5


def mm(inches):
    """Convert inches to mm"""
    return inches * INCH_TO_MM


def create_spiral_ramp_slot(slot_index, outer_r, inner_r, height, floor_t):
    """
    Create a single spiral ramp slot that:
    - Cuts from outer wall through inner wall
    - Is angled steeply for tangential airflow
    - Has a floor that slopes UPWARD as it goes inward
    - Extends past the inner wall into the chamber
    """
    base_angle = (2 * np.pi / NUM_SLOTS) * slot_index
    slot_angular_width_rad = np.radians(SLOT_ANGULAR_WIDTH)
    ramp_angle_rad = np.radians(RAMP_ANGLE)
    slope_rad = np.radians(RAMP_SLOPE)
    
    inner_extension = mm(RAMP_INNER_EXTENSION)
    
    # The slot goes from outer_r + extra, through inner_r, to inner_r - extension
    slot_outer_r = outer_r + 5  # extend past outer wall for clean cut
    slot_inner_r = inner_r - inner_extension  # extend INTO the chamber
    slot_length = slot_outer_r - slot_inner_r
    
    # Slot width at the radial position (arc length)
    slot_width = slot_angular_width_rad * ((outer_r + inner_r) / 2)
    
    # Height varies: taller at outer edge, shorter at inner edge due to slope
    slot_height_outer = mm(1.8)
    slot_height_inner = slot_height_outer * 0.6  # narrower at inner end
    
    # Create vertices for a tapered, sloped box
    # We'll create a custom mesh for this
    
    # Define the 8 corners of a tapered, sloped slot
    # Outer edge (at slot_outer_r)
    outer_z_bottom = floor_t
    outer_z_top = floor_t + slot_height_outer
    
    # Inner edge (at slot_inner_r) - raised due to slope
    z_rise = np.tan(slope_rad) * slot_length
    inner_z_bottom = floor_t + z_rise
    inner_z_top = inner_z_bottom + slot_height_inner
    
    # Half-widths for the slot
    outer_half_w = slot_width / 2
    inner_half_w = slot_width * 0.7 / 2  # narrower at inner end
    
    # Create points in local coordinates (before rotation)
    # Local X = radial direction, Y = tangential, Z = vertical
    vertices = np.array([
        # Outer edge - bottom corners
        [slot_outer_r, -outer_half_w, outer_z_bottom],
        [slot_outer_r, outer_half_w, outer_z_bottom],
        # Outer edge - top corners  
        [slot_outer_r, -outer_half_w, outer_z_top],
        [slot_outer_r, outer_half_w, outer_z_top],
        # Inner edge - bottom corners (raised)
        [slot_inner_r, -inner_half_w, inner_z_bottom],
        [slot_inner_r, inner_half_w, inner_z_bottom],
        # Inner edge - top corners
        [slot_inner_r, -inner_half_w, inner_z_top],
        [slot_inner_r, inner_half_w, inner_z_top],
    ])
    
    # Faces (triangles) - using right-hand rule for outward normals
    faces = np.array([
        # Bottom face (sloped floor)
        [0, 4, 5], [0, 5, 1],
        # Top face
        [2, 3, 7], [2, 7, 6],
        # Outer face
        [0, 1, 3], [0, 3, 2],
        # Inner face
        [4, 6, 7], [4, 7, 5],
        # Side face 1
        [0, 2, 6], [0, 6, 4],
        # Side face 2
        [1, 5, 7], [1, 7, 3],
    ])
    
    slot_mesh = trimesh.Trimesh(vertices=vertices, faces=faces)
    
    # Apply the steep ramp angle (rotate around Z axis)
    rot_ramp = trimesh.transformations.rotation_matrix(ramp_angle_rad, [0, 0, 1])
    slot_mesh.apply_transform(rot_ramp)
    
    # Rotate to slot position
    rot_pos = trimesh.transformations.rotation_matrix(base_angle, [0, 0, 1])
    slot_mesh.apply_transform(rot_pos)
    
    return slot_mesh


def create_ramp_deflector(slot_index, outer_r, inner_r, height, floor_t):
    """
    Create the angled deflector surface on one side of each slot.
    This is what physically redirects the air tangentially.
    Extends into the inner chamber.
    """
    base_angle = (2 * np.pi / NUM_SLOTS) * slot_index
    slot_angular_width_rad = np.radians(SLOT_ANGULAR_WIDTH)
    ramp_angle_rad = np.radians(RAMP_ANGLE)
    slope_rad = np.radians(RAMP_SLOPE)
    
    inner_extension = mm(RAMP_INNER_EXTENSION)
    
    # Ramp dimensions
    ramp_length = (outer_r - inner_r) + inner_extension * 0.8
    ramp_thickness = mm(0.2)
    ramp_height = mm(1.5)
    
    # Z position rises along the ramp
    z_rise = np.tan(slope_rad) * ramp_length * 0.5
    
    # Create ramp as thin box
    ramp = trimesh.creation.box(extents=[ramp_length, ramp_thickness, ramp_height])
    
    # Tilt the ramp to match the floor slope
    tilt = trimesh.transformations.rotation_matrix(slope_rad * 0.3, [0, 1, 0])
    ramp.apply_transform(tilt)
    
    # Position: at inner portion of slot, offset to trailing edge
    ramp_r = inner_r - inner_extension * 0.3 + ramp_length / 2
    ramp_z = floor_t + z_rise + ramp_height / 2
    
    # Offset in Y (tangential direction) - on the trailing edge of slot
    slot_width = slot_angular_width_rad * ((outer_r + inner_r) / 2)
    y_offset = slot_width * 0.35
    
    # Apply ramp angle rotation
    rot_ramp = trimesh.transformations.rotation_matrix(ramp_angle_rad, [0, 0, 1])
    ramp.apply_transform(rot_ramp)
    
    # Translate to position
    ramp.apply_translation([ramp_r, y_offset, ramp_z])
    
    # Rotate to slot position
    rot_pos = trimesh.transformations.rotation_matrix(base_angle, [0, 0, 1])
    ramp.apply_transform(rot_pos)
    
    return ramp


def generate_vortex_base():
    """Generate the complete vortex base model"""
    
    print("Creating vortex base collar V4 (steep angles, inner extension, sloped floor)...")
    
    # Dimensions in mm
    outer_r = mm(OUTER_DIAMETER / 2)
    inner_r = mm(INNER_DIAMETER / 2)
    height = mm(HEIGHT)
    floor_t = mm(FLOOR_THICKNESS)
    torch_r = mm(TORCH_HOLE_DIAMETER / 2)
    inner_ext = mm(RAMP_INNER_EXTENSION)
    
    # Step 1: Create outer cylinder
    print("  - Creating outer cylinder...")
    outer_cyl = trimesh.creation.cylinder(radius=outer_r, height=height, sections=72)
    outer_cyl.apply_translation([0, 0, height/2])
    
    # Step 2: Create inner cylinder - but the RAMPS extend inside, so we hollow
    # out to a smaller radius to leave material for ramps
    print("  - Hollowing interior (leaving room for ramps)...")
    # Inner hollow goes to inner_r minus extension
    hollow_r = inner_r - inner_ext
    inner_height = height - floor_t + 0.1
    inner_cyl = trimesh.creation.cylinder(radius=hollow_r, height=inner_height, sections=72)
    inner_cyl.apply_translation([0, 0, floor_t + inner_height/2])
    
    base = outer_cyl.difference(inner_cyl)
    
    # Step 3: Cut slots (these extend from outer wall through to inner chamber)
    print("  - Cutting angled slots with sloped floors...")
    for i in range(NUM_SLOTS):
        slot = create_spiral_ramp_slot(i, outer_r, inner_r, height, floor_t)
        base = base.difference(slot)
    
    # Step 4: Add deflector ramps
    print("  - Adding deflector ramps...")
    for i in range(NUM_SLOTS):
        ramp = create_ramp_deflector(i, outer_r, inner_r, height, floor_t)
        base = base.union(ramp)
    
    # Step 5: Cut torch hole through floor (bottom center)
    print("  - Adding torch hole (bottom center)...")
    torch_cyl = trimesh.creation.cylinder(radius=torch_r, height=floor_t * 3, sections=32)
    torch_cyl.apply_translation([0, 0, floor_t / 2])
    base = base.difference(torch_cyl)
    
    # Step 6: Create a lip for the tube to sit on
    # The tube (4" OD) sits on top of the collar, resting on the area between
    # the slot openings
    print("  - Adding tube seating lip...")
    lip_height = mm(0.3)
    lip_inner_r = inner_r - mm(0.1)  # slightly smaller than tube OD
    lip_outer_r = inner_r + mm(0.15)
    
    lip_outer = trimesh.creation.cylinder(radius=lip_outer_r, height=lip_height, sections=72)
    lip_inner = trimesh.creation.cylinder(radius=lip_inner_r, height=lip_height + 1, sections=72)
    lip_outer.apply_translation([0, 0, height + lip_height/2])
    lip_inner.apply_translation([0, 0, height + lip_height/2])
    lip = lip_outer.difference(lip_inner)
    
    base = base.union(lip)
    
    return base


def main():
    output_dir = Path(__file__).parent / "output"
    output_dir.mkdir(exist_ok=True)
    
    # Generate
    vortex_base = generate_vortex_base()
    
    # Validate
    print("  - Validating mesh...")
    if not vortex_base.is_watertight:
        print("    Attempting repair...")
        vortex_base.fill_holes()
    
    # Export
    stl_path = output_dir / "vortex_base_collar_v4.stl"
    vortex_base.export(str(stl_path))
    print(f"\nExported: {stl_path}")
    
    obj_path = output_dir / "vortex_base_collar_v4.obj"
    vortex_base.export(str(obj_path))
    print(f"Exported: {obj_path}")
    
    glb_path = output_dir / "vortex_base_collar_v4.glb"
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
    
    print(f"\n=== V4 DESIGN IMPROVEMENTS ===")
    print(f"  Ramp Angle: {RAMP_ANGLE}° from radial (STEEP tangential deflection)")
    print(f"  Ramps extend {RAMP_INNER_EXTENSION}\" INSIDE the 4\" tube diameter")
    print(f"  Slot floors slope upward at {RAMP_SLOPE}° (spiral ramp effect)")
    print(f"  Tube seating lip added on top")
    print(f"  Torch Entry: {TORCH_HOLE_DIAMETER}\" hole in bottom center")


if __name__ == "__main__":
    main()
