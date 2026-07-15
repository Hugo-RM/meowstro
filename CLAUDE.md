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
(`Select`/`MenuUp`/`MenuDown`/`Escape`/`Quit`/`None`) — game logic never touches `SDL_Event`/keycodes
directly. Menu navigation is keyboard-only (no mouse support).

**Rhythm timing model** — this is the part most bugs live in. Two independent clocks exist:
- *Audio/hit-detection clock*: `RhythmGame::getCurrentGameTimeMs()` prefers
  `Mix_GetMusicPosition()` (via `Audio::getMusicPositionMs()`), falling back to
  `SDL_GetTicks() - m_songStartTime` if the mixer can't report position. Note hit/miss timing
  (`handleRhythmInput`, `checkMissedNotes`) is evaluated against this clock and against
  `GameConfig::GameplayConfig::noteBeats` — a hand-transcribed list of song timestamps (built via
  `AudioLogic::msFromMscs(min, sec, centisec)` in `GameConfig::initializeBeatTimings()`).
- *Visual/fish-movement clock*: `RhythmGame::updateFishMovement()` just does `moveLeft(10)` per
  frame update, completely independent of the audio clock or `noteBeats`. Each fish's starting X is
  a hand-tuned pixel value in `GameplayConfig::fishStartXLocations`, manually paired index-for-index
  with `noteBeats` so a constant per-frame speed happens to reach the hook (`hookTargetX`) around
  the right time — this is why fish positions/notes are described as "hard-coded" in
  `docs/design_notes.md`.

  `GameConfig::AudioConfig::travelDuration` and `GameplayConfig::fishTargetX` exist as fields but are
  **not read anywhere in `src/`** — they look like an intended (but unfinished) time-based travel
  model that would derive fish speed/position from `noteBeats` and elapsed time instead of a fixed
  per-frame pixel delta. That gap — frame-rate-dependent movement vs. audio-position-based hit
  windows — is the root cause of the audio/visual sync drift called out in the README's Known
  Issues. Frame pacing itself is also suspect: `RhythmGame::initialize()` sets
  `m_targetFrameTime = SDL_GetPerformanceFrequency() / 20`, i.e. a 20 FPS cap, despite the adjacent
  comment claiming "60 FPS target" — any fix to visual sync should treat that target rate as
  parameterized, not assumed.

**Entity/animation system** — `Entity` is a static drawable (position + `SDL_Texture*`); `Sprite`
extends it with a frame grid (rows/cols) for animation. `AnimationSystem` is stateless logic that
mutates `Sprite`s/positions given `HookAnimationState`/`FisherAnimationState` structs owned by
`RhythmGame` — it drives hook-throw interpolation, fisher throw animation, and idle sway/bob effects
computed from a running time counter (`updateTiming`), separate from the rhythm clocks above.

**Resource management** — `ResourceManager` is a per-run cache (not a global singleton) keyed by
file path / (font, size, text, color) tuples, owning all `SDL_Texture*`/`Font` lifetimes; call sites
never free textures directly. `GameConfig` *is* a singleton (`GameConfig::getInstance()`) holding all
tunable constants (window, audio, visual, asset paths, gameplay, font sizes) — this is the first
place to check/change when adjusting timing, positions, or asset paths.

**Testing** — `meowstro_lib` is a static library containing everything except `main()`, so unit tests
link against real game logic without needing an `SDL_Init`'d window. Tests live in `tests/unit/`,
one file per class, using GoogleTest.

## Known gameplay/visual issues (see also `docs/design_notes.md` and README "Known Issues")

- Audio/visual desync: described above — fish movement is frame-based, hit detection is
  audio-clock-based, and there's no shared time-to-position mapping.
- `fishStartXLocations` and `noteBeats` are both hard-coded, hand-paired arrays tied to one specific
  song (`meowstro_short_ver.mp3`, 147 BPM) — changing the song or BPM requires manually re-deriving
  both.
- `SDL_Delay`-based frame limiting (`RhythmGame::update`) only runs when `action == InputAction::None`,
  so frame pacing is skipped entirely on frames where input was processed.
