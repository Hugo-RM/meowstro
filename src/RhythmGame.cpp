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
    , m_timeCounter(0.0f)
    , m_ocean(0, 0, nullptr)
    , m_scoreLabel(0, 0, nullptr)
    , m_scoreNumber(0, 0, nullptr)
    , m_fisher(0, 0, nullptr, 1, 2)
    , m_boat(0, 0, nullptr, 1, 1)
    , m_hook(0, 0, nullptr, 1, 1)
    , m_throwDuration(0)
    , m_hookTargetX(0)
    , m_hookTargetY(0)
    , m_thrownTimer(2)
    , m_hookStartX(0)
    , m_hookStartY(0)
    , m_isReturning(false)
    , m_isThrowing(false)
    , m_keydown(false)
    , m_thrown(false)
    , m_throwStartTime(0)
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
    m_timeCounter = 0.0f;
    m_noteHitFlags.assign(gameplayConfig.numBeats * 2, false);
    
    // Clear fish hit tracking data from previous game
    m_fishHits.clear();
    m_fishHitTimes.clear();
    m_fishHitTypes.clear();
    
    // Initialize game parameters
    m_throwDuration = gameplayConfig.throwDuration;
    m_hookTargetX = gameplayConfig.hookTargetX;
    m_hookTargetY = gameplayConfig.hookTargetY;
    
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
    
    updateTiming();
    
    double currentTime = SDL_GetTicks() - m_songStartTime;
    
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
        
        // Frame delay (only when no input events)
        SDL_Delay(visualConfig.frameDelay);
    }
    
    return true; // Continue game
}

void RhythmGame::updateTiming() {
    m_timeCounter += 0.05f;
}

void RhythmGame::handleRhythmInput(double currentTime) {
    const auto& config = GameConfig::getInstance();
    const auto& gameplayConfig = config.getGameplayConfig();
    const std::vector<double>& noteBeats = gameplayConfig.noteBeats;
    
    // Handle hook throwing
    if (!m_isThrowing) {
        m_thrown = true;
        m_thrownTimer = 2;
        m_isThrowing = true;
        m_isReturning = false;
        m_throwStartTime = SDL_GetTicks();
        
        int handX = m_fisher.getX() + 135;
        int handY = m_fisher.getY() + 50;
        m_hookStartX = handX;
        m_hookStartY = handY;
        m_hookTargetX = handX + 300;
        m_hookTargetY = handY + 475;
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
    updateSwayEffects();
    updateFisherAnimation();
    updateHookAnimation();
}

void RhythmGame::updateFishMovement() {
    const auto& config = GameConfig::getInstance();
    const auto& gameplayConfig = config.getGameplayConfig();
    
    // Move and animate fish (only if not hit)
    for (int i = 0; i < gameplayConfig.numBeats; i++) {
        if (m_fishHits.count(i)) {
            continue; // Skip hit fish
        }
        
        // Move fish left (same as original)
        m_fish[i].moveLeft(15);
        
        // Update base position after movement (for sway effects)
        m_fishBasePositions[i].first = m_fish[i].getX();
        m_fishBasePositions[i].second = m_fish[i].getY();
        
        // Animate fish frames (same as original)
        m_fish[i]++;
        if (m_fish[i].getCol() == 4) {
            m_fish[i].resetFrame();
        }
    }
}

void RhythmGame::updateSwayEffects() {
    const auto& config = GameConfig::getInstance();
    const auto& gameplayConfig = config.getGameplayConfig();
    
    // Apply sway effects to fish with individual offsets (like the original)
    for (int i = 0; i < gameplayConfig.numBeats; i++) {
        int fishSway = static_cast<int>(sin(m_timeCounter + i) * 1.1);
        int fishBob = static_cast<int>(cos(m_timeCounter + i) * 1.1);
        
        // Apply sway relative to base position (not accumulating)
        m_fish[i].setLoc(m_fishBasePositions[i].first + fishSway, 
                        m_fishBasePositions[i].second + fishBob);
    }
    
    // Calculate common sway and bob for other sprites
    int sway = static_cast<int>(sin(m_timeCounter) * 1.1);
    int bob = static_cast<int>(cos(m_timeCounter) * 1.1);
    
    // Apply sway to other sprites (only when not performing special actions)
    if (!m_isThrowing) {
        m_hook.setLoc(m_hookBasePosition.first + sway, m_hookBasePosition.second + bob);
    }
    m_boat.setLoc(m_boatBasePosition.first + sway, m_boatBasePosition.second + bob);
    m_fisher.setLoc(m_fisherBasePosition.first + sway, m_fisherBasePosition.second + bob);
}

void RhythmGame::updateFisherAnimation() {
    // Hand throwing sprite animation
    if (m_thrown) {
        m_fisher.setFrame(1, 2);
        m_thrownTimer--;
        
        if (m_thrownTimer <= 0) {
            m_thrown = false;
            m_fisher.setFrame(1, 1);
        }
    }
    else {
        m_fisher.setFrame(1, 1);
    }
}

void RhythmGame::updateHookAnimation() {
    // Hook throwing animation
    if (m_isThrowing) {
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - m_throwStartTime;
        
        float progress = static_cast<float>(elapsed) / m_throwDuration;
        if (progress >= 1.0f) {
            progress = 1.0f;
            
            if (!m_isReturning) {
                m_isReturning = true;
                // Swap start and target for return journey
                std::swap(m_hookStartX, m_hookTargetX);
                std::swap(m_hookStartY, m_hookTargetY);
            }
            else {
                m_isThrowing = false;
                m_isReturning = false;
                m_hook.setLoc(m_hookStartX, m_hookStartY);
            }
        }
        
        if (m_isThrowing) {
            int newX = static_cast<int>(m_hookStartX + (m_hookTargetX - m_hookStartX) * progress);
            int newY = static_cast<int>(m_hookStartY + (m_hookTargetY - m_hookStartY) * progress);
            m_hook.setLoc(newX, newY);
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