# Tucil3_13524027_13524077 - Sliding Ice Puzzle

## Description

Solver for Sliding Ice Puzzle with GUI editor and playback. Uses UCS, A*, or GBFS.

## Dependencies

- C++17 compiler
- CMake ≥ 3.14
- SFML 2.5+

## Build

```bash
cmake -B build
cmake --build build -j$(nproc)
```

## Run

**Editor mode** (no args — opens grid editor):
```bash
./build/sliding
```

**Player mode** (with puzzle file — CLI prompts for algorithm):
```bash
./build/sliding tests/test1.txt
```

## Usage

- **Editor**: Left-click to paint tiles, right-click to cycle weight. LOAD/SAVE/PLAY buttons or keyboard shortcuts (L/S/W/R). PLAY opens algorithm selection dialog.
- **Algorithm dialog**: Click UCS/A*/GBFS, then PLAY. Enter key also opens the dialog.
- **Player**: Step through solution with slider. Shows step count, cost, iterations, and elapsed time.

## File Format

```
<rows> <cols>
<row0 chars>
<row1 chars>
...
<weight_row0 tab-separated>
<weight_row1 tab-separated>
...
```

Tiles: `X` wall, `*` floor, `Z` start, `O` goal, `L` lava, `0-9` checkpoints.

## Project Structure

```
src/
  main.cpp
  core/algorithm/aStar.cpp, ucs.cpp, gbfs.cpp
  core/graph.cpp
  ui/grideditor.cpp, gameui.cpp, snapshot.cpp
  utils/fileIO.cpp
include/
  core/algorithm/aStar.hpp, ucs.hpp, gbfs.hpp
  core/graph.hpp
  ui/grideditor.hpp, gameui.hpp, snapshot.hpp
  utils/fileIO.hpp
```
