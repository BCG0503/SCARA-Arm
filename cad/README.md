# CAD Folder Structure

This file documents the purpose and recommended contents of the `cad/` subfolders in this repository.

- `archive/`:
  - Purpose: Historical snapshots and backups of CAD files. Use for older revisions that should be retained but not actively edited.
  - Contents: ZIPs, dated folders, or exported STEP/IGES copies of previous milestones.

- `assemblies/`:
  - Purpose: Top-level assembly files and subassemblies that describe how parts fit together.
  - Contents: Assembly files (e.g., `.step`, `.sldasm`, `.iam`) and subassembly folders. Keep one primary assembly file per major product state.
  - Naming: `A_<name>_v###.<ext>` (e.g., `A_scara_arm_v001.step`).

- `exports/`:
  - Purpose: Export-ready formats for manufacturing, sharing, or simulation.
  - Contents: STL for 3D printing, DXF for laser cutting, STEP/IGES for vendors, PDF drawings.
  - Notes: Exports should be generated from the canonical files in `parts/` or `assemblies/` and include the version in the filename.

- `parts/`:
  - Purpose: Individual part models used by assemblies.
  - Contents: Native part files and neutral exports (e.g., `.step`, `.sldprt`).
  - Naming: `P_<name>_v###.<ext>` (e.g., `P_base_plate_v002.step`).

- `references/`:
  - Purpose: Supplier-provided models, reference geometry, and hardware library.
  - Structure:
    - `components/`: Manufacturer 3D models (e.g., step files for motors, bearings). Do not modify originals — copy or reference only.
    - `hardware/`: Standard fasteners, brackets, and purchased items with datasheets.
  - Example: `references/components/MS17HD6P-3D-Model.step`.

General guidelines

- File formats: Prefer neutral interchange formats for sharing — `.step` / `.stp` for assemblies and parts, `.stl` for 3D printing, `.dxf` for 2D outlines. Keep native CAD files if available.
- Versioning: Include a simple `_v###` suffix in filenames and increment on non-trivial changes. For major revisions consider copying the old file into `archive/`.
- Source of truth: Treat files in `parts/` and `assemblies/` as canonical. `exports/` should be reproducible from them.
- Commits: Commit CAD files with descriptive messages mentioning the part/assembly and version (e.g., "Add P_base_plate_v002.step — added mounting holes").
- Large files: If repo size grows, consider storing very large binary CAD files in a release or external artifact storage and keep a lightweight reference here.