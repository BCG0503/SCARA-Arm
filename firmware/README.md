# Firmware Folder Structure

This file documents the purpose and recommended contents of the `firmware/` directory.

Layout overview

`local/`
  - Purpose: Host-side Python scripts and development helpers that run on a developer's machine. Typical contents are control scripts, calibration tools, data-loggers, test utilities, and flash/upload helpers implemented in Python.
  - Notes: Do not commit secrets or machine-specific binary caches. Keep scripts lightweight and document usage. Use `.gitignore` for transient files.

`scara/`
  - Purpose: Firmware that runs on the SCARA arm's microcontroller. This is the code deployed to the board (via PlatformIO) and handles real-time control, drivers, sensor reading, and motion execution. Host-side utilities live in `local/` while `scara/` contains the on-device software.
  - Contents: Shared modules, utilities, or common code used by multiple firmware projects. Keep this code modular and well-documented.

- `scara/`
  - Purpose: PlatformIO project for the SCARA arm firmware.
  - Contents:
    - `platformio.ini`: Project configuration for PlatformIO — environments, build flags, and upload settings.
    - `include/`: Public headers and small docs for the project.
    - `lib/`: Local libraries specific to this project. Prefer adding reusable code here rather than directly in `src/` when it's project-scoped.
    - `src/`: Project source files (entry point like `main.cpp`, drivers, tasks).
  - Naming: Keep source files descriptive and avoid long compound filenames. Use `kebab_case` or `snake_case` consistent with existing code.

General guidelines

- Build system: This repo uses PlatformIO for embedded builds. Typical commands (run from the `scara/` directory):

```powershell
cd firmware\scara
platformio run
# or to upload to a connected board (configured environment):
platformio run -t upload -e <env_name>
```

- Environments: Define environment names in `platformio.ini` and reference them in build/upload commands. Use clear environment names like `esp32_dev`, `stm32_release`, etc.

- Libraries: Prefer PlatformIO `lib/` for project-local libraries. For shared libraries used by multiple projects, consider moving them to the top-level `firmware/src/` or a `libs/` folder at repo root.

- Configuration: Keep hardware-specific settings (pins, calibration constants) in a single header or config file, and document required changes for different boards.

- Versioning & commits: When changing firmware behavior or interfaces, increment a version macro or record a short changelog in `scara/CHANGELOG.md` and commit with descriptive messages (e.g., "scara: add encoder calibration and bump version to v1.2").

- Large/Generated files: Avoid committing large binary build artifacts. Add `.pio` and other build output directories to `.gitignore` if not already excluded.