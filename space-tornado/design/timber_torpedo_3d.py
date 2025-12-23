#!/usr/bin/env python3
"""
Timber Torpedo 3D Model Generator & Converter

Generates the Space Tornado board as 3D printable parts.
Supports export to STL, OBJ, PLY, and optionally STEP (if cadquery installed).

Usage:
    python timber_torpedo_3d.py --list              # List all parts
    python timber_torpedo_3d.py --export-all        # Export all parts as STL
    python timber_torpedo_3d.py --part board        # Export just the board
    python timber_torpedo_3d.py --format obj        # Export as OBJ instead
    python timber_torpedo_3d.py --scale 0.1         # Scale to 10% (for test prints)
    python timber_torpedo_3d.py --split             # Export each part separately
"""

import argparse
import sys
from pathlib import Path

try:
    import numpy as np
except ImportError:
    print("ERROR: numpy is required. Install with: pip install numpy")
    sys.exit(1)

try:
    import trimesh
except ImportError:
    print("ERROR: trimesh is required. Install with: pip install trimesh")
    sys.exit(1)

# Optional: cadquery for STEP export
try:
    import cadquery as cq
    HAS_CADQUERY = True
except ImportError:
    HAS_CADQUERY = False


# ============================================================================
# Model Dimensions (all in inches - can scale for printing)
# ============================================================================
DIMENSIONS = {
    "length": 53.0,           # 4'5" total length
    "width": 28.0,            # 28" width
    "thickness": 1.5,         # Board thickness
    "drive_hole_diameter": 13.0,  # Center hole for drive wheel
    "caster_bracket_radius": 1.2,
    "caster_bracket_height": 1.0,
    "caster_wheel_radius": 1.5,
    "caster_wheel_width": 0.8,
    "caster_height": 3.0,     # Total caster assembly height
}


def create_elliptical_board(length, width, thickness, hole_diameter, resolution=64):
    """Create the main elliptical board with center hole."""
    from shapely.geometry import Polygon as ShapelyPolygon
    
    # Create ellipse points
    theta = np.linspace(0, 2 * np.pi, resolution, endpoint=False)
    outer_x = (width / 2) * np.cos(theta)
    outer_y = (length / 2) * np.sin(theta)
    
    # Create inner circle (hole) points
    hole_radius = hole_diameter / 2
    inner_x = hole_radius * np.cos(theta)
    inner_y = hole_radius * np.sin(theta)
    
    # Build vertices for the 2D shape
    outer_verts = list(zip(outer_x, outer_y))
    inner_verts = list(zip(inner_x, inner_y))
    
    # Create shapely polygon with hole
    outer_poly = ShapelyPolygon(outer_verts, [inner_verts])
    
    # Extrude to 3D
    board = trimesh.creation.extrude_polygon(outer_poly, height=thickness)
    
    return board


def create_caster_bracket(radius, height):
    """Create a caster mounting bracket (cylinder)."""
    bracket = trimesh.creation.cylinder(
        radius=radius,
        height=height,
        sections=32
    )
    return bracket


def create_caster_wheel(radius, width):
    """Create a caster wheel (cylinder on its side)."""
    wheel = trimesh.creation.cylinder(
        radius=radius,
        height=width,
        sections=32
    )
    # Rotate to be horizontal (wheel orientation)
    wheel.apply_transform(trimesh.transformations.rotation_matrix(
        np.pi / 2, [1, 0, 0]
    ))
    return wheel


def create_full_caster(bracket_radius, bracket_height, wheel_radius, wheel_width):
    """Create a complete caster assembly (bracket + wheel)."""
    bracket = create_caster_bracket(bracket_radius, bracket_height)
    wheel = create_caster_wheel(wheel_radius, wheel_width)
    
    # Position wheel below bracket
    wheel.apply_translation([0, 0, -bracket_height / 2 - wheel_radius])
    bracket.apply_translation([0, 0, 0])
    
    # Combine into single mesh
    caster = trimesh.util.concatenate([bracket, wheel])
    return caster


def get_caster_positions(width, length):
    """Get the 4 caster positions."""
    return [
        {"name": "front_left",  "x": -width/4, "z": -length/3},
        {"name": "front_right", "x":  width/4, "z": -length/3},
        {"name": "rear_left",   "x": -width/4, "z":  length/3},
        {"name": "rear_right",  "x":  width/4, "z":  length/3},
    ]


def generate_all_parts(dims=None, scale=1.0):
    """Generate all parts of the Timber Torpedo model."""
    if dims is None:
        dims = DIMENSIONS
    
    parts = {}
    
    # Main board
    print("  Generating board...")
    board = create_elliptical_board(
        dims["length"],
        dims["width"],
        dims["thickness"],
        dims["drive_hole_diameter"]
    )
    # Position board at correct height
    board.apply_translation([0, 0, dims["caster_height"]])
    parts["board"] = board
    
    # Casters
    caster_positions = get_caster_positions(dims["width"], dims["length"])
    for pos in caster_positions:
        print(f"  Generating caster: {pos['name']}...")
        caster = create_full_caster(
            dims["caster_bracket_radius"],
            dims["caster_bracket_height"],
            dims["caster_wheel_radius"],
            dims["caster_wheel_width"]
        )
        # Position caster
        caster.apply_translation([
            pos["x"],
            pos["z"],  # Note: y/z swap for top-down view
            dims["caster_height"] - dims["caster_bracket_height"] / 2
        ])
        parts[f"caster_{pos['name']}"] = caster
    
    # Apply scale if not 1.0
    if scale != 1.0:
        print(f"  Applying scale: {scale}")
        for name, mesh in parts.items():
            mesh.apply_scale(scale)
    
    return parts


def generate_assembly(dims=None, scale=1.0):
    """Generate complete assembled model."""
    parts = generate_all_parts(dims, scale)
    assembly = trimesh.util.concatenate(list(parts.values()))
    return assembly


def export_mesh(mesh, filepath, format=None):
    """Export mesh to file."""
    filepath = Path(filepath)
    
    if format:
        filepath = filepath.with_suffix(f".{format}")
    
    mesh.export(str(filepath))
    print(f"  Exported: {filepath}")
    return filepath


def export_step(dims, filepath, scale=1.0):
    """Export to STEP using cadquery (if available)."""
    if not HAS_CADQUERY:
        print("ERROR: cadquery not installed. Install with: pip install cadquery-ocp")
        print("       Or use: conda install -c conda-forge cadquery")
        return None
    
    print("  Generating CAD model with cadquery...")
    
    # Create the board as a proper CAD solid
    board = (
        cq.Workplane("XY")
        .ellipse(dims["width"] / 2, dims["length"] / 2)
        .extrude(dims["thickness"])
        .faces(">Z")
        .workplane()
        .hole(dims["drive_hole_diameter"])
        .translate((0, 0, dims["caster_height"]))
    )
    
    if scale != 1.0:
        # Scale the model
        board = board.val().scale(scale)
    
    # Export to STEP
    filepath = Path(filepath).with_suffix(".step")
    cq.exporters.export(board, str(filepath))
    print(f"  Exported STEP: {filepath}")
    return filepath


def main():
    parser = argparse.ArgumentParser(
        description="Timber Torpedo 3D Model Generator & Converter",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python timber_torpedo_3d.py --list
  python timber_torpedo_3d.py --export-all --format stl
  python timber_torpedo_3d.py --part board --format obj
  python timber_torpedo_3d.py --assembly --format stl
  python timber_torpedo_3d.py --split --scale 0.1 --output ./prints/
  python timber_torpedo_3d.py --step  # Export as STEP (requires cadquery)
        """
    )
    
    parser.add_argument("--list", action="store_true",
                        help="List all available parts")
    parser.add_argument("--export-all", action="store_true",
                        help="Export all parts as separate files")
    parser.add_argument("--assembly", action="store_true",
                        help="Export complete assembled model")
    parser.add_argument("--part", type=str,
                        help="Export specific part by name")
    parser.add_argument("--split", action="store_true",
                        help="Split and export each part separately")
    parser.add_argument("--step", action="store_true",
                        help="Export as STEP file (requires cadquery)")
    
    parser.add_argument("--format", type=str, default="stl",
                        choices=["stl", "obj", "ply", "off", "glb", "gltf"],
                        help="Output format (default: stl)")
    parser.add_argument("--scale", type=float, default=1.0,
                        help="Scale factor (e.g., 0.1 for 10%% size)")
    parser.add_argument("--output", "-o", type=str, default="./output",
                        help="Output directory (default: ./output)")
    parser.add_argument("--mm", action="store_true",
                        help="Convert inches to mm (multiply by 25.4)")
    
    args = parser.parse_args()
    
    # Create output directory
    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # Adjust scale for mm if requested
    scale = args.scale
    if args.mm:
        scale *= 25.4
        print(f"Converting to mm (scale factor: {scale})")
    
    # List parts
    if args.list:
        print("\nAvailable parts:")
        print("  - board (main elliptical deck with center hole)")
        print("  - caster_front_left")
        print("  - caster_front_right")
        print("  - caster_rear_left")
        print("  - caster_rear_right")
        print("\nDimensions (inches):")
        for key, val in DIMENSIONS.items():
            print(f"  {key}: {val}")
        print(f"\ncadquery available for STEP export: {HAS_CADQUERY}")
        return
    
    # STEP export
    if args.step:
        export_step(DIMENSIONS, output_dir / "timber_torpedo", scale)
        return
    
    # Generate parts
    print("\nGenerating 3D model...")
    parts = generate_all_parts(scale=scale)
    
    # Export specific part
    if args.part:
        if args.part not in parts:
            print(f"ERROR: Unknown part '{args.part}'")
            print(f"Available: {', '.join(parts.keys())}")
            return
        export_mesh(parts[args.part], output_dir / args.part, args.format)
        return
    
    # Export all parts separately
    if args.export_all or args.split:
        print(f"\nExporting {len(parts)} parts to {output_dir}/")
        for name, mesh in parts.items():
            export_mesh(mesh, output_dir / name, args.format)
        return
    
    # Export assembly (default if no specific action)
    if args.assembly or not any([args.list, args.export_all, args.part, args.split, args.step]):
        print("\nExporting complete assembly...")
        assembly = trimesh.util.concatenate(list(parts.values()))
        export_mesh(assembly, output_dir / "timber_torpedo_assembly", args.format)


if __name__ == "__main__":
    main()
