# HDT-SMP Force Fields

A Skyrim SKSE plugin that adds customizable force fields to the game, compatible with HDT-SMP64 3.0.0 and later.

## Features

- **Multiple Force Field Types**: Spherical, cylindrical, planar, cone, and vortex force fields
- **Dynamic Configuration**: XML-based configuration system with per-object customization
- **Performance Optimized**: Multi-threaded force application with dynamic threading policy selection
- **Full Runtime Support**: Works with Skyrim SE, Skyrim AE, and Skyrim VR via a single binary
- **Address Library Support**: Uses address library for version independence across different Skyrim editions

## Installation

### For Mod Managers (Vortex, MO2)

1. Download the latest release from [GitHub Releases](https://github.com/YOUR-USERNAME/hdtSMP-force-fields-NG/releases)
2. Extract the `.zip` file into your Skyrim directory or let your mod manager handle it
3. The plugin will be installed to `SKSE/Plugins/HDT-SMP Force Fields.dll`

### Manual Installation

Extract the downloaded archive so that the `SKSE/` folder merges with your Skyrim installation directory.

## Configuration

The plugin creates a config file at `Data/SKSE/Plugins/HDT-SMP Force Fields.ini` on first run:

```ini
[General]
; Log frame-by-frame performance statistics
bLogStatistics=false

[ForceFields]
; Global multiplier for all force field strengths
fForceMultiplier=1.0

[Performance]
; Number of worker threads (0-128; 0 disables multi-threading)
; Default: 8
iThreads=8
```

### Adding Force Fields to Objects

Attach force fields to NIF geometry by adding a `NiStringsExtraData` node named `"HDT-SMP Force Field"` with the following format:

```
Field Type:     SPHERICAL, CYLINDRICAL, PLANAR_BOX, PLANAR_CYL, PLANAR_SPH, VORTEX
Parameters:     Space-separated values with type prefix (f=float, i=int, v=vec3, q=quaternion)
```

**Example**: 
```
SPHERICAL
fRadius=100.0 fStrength=50.0
```

## Requirements

- **Skyrim SE/AE/VR** with SKSE installed
- **HDT-SMP64 3.0.0** or later
- **Address Library** (for version independence)

## Building from Source

### Prerequisites

- Visual Studio 2022 or later
- CMake 3.21+
- vcpkg (managed automatically by the build system)
- Git (for cloning dependencies)

### Build Steps

```bash
git clone https://github.com/YOUR-USERNAME/hdtSMP-force-fields-NG.git
cd hdtSMP-force-fields-NG
cmake --preset ALL
cmake --build build --config Release --parallel
```

The compiled plugin will be at `build/Release/HDT-SMP Force Fields.dll`

## CI/CD

This project includes GitHub Actions workflows that:
- Build the plugin automatically on every push and pull request
- Create releases automatically when you push a tag (e.g., `v0.9.1`)
- Package the plugin in Vortex/MO2-compatible format
- Upload build artifacts for every commit

## Development

### Code Structure

- **`hdtSMP-force-fields/`** — Plugin source code
  - `ForceFieldManager.*` — Core force field management and threading
  - `factories/` — Force field type implementations
  - `objects/` — Trait classes for different object types
  - `hooks.cpp` — Engine hooks for attaching/detaching force fields
  
- **`include/`** — Public headers
  - `hdtSMP64/PluginAPI.h` — Interface for communicating with HDT-SMP64
  
- **`cmake/`** — Build configuration
  - `Plugin.h.in` — CMake template for version info
  - `ports/` — Custom vcpkg port overlays

### Dependencies

- **CommonLibVR** — Skyrim RE structures and SKSE bindings (includes OpenVR headers)
- **Bullet Physics** — Physics engine for force calculations
- **spdlog** — Logging library
- **xbyak** — Assembler library for SKSE address library support
- **detours** — Microsoft Detours for engine hooking
- **ClibUtil** — Utility library for INI parsing and singletons

## Credits

- **Original Author**: jgernandt
- **HDT-SMP Support**: Updated to work with HDT-SMP64 3.0.0 with assistance from Claude Sonnet
- **GitHub Actions CI/CD**: Implemented with assistance from Claude Sonnet
- **Bullet Physics**: Used for physics-based force calculations
- **SKSE Team**: For the Script Extender framework

## License

MIT License — See LICENSE file for details

## Troubleshooting

### Plugin doesn't load

- Verify SKSE is installed and running
- Check that HDT-SMP64 3.0.0+ is installed
- Look for errors in `My Games/Skyrim SE/SKSE/SKSE.log`

### Force fields not working

- Ensure objects have proper `NiStringsExtraData` nodes with correct format
- Check `Data/SKSE/Plugins/HDT-SMP Force Fields.log` for parsing errors
- Verify the field type name matches exactly (case-sensitive)

### Performance issues

- Reduce `iThreads` in the INI if using many force fields
- Enable `bLogStatistics` to identify which frames are slow
- Limit the number of active force fields simultaneously

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Support

For issues, questions, or feature requests, please open an issue on GitHub.

---

**Built with** [CommonLibVR](https://github.com/alandtse/CommonLibVR) | **Managed by** [CMake](https://cmake.org/) | **Distributed via** GitHub Releases
