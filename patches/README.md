# Upstream diffs

Both upstream projects are vendored into this repository. The same changes are
kept here as standalone patches, which makes it straightforward to review them,
submit them upstream, or rebase onto a newer upstream revision.

| Patch | Upstream | Base commit |
|-------|----------|-------------|
| `fallout1-ce-ps3.patch` | [alexbatalov/fallout1-ce](https://github.com/alexbatalov/fallout1-ce) | `0609bcfd0ec40ff0571d0f57fab2821eb461dc8b` |
| `sdl2-psl1ght-ps3.patch` | [ps3aqua/SDL2_PSL1GHT](https://github.com/ps3aqua/SDL2_PSL1GHT) | `ff407b9a700e44c952f8c98ab12ba83e06cf0286` |

Reapplying, for example:

```bash
git clone https://github.com/alexbatalov/fallout1-ce
cd fallout1-ce
git checkout 0609bcfd
patch -p1 < ../patches/fallout1-ce-ps3.patch
```

## Changes worth upstreaming

Some of these are not PS3-specific — they are correctness fixes that apply on
every platform:

- **`movie_lib.cc`, `getOffset()` return type `uint8_t` → `int`.** Motion
  offsets are truncated to 0–255, which corrupts blocks during MVE playback.
  The equivalent code in fallout2-ce computes the same value as a full `int`.
- **`movie_lib.cc`, endian-safe stream access** (`mveStreamU16`,
  `storeUInt32LE`, header fields in `_ioReset`). Byte layout on little-endian
  hosts is bit-for-bit identical; big-endian hosts get a working decoder.
- **`plib/gnw/text.cc`, `.fon` header fields** read through `SDL_SwapLE32`
  instead of raw struct access.

## PS3-specific changes

These stay downstream:

- `cmake/toolchain/ps3.cmake` and the `if(PS3)` branch in `CMakeLists.txt`.
- `src/platform/ps3/` — data path definition and the on-screen keyboard bridge.
- Path resolution rooted at the game data directory, with a resolution cache
  (`platform_compat.cc`).
- Gamepad-to-mouse emulation and the keyboard shortcut map
  (`plib/gnw/dxinput.cc`, `plib/gnw/input.cc`).
- Audio sample format switched to `AUDIO_S16SYS`, plus an `SDL_mutex` wrapper
  standing in for `std::recursive_mutex` (newlib has no pthreads).
- Video initialisation without OpenGL flags (`plib/gnw/svga.cc`).
- On-screen keyboard hooks in the three text entry paths (`game/editor.cc`,
  `game/loadsave.cc`, `game/gdialog.cc`).
