# Development

`minifetch` is an original, lightweight system information utility developed specifically for MINIX 3.

The project was created to provide a native MINIX alternative to terminal-based system information utilities such as `neofetch`, using the system interfaces available on MINIX rather than relying on Linux-specific facilities or 
compatibility layers.

## Development process

The project was developed through iterative AI-assisted collaboration with ChatGPT. Much of the source code, system information collection logic, terminal display implementation, build configuration, and documentation was generated or 
refined during this collaboration, with the resulting implementation compiled, tested, debugged, and validated on a MINIX 3 system.

The project maintainer directed the development, selected features, evaluated implementation approaches, reviewed results, and performed testing within the target environment.

Development and testing were performed on MINIX 3.4.0 (i386) running in a QEMU/KVM virtual machine.

The development environment included:

- Clang and BSD make
- X11 with the dwm window manager
- xterm and remote SSH terminal sessions
- ANSI 256-color and true-color terminal output

The resulting program is a standalone native C application and does not require AI services to build or operate.

## Technical implementation

The implementation was developed by investigating the system information interfaces available on MINIX 3.4.0 and adapting them to the requirements of a terminal-based system information utility.

Development included:

- Investigating MINIX-compatible methods for retrieving operating system identification, uptime, CPU information, memory usage, and load averages.
- Implementing filesystem capacity and usage reporting.
- Developing environment-dependent detection of display, window manager, terminal, and shell information.
- Designing fixed-width information panels with ANSI colors and Unicode borders.
- Implementing automatic layout selection based on terminal width.
- Providing ASCII display alternatives and plain-text output for redirected streams.
- Integrating ANSI-colored MINIX mascot artwork with user-specific and system-wide lookup paths.
- Developing a BSD make build system with installation and uninstallation targets.

The implementation avoids Linux-specific `/proc` conventions and does not require a Linux compatibility layer.

## Source organization

The application is divided into separate modules for system information collection and presentation.

| Module | Responsibility |
|---|---|
| `main.c` | Command-line processing and program coordination |
| `system.c` | Operating system identification and uptime |
| `cpu.c` | CPU information |
| `memory.c` | Physical memory information |
| `load.c` | System load averages |
| `filesystem.c` | Filesystem capacity and usage |
| `ui.c` | Display, window manager, terminal, and shell detection |
| `display.c` | Terminal formatting, artwork, colors, and layout |

Information-gathering operations are handled independently so that unavailable information does not prevent the remaining system details from being displayed.

## Artwork provenance

The included ANSI artwork depicts Rocky Raccoon, the MINIX 3 mascot.

The artwork was derived from a JPEG image available through Wikimedia Commons. The image background was removed to produce a transparent PNG, which was subsequently converted into ANSI-colored text using `jp2a`.

The intermediate PNG and original JPEG are not distributed with the project. Only the resulting ANSI artwork is included.

The adapted artwork is distributed under Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0), separately from the MIT-licensed program source code.

Additional attribution and licensing information is provided in [artwork/README.md](artwork/README.md).

## Testing and validation

The implementation was tested directly on MINIX 3.4.0 throughout development.

Testing included:

- Repeated compilation using Clang and BSD make.
- Verifying system information against available MINIX utilities and system interfaces.
- Testing native X11 and remote SSH terminal environments.
- Verifying Unicode and ASCII display styles.
- Testing terminal-width-dependent layout selection.
- Verifying plain-text output when redirected through shell pipelines.
- Testing terminal color palette rendering.
- Installing the executable and artwork under `/usr/local`.
- Confirming that the installed artwork is used when no user-specific artwork is available.

The project was developed using iterative manual testing and diagnostic programs. No automated regression test suite is currently included in the distributed repository.

## Limitations

The project has been developed and tested on MINIX 3.4.0 (i386).

Compatibility with other MINIX releases, architectures, or operating systems has not been established.

Some system information may be unavailable depending on the runtime environment. Display and window manager detection are particularly dependent on the active graphical session and available utilities.

Filesystem reporting currently targets `/`, `/usr`, and `/home`.

The project remains experimental.

## Licensing

The original `minifetch` C source code is released under the MIT License.

The included Rocky Raccoon ANSI artwork is distributed separately under CC BY-SA 4.0.

See [LICENSE](LICENSE) and [artwork/README.md](artwork/README.md) for details.
