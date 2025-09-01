#pragma once

#include "RenderWindow.hpp"
#include "ResourceManager.hpp"
#include "GameStats.hpp"
#include "InputHandler.hpp"
#include "Entity.hpp"
#include "Sprite.hpp"
#include "Audio.hpp"
#include "AudioLogic.hpp"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <SDL.h>


class RhythmGame {
public:
    RhythmGame();
    ~RhythmGame();
    
    // Initialize the game with required dependencies
    void initialize(RenderWindow& window, ResourceManager& resourceManager, GameStats& stats);
    
    // Main game update - returns true if game should continue, false if ended
    bool update(InputAction action, InputHandler& inputHandler);
    
    // Render the game
    void render(RenderWindow& window);
    
    // Check if game is over (music stopped)
    bool isGameOver(bool exitEarly = false) const;
    
    // Clean up resources when exiting gameplay
    void cleanup();

private:
    // Game dependencies
    ResourceManager* m_resourceManager;
    GameStats* m_gameStats;
    
    // Audio system
    Audio m_audioPlayer;
    AudioLogic m_rhythmLogic;
    
    // Game timing
    Uint32 m_songStartTime;
    float m_timeCounter;
    std::vector<bool> m_noteHitFlags;
    
    // Game entities
    Entity m_ocean;
    Entity m_scoreLabel;
    Entity m_scoreNumber;
    Sprite m_fisher;
    Sprite m_boat;
    Sprite m_hook;
    std::vector<Sprite> m_fish;
    
    // Base positions for sway effect (to avoid accumulating position changes)
    std::vector<std::pair<int, int>> m_fishBasePositions;
    std::pair<int, int> m_fisherBasePosition;
    std::pair<int, int> m_boatBasePosition;
    std::pair<int, int> m_hookBasePosition;
    
    // Hit tracking
    std::unordered_set<int> m_fishHits;
    std::unordered_map<int, Uint32> m_fishHitTimes;
    std::unordered_map<int, bool> m_fishHitTypes; // false = Good, true = Perfect
    
    // Animation state
    int m_throwDuration;
    int m_hookTargetX;
    int m_hookTargetY;
    int m_thrownTimer;
    int m_hookStartX;
    int m_hookStartY;
    bool m_isReturning;
    bool m_isThrowing;
    bool m_keydown;
    bool m_thrown;
    Uint32 m_throwStartTime;
    
    // Textures
    SDL_Texture* m_fishTextures[3];
    SDL_Texture* m_perfectHitTexture;
    SDL_Texture* m_goodHitTexture;
    
    // Last score for texture updating
    int m_lastScore;
    
    // Private helper methods
    void initializeTextures();
    void initializeEntities();
    void initializeFish();
    void updateTiming();
    void handleRhythmInput(double currentTime);
    void updateAnimations();
    void updateHookAnimation();
    void updateFisherAnimation();
    void updateSwayEffects();
    void updateFishMovement();
    void checkMissedNotes(double currentTime);
    void renderFish(RenderWindow& window, Uint32 currentTicks);
    void updateScore();
    
    // Format score helper
    std::string formatScore(int score);
};