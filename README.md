# minifetch

**minifetch** is a lightweight system information utility written in C for MINIX 3.

Inspired by terminal-based system information tools such as `neofetch`, it displays system, hardware, and desktop information alongside ANSI-colored artwork of Rocky Raccoon, the MINIX 3 mascot.

The program is designed to run natively on MINIX without requiring Linux compatibility layers or external scripting runtimes.

## Features

- MINIX operating system, release, version, and hostname
- System uptime
- CPU identification and clock speed
- Memory usage
- System load averages
- Filesystem usage for `/`, `/usr`, and `/home`
- Display, window manager, terminal, and shell information
- ANSI 16-color terminal palette
- Unicode or ASCII display borders
- Automatic layout adjustment based on terminal width
- Plain-text output when redirected or piped

The display is organized into three information sections: **SYSTEM**, **HARDWARE**, and **UI**, with a separate terminal color palette.

## Requirements

- MINIX 3
- C compiler (tested with Clang)
- BSD make
- ANSI-compatible terminal for colored output

The `xprop` utility is used when available to assist with window manager detection. It is not required for basic operation.

## Building

Clone the repository and build:

```sh
git clone https://github.com/rcbellN51/minifetch.git
cd minifetch
make
```

Run directly from the build directory:

```sh
./minifetch
```

## Installation

Install the executable and its artwork:

```sh
su
make install
exit
```

The default installation paths are:

| File | Location |
|---|---|
| Executable | `/usr/local/bin/minifetch` |
| Artwork | `/usr/local/share/minifetch/minix.ascii` |

A different installation prefix can be specified with `PREFIX`. The Makefile also supports `DESTDIR` for staged installations.

When changing `PREFIX`, perform a clean rebuild so the correct artwork directory is compiled into the executable.

To uninstall:

```sh
su
make uninstall
exit
```

## Usage

```text
minifetch [--ascii | --unicode]
```

| Option | Description |
|---|---|
| `--unicode` | Use Unicode borders and color swatches (default) |
| `--ascii` | Use ASCII borders and ASCII color swatches |

### Display modes

The output automatically adapts to terminal width:

- **104 columns or wider:** Artwork alongside information boxes.
- **55–103 columns:** Information boxes without artwork.
- **Narrower than 55 columns:** Plain-text output.

When standard output is redirected or piped, the program also uses plain-text output.

## Artwork configuration

By default, `minifetch` looks for artwork in:

```text
~/.config/minifetch/minix.ascii
```

If no user-specific artwork is found, it uses the system-wide artwork installed at:

```text
/usr/local/share/minifetch/minix.ascii
```

Users can customize the artwork by replacing their personal `minix.ascii` file.

## Compatibility

Developed and tested on **MINIX 3.4.0 (i386)**, running in a QEMU/KVM virtual machine.

Testing includes native X11 sessions using `dwm` and terminal sessions over SSH.

Compatibility with other MINIX versions and architectures has not yet been established.

## Development

For implementation notes, development history, testing, and project provenance, see [DEVELOPMENT.md](DEVELOPMENT.md).

## Licensing

The original `minifetch` program source code is released under the **MIT License**. See [LICENSE](LICENSE).

The included Rocky Raccoon artwork is derived from an image distributed under **Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)** and is distributed separately under that license.

See [artwork/README.md](artwork/README.md) for attribution and details of the adaptation.

`minifetch` is an independent project and is not officially affiliated with or endorsed by the MINIX project.
