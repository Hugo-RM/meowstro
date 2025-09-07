#include "RhythmGame.hpp"
#include "GameConfig.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <SDL_mixer.h>

RhythmGame::RhythmGame() 
    : m_resourceManager(nullptr)
    , m_gameStats(nullptr)
    , m_songStartTime(0)
    , m_lastFrameTime(0)
    , m_targetFrameTime(0)
    , m_ocean(0, 0, nullptr)
    , m_scoreLabel(0, 0, nullptr)
    , m_scoreNumber(0, 0, nullptr)
    , m_fisher(0, 0, nullptr, 1, 2)
    , m_boat(0, 0, nullptr, 1, 1)
    , m_hook(0, 0, nullptr, 1, 1)
    , m_throwDuration(0)
    , m_hookTargetX(0)
    , m_hookTargetY(0)
    , m_perfectHitTexture(nullptr)
    , m_goodHitTexture(nullptr)
    , m_lastScore(-1)
{
    m_fishTextures[0] = nullptr;
    m_fishTextures[1] = nullptr;
    m_fishTextures[2] = nullptr;
}

RhythmGame::~RhythmGame() {
    m_audioPlayer.stopBackgroundMusic();
}

void RhythmGame::initialize(RenderWindow& window, ResourceManager& resourceManager, GameStats& stats) {
    m_resourceManager = &resourceManager;
    m_gameStats = &stats;
    
    auto& config = GameConfig::getInstance();
    config.initializeBeatTimings();
    
    const auto& gameplayConfig = config.getGameplayConfig();
    
    // Initialize timing
    m_songStartTime = SDL_GetTicks();
    m_noteHitFlags.assign(gameplayConfig.numBeats * 2, false);
    
    // Initialize frame timing (60 FPS target)
    m_targetFrameTime = SDL_GetPerformanceFrequency() / 20;
    m_lastFrameTime = SDL_GetPerformanceCounter();
    
    // Initialize animation system
    m_animationSystem.initialize();
    
    // Clear fish hit tracking data from previous game
    m_fishHits.clear();
    m_fishHitTimes.clear();
    m_fishHitTypes.clear();
    
    // Initialize animation parameters
    m_throwDuration = gameplayConfig.throwDuration;
    m_hookTargetX = gameplayConfig.hookTargetX;
    m_hookTargetY = gameplayConfig.hookTargetY;
    m_hookAnimationState.throwDuration = m_throwDuration;
    
    // Initialize textures and entities
    initializeTextures();
    initializeEntities();
    initializeFish();
    
    // Start music
    const auto& audioConfig = config.getAudioConfig();
    m_audioPlayer.playBackgroundMusic(audioConfig.backgroundMusicPath);
}

void RhythmGame::initializeTextures() {
    const auto& config = GameConfig::getInstance();
    const auto& assetPaths = config.getAssetPaths();
    const auto& fontSizes = config.getFontSizes();
    const auto& visualConfig = config.getVisualConfig();
    
    // Load fish textures
    m_fishTextures[0] = m_resourceManager->loadTexture(assetPaths.blueFishTexture);
    m_fishTextures[1] = m_resourceManager->loadTexture(assetPaths.greenFishTexture);
    m_fishTextures[2] = m_resourceManager->loadTexture(assetPaths.goldFishTexture);
    
    // Load hit feedback textures
    m_perfectHitTexture = m_resourceManager->createTextTexture(assetPaths.fontPath, fontSizes.hitFeedback, "1000", visualConfig.RED);
    m_goodHitTexture = m_resourceManager->createTextTexture(assetPaths.fontPath, fontSizes.hitFeedback, "500", visualConfig.RED);
}

void RhythmGame::initializeEntities() {
    const auto& config = GameConfig::getInstance();
    const auto& assetPaths = config.getAssetPaths();
    const auto& fontSizes = config.getFontSizes();
    const auto& visualConfig = config.getVisualConfig();
    
    // Load textures
    SDL_Texture* oceanTexture = m_resourceManager->loadTexture(assetPaths.oceanTexture);
    SDL_Texture* boatTexture = m_resourceManager->loadTexture(assetPaths.boatTexture);
    SDL_Texture* fisherTexture = m_resourceManager->loadTexture(assetPaths.fisherTexture);
    SDL_Texture* hookTexture = m_resourceManager->loadTexture(assetPaths.hookTexture);
    SDL_Texture* scoreTexture = m_resourceManager->createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "SCORE", visualConfig.BLACK);
    SDL_Texture* numberTexture = m_resourceManager->createTextTexture(assetPaths.fontPath, fontSizes.gameNumbers, "000000", visualConfig.BLACK);
    
    // Initialize entities
    m_ocean = Entity(0, 0, oceanTexture);
    m_scoreLabel = Entity(1720, 100, scoreTexture);
    m_scoreNumber = Entity(1720, 150, numberTexture);
    m_fisher = Sprite(300, 200, fisherTexture, 1, 2);
    m_boat = Sprite(150, 350, boatTexture, 1, 1);
    m_hook = Sprite(430, 215, hookTexture, 1, 1);
    
    // Store base positions for sway effect
    m_fisherBasePosition = {300, 200};
    m_boatBasePosition = {150, 350};
    m_hookBasePosition = {430, 215};
}

void RhythmGame::initializeFish() {
    const auto& config = GameConfig::getInstance();
    const auto& gameplayConfig = config.getGameplayConfig();
    
    m_fish.clear();
    m_fish.reserve(gameplayConfig.numBeats);
    m_fishBasePositions.clear();
    m_fishBasePositions.reserve(gameplayConfig.numBeats);
    
    for (int i = 0; i < gameplayConfig.numBeats; ++i) {
        int baseX = gameplayConfig.fishStartXLocations[i];
        int baseY = 720;
        
        m_fish.emplace_back(Sprite(baseX, baseY, m_fishTextures[rand() % gameplayConfig.numFishTextures], 1, 6));
        m_fishBasePositions.emplace_back(baseX, baseY);
    }
}

bool RhythmGame::update(InputAction action, InputHandler& inputHandler) {
    const auto& config = GameConfig::getInstance();
    const auto& visualConfig = config.getVisualConfig();
    
    // Handle input state
    if (action == InputAction::Quit || action == InputAction::Escape) {
        return false; // Game should end (ESC or window close)
    }
    
    // Update animation timing with current performance counter
    Uint64 currentPerformanceTime = SDL_GetPerformanceCounter();
    m_animationSystem.updateTiming(currentPerformanceTime);
    
    double currentTime = getCurrentGameTimeMs();
    
    // Handle rhythm input - simplified logic
    if (action == InputAction::Select) {
        handleRhythmInput(currentTime);
    }
    
    // Only do these updates when no specific action is being processed
    // (to avoid duplicate work when processing multiple events per frame)
    if (action == InputAction::None) {
        // Check for missed notes
        checkMissedNotes(currentTime);
        
        // Update score display
        updateScore();
        
        // Update animations and fish movement
        updateAnimations();
        updateFishMovement();
        
        // Check if game should end (music stopped)
        if (Mix_PlayingMusic() == 0) {
            return false;
        }
        
        // Maintain consistent frame rate (only when no input events)
        Uint64 currentFrameTime = SDL_GetPerformanceCounter();
        Uint64 frameTime = currentFrameTime - m_lastFrameTime;
        
        if (frameTime < m_targetFrameTime) {
            Uint32 delayMs = (Uint32)((m_targetFrameTime - frameTime) * 1000 / SDL_GetPerformanceFrequency());
            SDL_Delay(delayMs);
        }
        m_lastFrameTime = SDL_GetPerformanceCounter();
    }
    
    return true; // Continue game
}


void RhythmGame::handleRhythmInput(double currentTime) {
    const auto& config = GameConfig::getInstance();
    const auto& gameplayConfig = config.getGameplayConfig();
    const std::vector<double>& noteBeats = gameplayConfig.noteBeats;
    
    // Handle hook throwing
    if (!m_hookAnimationState.isThrowing) {
        // Start fisher animation
        m_animationSystem.startFisherThrow(m_fisherAnimationState);
        
        // Start hook throwing
        m_hookAnimationState.isThrowing = true;
        m_hookAnimationState.isReturning = false;
        m_hookAnimationState.throwStartTime = SDL_GetTicks();
        
        int handX = m_fisher.getX() + 135;
        int handY = m_fisher.getY() + 50;
        m_hookAnimationState.hookStartX = handX;
        m_hookAnimationState.hookStartY = handY;
        m_hookAnimationState.hookTargetX = handX + 300;
        m_hookAnimationState.hookTargetY = handY + 475;
    }
    
    // Check rhythm timing
    for (int i = 0; i < noteBeats.size(); ++i) {
        // Skip notes that have already been hit
        if (m_noteHitFlags[i]) continue;
        
        double expected = noteBeats[i];
        double delta = fabs(currentTime - expected);
        
        if (delta <= m_rhythmLogic.getGOOD()) {
            short int scoreType = m_rhythmLogic.checkHit(expected, currentTime);
            m_noteHitFlags[i] = true;
            m_fishHits.insert(i);
            m_fishHitTimes[i] = SDL_GetTicks();
            
            if (scoreType == 2) { // Perfect
                (*m_gameStats)++;
                m_gameStats->increaseScore(1000);
                m_fishHitTypes[i] = true;
            }
            else if (scoreType == 1) { // Good
                (*m_gameStats)++;
                m_gameStats->increaseScore(500);
                m_fishHitTypes[i] = false;
            }
            break;
        }
    }
}

void RhythmGame::checkMissedNotes(double currentTime) {
    const auto& config = GameConfig::getInstance();
    const auto& gameplayConfig = config.getGameplayConfig();
    const std::vector<double>& noteBeats = gameplayConfig.noteBeats;
    
    for (int i = 0; i < noteBeats.size(); ++i) {
        if (m_noteHitFlags[i]) continue;
        
        double noteTime = noteBeats[i];
        if (currentTime > noteTime + m_rhythmLogic.getGOOD()) {
            (*m_gameStats)--;
            m_noteHitFlags[i] = true;
        }
    }
}

void RhythmGame::updateScore() {
    int currentScore = m_gameStats->getScore();
    if (currentScore != m_lastScore) {
        const auto& config = GameConfig::getInstance();
        const auto& assetPaths = config.getAssetPaths();
        const auto& fontSizes = config.getFontSizes();
        const auto& visualConfig = config.getVisualConfig();
        
        std::string strNum = formatScore(currentScore);
        SDL_Texture* numberTexture = m_resourceManager->createTextTexture(assetPaths.fontPath, fontSizes.gameNumbers, strNum, visualConfig.BLACK);
        m_scoreNumber.setTexture(numberTexture);
        m_lastScore = currentScore;
    }
}

void RhythmGame::updateAnimations() {
    // Update fisher animation
    m_animationSystem.updateFisherAnimation(m_fisher, m_fisherAnimationState);
    
    // Update hook animation
    m_animationSystem.updateHookAnimation(m_hook, m_hookAnimationState);
    
    // Update sway effects
    m_animationSystem.updateHookSway(m_hook, m_hookBasePosition, m_hookAnimationState);
    m_animationSystem.updateSwayEffects(m_boat, m_boatBasePosition);
    m_animationSystem.updateSwayEffects(m_fisher, m_fisherBasePosition);
    m_animationSystem.updateSwayEffects(m_fish, m_fishBasePositions);
}

void RhythmGame::updateFishMovement() {
    const auto& config = GameConfig::getInstance();
    const auto& gameplayConfig = config.getGameplayConfig();
    
    // Move fish (only if not hit)
    for (int i = 0; i < gameplayConfig.numBeats; i++) {
        if (m_fishHits.count(i)) {
            continue; // Skip hit fish
        }
        
        // Move fish left (same as original)
        m_fish[i].moveLeft(10);
        
        // Update base position after movement (for sway effects)
        m_fishBasePositions[i].first = m_fish[i].getX();
        m_fishBasePositions[i].second = m_fish[i].getY();
    }
    
    // Animate fish frames using animation system (for all fish including hit ones)
    // The animation system will handle the frame updates
    for (int i = 0; i < gameplayConfig.numBeats; i++) {
        if (!m_fishHits.count(i)) {
            m_fish[i]++;
            if (m_fish[i].getCol() == 4) {
                m_fish[i].resetFrame();
            }
        }
    }
}




void RhythmGame::render(RenderWindow& window) {
    window.clear();
    
    // Render background
    window.render(m_ocean);
    
    // Render fish with hit feedback
    Uint32 currentTicks = SDL_GetTicks();
    renderFish(window, currentTicks);
    
    // Render game objects
    window.render(m_boat);
    window.render(m_hook);
    window.render(m_fisher);
    window.render(m_scoreLabel);
    window.render(m_scoreNumber);
    
    window.display();
}

void RhythmGame::renderFish(RenderWindow& window, Uint32 currentTicks) {
    const auto& config = GameConfig::getInstance();
    const auto& gameplayConfig = config.getGameplayConfig();
    
    for (int i = 0; i < gameplayConfig.numBeats; i++) {
        if (m_fishHits.count(i)) {
            // Fish was hit - calculate time since hit
            Uint32 timeSinceHit = currentTicks - m_fishHitTimes[i];
            
            if (timeSinceHit < 1000) {
                // Show score text instead of fish for 1 second
                SDL_Texture* scoreTex = m_fishHitTypes[i] ? m_perfectHitTexture : m_goodHitTexture;
                
                SDL_Rect textRect;
                textRect.x = m_fish[i].getX();
                textRect.y = m_fish[i].getY() - 30;
                SDL_QueryTexture(scoreTex, NULL, NULL, &textRect.w, &textRect.h);
                
                SDL_RenderCopy(window.getRenderer(), scoreTex, NULL, &textRect);
            }
            continue; // Skip rendering the fish itself
        }
        
        // Render normal fish (movement happens in updateFishMovement)
        window.render(m_fish[i]);
    }
}

bool RhythmGame::isGameOver(bool exitEarly) const {
    return Mix_PlayingMusic() == 0 || exitEarly;
}

void RhythmGame::cleanup() {
    // Stop background music (like the original gameLoop does)
    m_audioPlayer.stopBackgroundMusic();
}

std::string RhythmGame::formatScore(int score) {
    std::ostringstream ss;
    ss << std::setw(6) << std::setfill('0') << score;
    return ss.str();
}

double RhythmGame::getCurrentGameTimeMs() const {
    // Try to get precise audio position first
    double audioTimeMs = m_audioPlayer.getMusicPositionMs();
    if (audioTimeMs >= 0.0) {
        return audioTimeMs;
    }
    
    // Fallback to SDL_GetTicks timing
    return SDL_GetTicks() - m_songStartTime;
}