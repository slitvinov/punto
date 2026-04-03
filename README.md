# punto

A particle visualization tool. Reads coordinate data from ASCII files
and displays them as an interactive 3D scene using SDL2.

Originally written by Mariano Revenga (1998-2009).

## Features

- 1D, 2D, and 3D visualization
- Display modes: circles, squares, spheres
- Color gradients with 5 palettes (spectrum, red, green, blue, grey)
- Vector field plotting
- Animation with multiple data blocks
- Interactive rotation, zoom, and panning
- Particle filtering by range
- Trace particle trajectories
- Periodic boundary images
- Frame export to BMP

## Building

Requires SDL2.

    cd src && make

On macOS with Homebrew:

    brew install sdl2
    cd src && make

## Usage

    punto [options] file

Generate sample data and run the tutorial:

    cd src && make data
    cd .. && sh TUTORIAL

### Options

| Option | Description |
|--------|-------------|
| `-s N` | max particle size in pixels (default: 12) |
| `-r [min:max]` | enable radius column |
| `-c` | enable color column |
| `-G [min:max]` | gradient color scale |
| `-p N` | palette: 0 spectrum, 1 red, 2 green, 3 blue, 4 grey |
| `-V` | vector field mode |
| `-B [x0:y0:z0:x1:y1:z1]` | border box |
| `-D N` | force dimension to N |
| `-t N` | type: 0 circles, 1 squares, 2 spheres |
| `-z a:b:c:d:e` | choose columns to plot |
| `-g WxH` | window geometry |
| `-d cs` | delay between frames (centiseconds) |
| `-n N1:N2` | plot only particles N1 to N2 |
| `-bg color` | background color |
| `-fg color` | foreground color |
| `-trace [n]` | trace mode with n points |

### Keyboard

| Key | Action |
|-----|--------|
| `4`/`7` | rotate X axis |
| `5`/`8` | rotate Y axis |
| `6`/`9` | rotate Z axis |
| `0` | reset view |
| `z`/`Z` | zoom in/out |
| `i`/`I` | increase/decrease particle size |
| arrows | pan |
| `s` | start/stop animation |
| `r` | reverse animation |
| `space` | step one frame |
| `+`/`-` | increase/decrease delay |
| `.` | reset delay |
| `b` | toggle border box |
| `c` | toggle comment |
| `t` | toggle trace |
| `p` | toggle periodic images |
| `a` | cycle arrow type (vector mode) |
| `F` | save current frame |
| `S` | toggle save-all mode |
| `q` | quit |

## Data format

ASCII file with space/tab-separated columns. Minimum one column (x).
Additional columns: y, z, radius, color, vector components.

Blocks are separated by blank or comment lines. Comment lines start
with `#`. Multiple blocks produce an animation.

## License

GNU General Public License v2. See COPYING.
