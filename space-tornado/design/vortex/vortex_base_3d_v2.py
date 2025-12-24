"""
Vortex Base Collar - 3D Model Generator V2
Creates a passive convection-driven fire vortex intake base with angled ramp-slots.

Key features:
- 6 angled ramp-slots for tangential air entry  
- Torch entry from the BOTTOM center
- 4" inner diameter (tube sits inside)
- ~6" outer diameter
- ~2.5" height

Uses constructive solid geometry with proper curved ramps.
"""

import numpy as np
import trimesh
from pathlib import Path
from scipy.spatial import ConvexHull

# All measurements in inches (will convert to mm for STL)
INCH_TO_MM = 25.4

# Design parameters
INNER_DIAMETER = 4.0  # inches - tube fits inside
OUTER_DIAMETER = 6.0  # inches  
HEIGHT = 2.5  # inches
FLOOR_THICKNESS = 0.3  # inches

# Ramp-slot parameters
NUM_SLOTS = 6
SLOT_ANGULAR_WIDTH = 25  # degrees - how wide each slot opening is
RAMP_ANGLE = 22  # degrees from radial - the deflection angle

# Torch entry (from bottom)
TORCH_HOLE_DIAMETER = 0.5  # inches (for 1/4" NPT fitting)

# Resolution
SEGMENTS = 72  # circular resolution


def create_hollow_cylinder(outer_r, inner_r, height, floor_thickness, segments=72):
    """Create a hollow cylinder with a floor"""
    # Create vertices for outer and inner walls, top and bottom
    angles = np.linspace(0, 2*np.pi, segments, endpoint=False)
    
    vertices = []
    faces = []
    
    # Bottom outer ring (on floor)
    for a in angles:
        vertices.append([outer_r * np.cos(a), outer_r * np.sin(a), 0])
    # Top outer ring
    for a in angles:
        vertices.append([outer_r * np.cos(a), outer_r * np.sin(a), height])
    # Bottom inner ring (at floor level)
    for a in angles:
        vertices.append([inner_r * np.cos(a), inner_r * np.sin(a), floor_thickness])
    # Top inner ring
    for a in angles:
        vertices.append([inner_r * np.cos(a), inner_r * np.sin(a), height])
    
    n = segments
    
    # Outer wall faces
    for i in range(n):
        i_next = (i + 1) % n
        # Outer wall quad (two triangles)
        faces.append([i, i_next, i + n])
        faces.append([i_next, i_next + n, i + n])
    
    # Inner wall faces (reversed winding)
    for i in range(n):
        i_next = (i + 1) % n
        faces.append([2*n + i, 2*n + i + n, 2*n + i_next])
        faces.append([2*n + i_next, 2*n + i + n, 2*n + i_next + n])
    
    # Top ring (between outer and inner top)
    for i in range(n):
        i_next = (i + 1) % n
        faces.append([n + i, 3*n + i, n + i_next])
        faces.append([n + i_next, 3*n + i, 3*n + i_next])
    
    # Bottom ring (floor - between outer bottom and inner bottom at floor level)
    for i in range(n):
        i_next = (i + 1) % n
        faces.append([i, i_next, 2*n + i])
        faces.append([i_next, 2*n + i_next, 2*n + i])
    
    # Floor surface (inner circle at floor level)
    # Add center point for floor
    center_idx = len(vertices)
    vertices.append([0, 0, floor_thickness])
    for i in range(n):
        i_next = (i + 1) % n
        faces.append([center_idx, 2*n + i_next, 2*n + i])
    
    # Bottom surface (outer circle at z=0)
    bottom_center_idx = len(vertices)
    vertices.append([0, 0, 0])
    for i in range(n):
        i_next = (i + 1) % n
        faces.append([bottom_center_idx, i, i_next])
    
    mesh = trimesh.Trimesh(vertices=np.array(vertices), faces=np.array(faces))
    mesh.fix_normals()
    return mesh


def create_slot_cutter(slot_index, outer_r, inner_r, height, floor_t, segments=72):
    """
    Create a wedge shape that cuts a slot into the cylinder.
    The slot is angled to create tangential airflow.
    """
    slot_angular_width_rad = np.radians(SLOT_ANGULAR_WIDTH)
    ramp_angle_rad = np.radians(RAMP_ANGLE)
    base_angle = (2 * np.pi / NUM_SLOTS) * slot_index
    
    slot_height = height - floor_t - 0.1 * INCH_TO_MM  # Leave a bit of floor
    slot_bottom = floor_t
    slot_top = floor_t + slot_height
    
    # The slot is a curved wedge from outer to inner radius
    # But it's rotated by the ramp angle to create tangential flow
    
    # Create points for the slot
    slot_angles = np.linspace(
        base_angle - slot_angular_width_rad/2 + ramp_angle_rad,
        base_angle + slot_angular_width_rad/2 + ramp_angle_rad,
        12
    )
    
    vertices = []
    
    # Outer edge points (bottom)
    extend_r = outer_r * 1.1  # Extend past outer wall for clean cut
    for a in slot_angles:
        vertices.append([extend_r * np.cos(a), extend_r * np.sin(a), slot_bottom])
    
    # Inner edge points (bottom) - these angle toward tangent
    for a in slot_angles:
        # Inner points are at inner radius but shifted by ramp angle
        vertices.append([inner_r * np.cos(a), inner_r * np.sin(a), slot_bottom])
    
    # Outer edge points (top)
    for a in slot_angles:
        vertices.append([extend_r * np.cos(a), extend_r * np.sin(a), slot_top])
    
    # Inner edge points (top)
    for a in slot_angles:
        vertices.append([inner_r * np.cos(a), inner_r * np.sin(a), slot_top])
    
    vertices = np.array(vertices)
    
    # Create faces using convex hull for simplicity
    try:
        hull = ConvexHull(vertices)
        mesh = trimesh.Trimesh(vertices=vertices, faces=hull.simplices)
        mesh.fix_normals()
        return mesh
    except:
        # Fallback: create box
        box = trimesh.creation.box(extents=[
            (outer_r - inner_r) * 1.5,
            slot_angular_width_rad * outer_r * 1.2,
            slot_height
        ])
        mid_r = (outer_r + inner_r) / 2
        box.apply_translation([0, 0, (slot_bottom + slot_top) / 2])
        
        # Rotate by ramp angle
        rot = trimesh.transformations.rotation_matrix(ramp_angle_rad, [0, 0, 1])
        box.apply_transform(rot)
        
        # Move to radius
        box.apply_translation([mid_r, 0, 0])
        
        # Rotate to slot position
        rot2 = trimesh.transformations.rotation_matrix(base_angle, [0, 0, 1])
        box.apply_transform(rot2)
        
        return box


def create_ramp(slot_index, outer_r, inner_r, height, floor_t):
    """
    Create the angled ramp surface that deflects air tangentially.
    This is a wedge-shaped piece that sits on one side of each slot.
    """
    ramp_angle_rad = np.radians(RAMP_ANGLE)
    slot_angular_width_rad = np.radians(SLOT_ANGULAR_WIDTH)
    base_angle = (2 * np.pi / NUM_SLOTS) * slot_index
    
    ramp_height = (height - floor_t) * 0.8
    ramp_thickness = 0.15 * INCH_TO_MM
    ramp_length = (outer_r - inner_r) * 0.7
    
    # Create a thin angled plate
    ramp = trimesh.creation.box(extents=[
        ramp_length,
        ramp_thickness,
        ramp_height
    ])
    
    # Position at the edge of the slot
    ramp_r = inner_r + ramp_length / 2 + 2
    ramp_z = floor_t + ramp_height / 2
    
    # Angle it
    ramp_offset_angle = base_angle + slot_angular_width_rad/2 * 0.3
    rot = trimesh.transformations.rotation_matrix(
        ramp_offset_angle + ramp_angle_rad, [0, 0, 1]
    )
    ramp.apply_transform(rot)
    
    # Position
    x = ramp_r * np.cos(ramp_offset_angle)
    y = ramp_r * np.sin(ramp_offset_angle)
    ramp.apply_translation([x, y, ramp_z])
    
    return ramp


def generate_vortex_base():
    """Generate the complete vortex base model"""
    print("Creating vortex base collar V2...")
    
    outer_r = (OUTER_DIAMETER / 2) * INCH_TO_MM
    inner_r = (INNER_DIAMETER / 2) * INCH_TO_MM
    height = HEIGHT * INCH_TO_MM
    floor_t = FLOOR_THICKNESS * INCH_TO_MM
    torch_r = (TORCH_HOLE_DIAMETER / 2) * INCH_TO_MM
    
    # Step 1: Create main hollow cylinder body
    print("  - Creating main cylinder body...")
    base = create_hollow_cylinder(outer_r, inner_r, height, floor_t, SEGMENTS)
    
    # Step 2: Cut slots
    print("  - Cutting ramp-slots...")
    for i in range(NUM_SLOTS):
        slot = create_slot_cutter(i, outer_r, inner_r, height, floor_t)
        try:
            base = base.difference(slot)
        except Exception as e:
            print(f"    Warning: Slot {i} boolean failed: {e}")
    
    # Step 3: Add ramp surfaces
    print("  - Adding ramp surfaces...")
    for i in range(NUM_SLOTS):
        try:
            ramp = create_ramp(i, outer_r, inner_r, height, floor_t)
            base = base.union(ramp)
        except Exception as e:
            print(f"    Warning: Ramp {i} union failed: {e}")
    
    # Step 4: Cut torch hole in bottom center
    print("  - Adding torch hole (bottom center)...")
    torch_hole = trimesh.creation.cylinder(
        radius=torch_r,
        height=floor_t * 4,
        sections=32
    )
    torch_hole.apply_translation([0, 0, floor_t / 2])
    try:
        base = base.difference(torch_hole)
    except Exception as e:
        print(f"    Warning: Torch hole boolean failed: {e}")
    
    return base


def main():
    output_dir = Path(__file__).parent / "output"
    output_dir.mkdir(exist_ok=True)
    
    # Generate the base
    vortex_base = generate_vortex_base()
    
    # Fix mesh if needed
    if not vortex_base.is_watertight:
        print("  - Fixing mesh...")
        vortex_base.fill_holes()
        vortex_base.fix_normals()
    
    # Export files
    stl_path = output_dir / "vortex_base_collar_v2.stl"
    vortex_base.export(str(stl_path))
    print(f"\nExported: {stl_path}")
    
    obj_path = output_dir / "vortex_base_collar_v2.obj"
    vortex_base.export(str(obj_path))
    print(f"Exported: {obj_path}")
    
    glb_path = output_dir / "vortex_base_collar_v2.glb"
    vortex_base.export(str(glb_path))
    print(f"Exported: {glb_path}")
    
    # Print stats
    print(f"\nModel Statistics:")
    print(f"  Vertices: {len(vortex_base.vertices)}")
    print(f"  Faces: {len(vortex_base.faces)}")
    print(f"  Watertight: {vortex_base.is_watertight}")
    
    bounds = vortex_base.bounds
    dims = bounds[1] - bounds[0]
    print(f"\nDimensions:")
    print(f"  Diameter: {dims[0]:.1f} mm ({dims[0]/INCH_TO_MM:.2f} in)")
    print(f"  Height: {dims[2]:.1f} mm ({dims[2]/INCH_TO_MM:.2f} in)")
    
    print(f"\nDesign Parameters:")
    print(f"  Inner Diameter: {INNER_DIAMETER}\" (tube fits inside)")
    print(f"  Outer Diameter: {OUTER_DIAMETER}\"")
    print(f"  Height: {HEIGHT}\"")
    print(f"  Slots: {NUM_SLOTS} @ {360//NUM_SLOTS}° spacing")
    print(f"  Ramp Angle: {RAMP_ANGLE}° from radial")
    print(f"  Torch Hole: {TORCH_HOLE_DIAMETER}\" diameter (bottom center)")


if __name__ == "__main__":
    main()
