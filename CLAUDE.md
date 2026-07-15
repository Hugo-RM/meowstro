# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Meowstro is a C++17/SDL2 rhythm game: a cat fishes from a boat, fish swim in from the right on a
beat map, and the player presses SPACE when a fish crosses the hook to score Perfect/Good/Miss.
Built with CMake; targets Windows (vcpkg), Linux, and macOS (system SDL2 packages).

## Build & test (Linux/WSL)

```bash
# Configure + build
cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build

# Run the game
./build/bin/Debug/meowstro

# Run all tests (builds first, runs gtest binary, then ctest)
./test.sh

# Run a single test / filtered subset directly via gtest
./build/bin/Debug/meowstro_tests --gtest_filter=GameStatsTest.*

# Or via ctest (only runs as one registered "unit_tests" test, so prefer the gtest binary above for filtering)
cd build && ctest --output-on-failure
```

Debug binaries land in `build/bin/Debug`, Release in `build/bin/Release` (same layout on all
platforms). Assets are copied into that directory as a post-build step, so the game/tests must be
run from an environment where `build/bin/<Config>/assets` exists (handled automatically by the
build).

If SDL2 dev packages aren't installed locally, a Docker container is a reliable way to get an exact
match for the versions in `DEPENDENCIES.md` (`ubuntu:24.04` + `apt-get install libsdl2-dev
libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libgtest-dev googletest`). The built game can be
smoke-tested headlessly with `Xvfb` + `SDL_AUDIODRIVER=dummy` (or a null ALSA sink for realistic
audio-position timing — a true `dummy`/null audio sink does not pace playback to real time, so
`Mix_GetMusicPosition` races through the whole track almost instantly; don't use it to judge
gameplay timing, only to check nothing crashes) and `xdotool` to inject key events.

When adding a new `.cpp`/`.hpp`, add it to **three** places in `CMakeLists.txt`: `SOURCES`/`HEADERS`
(main exe), `GAME_LIB_SOURCES`/`GAME_LIB_HEADERS` (the `meowstro_lib` static lib used by tests), and
— if it has tests — the `meowstro_tests` executable's source list.

CI (`.github/workflows/build.yml`) runs cppcheck and clang-tidy as static analysis gates before
building, on both Windows and Linux, for every branch push.

## Architecture

**Game loop / state machine** — `GameStateManager` owns a `GameState` enum
(`MainMenu -> Playing -> EndScreen`, plus `Quit`) and drives everything from `run()`, dispatching to
`MenuSystem` (menus/end screen) or `RhythmGame` (gameplay) per state. Input is funneled through
`InputHandler::processInput`, which maps raw SDL events to an `InputAction` enum
(`Select`/`MenuUp`/`MenuDown`/`Escape`/`Quit`/`ToggleFullscreen`/`ToggleMute`/`None`) — game logic
never touches `SDL_Event`/keycodes directly. `ToggleFullscreen` (F11) is recognized universally in
`InputHandler::processInput` regardless of state and is handled independently in each of the three
small event loops (`GameStateManager::runGameplay`, `MenuSystem::runMainMenu`/`runEndScreen`) by
calling `RenderWindow::toggleFullscreen()`. Menu navigation is keyboard-only (no mouse support).

**Rhythm timing model** — one audio clock drives both hit detection and visuals.
`RhythmGame::getCurrentGameTimeMs()` prefers `Mix_GetMusicPosition()` (via
`Audio::getMusicPositionMs()`), falling back to `SDL_GetTicks() - m_songStartTime` if the mixer can't
report position. This same `currentTime` value is used for two things every frame:
- *Hit/miss judgment* (`handleRhythmInput`, `checkMissedNotes`) against
  `GameConfig::GameplayConfig::noteBeats` — a hand-transcribed list of song timestamps built via
  `AudioLogic::msFromMscs(min, sec, centisec)` in `GameConfig::initializeBeatTimings()`. These
  timestamps were spot-checked against the actual track with an onset detector (see
  `docs/design_notes.md`) and look accurate; no evidence they need correcting.
- *Fish position* (`RhythmGame::updateFishMovement`) via `AudioLogic::interpolatePosition`, a pure
  function that linearly interpolates each fish from `GameplayConfig::fishStartXLocations[i]` to
  `GameplayConfig::fishTargetX`, timed so it arrives exactly at `noteBeats[i]` after
  `AudioConfig::travelDuration` ms of travel (clamped before spawn, extrapolated past a miss). Because
  this is a pure function of the same `currentTime` used for hit detection — not a per-frame pixel
  step — fish position stays locked to the hit window regardless of framerate. This routine is unit
  tested directly in `tests/unit/test_AudioLogic.cpp` without needing SDL. Fish swim-frame animation
  (fin flap) is stepped on its own fixed wall-clock interval (`m_lastFishFrameTicks`) so its speed
  doesn't vary with framerate either. `RhythmGame`'s frame pacing targets 60 FPS via
  `SDL_GetPerformanceFrequency() / 60` and `SDL_Delay`.

**Window / rendering** — `RenderWindow` creates the window with
`SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI` and calls `SDL_RenderSetLogicalSize(renderer, w, h)`
right after creating the renderer, where `w`/`h` are the fixed 1920x1080 design resolution every
entity's hard-coded pixel position assumes. This is what makes resizing the window, entering
fullscreen (`toggleFullscreen()`, desktop/borderless via `SDL_WINDOW_FULLSCREEN_DESKTOP`), and HiDPI
displays all work without touching any of the hard-coded UI coordinates — SDL letterboxes and scales
automatically. Renderer creation falls back from `SDL_RENDERER_ACCELERATED` to `SDL_RENDERER_SOFTWARE`
if no accelerated driver is available (VMs, some Linux setups) instead of refusing to launch.

**Entity/animation system** — `Entity` is a static drawable (position + `SDL_Texture*`); `Sprite`
extends it with a frame grid (rows/cols) for animation. `AnimationSystem` is stateless logic that
mutates `Sprite`s/positions given `HookAnimationState`/`FisherAnimationState` structs owned by
`RhythmGame` — it drives hook-throw interpolation, fisher throw animation, and idle sway/bob effects
computed from a running time counter (`updateTiming`), separate from the rhythm clocks above.

**Resource management** — `ResourceManager` is a per-run cache (not a global singleton) keyed by
file path / (font, size, text, color) tuples, owning all `SDL_Texture*`/`Font` lifetimes; call sites
never free textures directly. `GameConfig` *is* a singleton (`GameConfig::getInstance()`) holding all
tunable constants (window, audio, visual, asset paths, gameplay, font sizes) — this is the first
place to check/change when adjusting timing, positions, volume, or asset paths.

**Audio** — `Audio` wraps SDL_mixer music playback. Volume is set from
`GameConfig::AudioConfig::musicVolume` on every `playBackgroundMusic()` call; `toggleMute()` (bound to
M during gameplay) remembers the pre-mute volume via `Mix_VolumeMusic(-1)` (query-only) and restores
it on unmute.

**Testing** — `meowstro_lib` is a static library containing everything except `main()`, so unit tests
link against real game logic without needing an `SDL_Init`'d window. Tests live in `tests/unit/`, one
file per class, using GoogleTest. `AudioLogic` is the one class most gameplay-timing correctness
depends on (hit windows, beat/timestamp conversion, fish interpolation) — its tests are the place to
add coverage when touching timing math.

## Known limitations

- `fishStartXLocations` and `noteBeats` are hand-transcribed, hand-paired arrays tied to one specific
  track (`meowstro_short_ver.mp3`, 147 BPM) — changing the song requires manually re-deriving both.
  There's no beatmap editor/importer.
- The whole UI is laid out in absolute pixel coordinates against a fixed 1920x1080 design resolution;
  logical-size scaling (above) makes this display-independent but the layout itself isn't responsive
  to aspect ratio changes beyond letterboxing.
