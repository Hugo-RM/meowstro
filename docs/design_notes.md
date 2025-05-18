# 🎮 Design Notes for Meowstro

This file documents the key design decisions, architecture, and reasoning behind the development of **Meowstro**, a rhythm-based cat fishing game built with SDL2 and C++.

---

## 🧱 Project Structure

- `main.cpp` - Game loop and Menus
- `Audio.cpp/hpp` - Begins playing background music
- `AudioLogic.cpp/hpp` - Handles beat timing and hit judgment (Perfect/Good/Miss)
- `GameStats.cpp/hpp` - Tracks score, hits, misses, and accuracy
- `Font.cpp/hpp` - Allows for easy creation of strings that can be rendered
- `RenderWindow.cpp/hpp` - Handles SDL window and renderer creation/usage
- `Sprite.cpp/hpp` - Animated sprites with frames, inherits from Entity
- `Entity.cpp/hpp` - Base drawable objects, Mostly static images

---

## 🕹️ Gameplay Flow

- Fish travel left from the right side of the screen at a rate of about 200px per sec
- The player presses **SPACE** to catch fish on beat.
- Hit accuracy is judged as:
  - **Perfect:** within 50 ms
  - **Good:** within 100 ms
  - **Miss:** beyond 100 ms

---

## 🎯 Timing Logic

- All beats are synced to the music using `SDL_GetTicks() - songStartTime`
- `AudioLogic` provides:
  - `msFromMscs()` - helper to convert M:SS:CS to ms
  - `checkHit(expected, actual)` - classifies accuracy level

---

## 📊 Scoring & Accuracy

- `GameStats` tracks score, hits, misses, accuracy
- Accuracy = `hits / (hits + misses) * 100`
- Score is +1000 for Perfect, +500 for Good

---

## 🎨 Rendering

- Fish use `Sprite` class to animate through frames
- Font Textures created with `Font::renderText()` via `Font.hpp` to easily render
- Hook uses linear interpolation for throw and return motion

---

## 🧪 Known Issues

- Fish x-axis locations are hard-coded 
- Probably not usable on other OS
- Accuracy of visual feedback (fish locations) not good
- Notes are hard-coded

---

## 🙌 Credits

- Music by: Jaime, Leonardo, Jay
- Game Art: Hugo
- Game design and rhythm logic: Jaime
- Audio syncing and gameplay mechanics: Jaime
- Game Stats Tracking and logic: Jay
- Font class abstraction and logic: Leonardo 
- Graphics and rendering systems: Hugo
