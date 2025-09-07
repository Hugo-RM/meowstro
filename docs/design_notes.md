# Design Notes for Meowstro

This file documents the key design decisions, architecture, and reasoning behind the development of **Meowstro**, a rhythm-based cat fishing game built with SDL2 and C++.

## Project Architecture

### Core Systems Overview

Meowstro follows a component-based architecture with clear separation of concerns across multiple systems:

**Game State Management**
- `GameStateManager`: Central game loop managing state transitions (Menu -> Playing -> GameOver)
- State-driven architecture ensuring clean transitions between game phases
- Event-driven input handling through SDL2

**Rhythm and Audio System**
- `RhythmGame`: Core gameplay logic managing fish spawning, beat timing, and hit detection
- `AudioLogic`: Timing conversion utilities and hit evaluation with configurable windows
  - Perfect hits: ≤60ms window
  - Good hits: ≤120ms window
- `Audio`: SDL2_mixer integration for music playback

**Entity and Animation System**
- `Entity`: Base class for static drawable objects
- `Sprite`: Extended entity class with frame-based animation support
- `AnimationSystem`: Manages complex animation states (hook throwing, fisher movements)

**Resource Management**
- `ResourceManager`: Singleton pattern for centralized asset loading and caching
- Manages textures, fonts, and audio with automatic cleanup
- Texture validation and cache management for memory efficiency
- Font management with size and style variants

**Input and Configuration**
- `InputHandler`: Key mapping and input processing abstraction
- `GameConfig`: Singleton configuration management with persistent settings
- Clean separation between input detection and game logic

### Technical Implementation Details

**Build System**
- CMake-based cross-platform build system supporting Windows (vcpkg) and Linux/macOS (system packages)
- Dual-target setup: main executable and static library for testing
- Automatic asset copying to build directories
- Google Test integration for unit testing

**Memory Management**
- RAII principles throughout the codebase
- Smart pointer usage in ResourceManager for automatic cleanup
- Exception-based error handling with custom exception types

**Performance Considerations**
- Frame limiting system in RhythmGame for consistent framerates
- Texture caching to minimize SDL2 texture creation overhead

---

## Known Issues

- Fish x-axis locations are hard-coded 
- Accuracy of visual feedback (fish locations) not good
- Notes are hard-coded

## Game Design and Mechanics

### Core Gameplay Loop
1. **Menu Phase**: Player navigates main menu with cat-themed UI
2. **Gameplay Phase**: Rhythm-based fishing with beat synchronization
3. **End Screen Phase**: Score display and accuracy statistics

### Rhythm Mechanics
- **Beat Detection**: Fish spawn and travel left
- **Player Input**: SPACE key for catching fish
- **Timing Windows**: 
  - Perfect: ≤60ms from expected beat (1000 points)
  - Good: ≤120ms from expected beat (500 points)
  - Miss: >120ms or no input (0 points)
- **Accuracy Calculation**: hits / (hits + misses) × 100

### Scoring System
- Score accumulation with perfect/good hit bonuses
- Real-time score display with texture updates

### Visual Design
- Cat-themed assets with ocean fishing setting
- Sprite-based animations for fisher, boat, hook, and fish

### Audio Integration
- Single background track: "meowstro_short_ver.mp3"
- SDL2_mixer for audio

## Asset Structure

### Graphics Assets
- All under `assets/images/`
- `Ocean.png`: Background ocean scene
- `fisher.png`: Main character sprite
- `boat.png`: Fishing boat sprite
- `hook.png`: Fishing hook sprite
- Fish sprites: `blue_fish.png`, `green_fish.png`, `gold_fish.png`
- Menu elements: `menu_cat.png`, `select_cat.png`

### Audio Assets
- Background music: `assets/audio/meowstro_short_ver.mp3`

### Typography
- Font: Comic Sans MS for game text and UI elements

## Testing Framework

### Unit Test Coverage (more incoming)
- `test_Entity.cpp`: Entity positioning and rendering
- `test_Sprite.cpp`: Animation frame management
- `test_GameStats.cpp`: Score calculation and accuracy
- `test_GameConfig.cpp`: Configuration persistence
- `test_ResourceManager.cpp`: Asset loading and caching
- `test_InputHandler.cpp`: Input processing and key mapping
- `test_Logger.cpp`: Logging system functionality
- `test_AssetLoading.cpp`: Asset file validation

### Test Architecture
- Google Test framework integration
- Separate static library (meowstro_lib) for testable game logic
- CTest integration for automated testing

---

## Credits

- Music by: Jaime, Leonardo, Jay
- Game Art: Hugo
- Game design and rhythm logic: Jaime
- Audio syncing and gameplay mechanics: Jaime
- Game Stats Tracking and logic: Jay
- Font class abstraction and logic: Leonardo 
- Graphics and rendering systems: Hugo
