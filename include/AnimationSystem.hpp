#pragma once

#include "Sprite.hpp"
#include "Entity.hpp"

#include <SDL.h>
#include <vector>
#include <utility>

// Animation state for hook throwing
struct HookAnimationState {
    bool isThrowing;
    bool isReturning;
    Uint32 throwStartTime;
    int throwDuration;
    int hookStartX;
    int hookStartY;
    int hookTargetX;
    int hookTargetY;
    
    HookAnimationState() 
        : isThrowing(false), isReturning(false), throwStartTime(0)
        , throwDuration(0), hookStartX(0), hookStartY(0)
        , hookTargetX(0), hookTargetY(0) {}
};

// Animation state for fisher
struct FisherAnimationState {
    bool thrown;
    int thrownTimer;
    
    FisherAnimationState() : thrown(false), thrownTimer(2) {}
};

class AnimationSystem {
public:
    AnimationSystem();
    ~AnimationSystem() = default;
    
    // Initialize animation system with timing
    void initialize();
    
    // Update animation timing (call once per frame)
    void updateTiming();
    void updateTiming(Uint64 currentTime);
    
    // Hook throwing animation
    void startHookThrow(Sprite& hook, int handX, int handY, int throwDuration);
    void updateHookAnimation(Sprite& hook, HookAnimationState& state);
    bool isHookThrowing(const HookAnimationState& state) const;
    
    // Fisher animation
    void startFisherThrow(FisherAnimationState& state);
    void updateFisherAnimation(Sprite& fisher, FisherAnimationState& state);
    
    // Sway effects for sprites
    void updateSwayEffects(std::vector<Sprite>& fish, 
                          const std::vector<std::pair<int, int>>& fishBasePositions);
    void updateSwayEffects(Sprite& sprite, const std::pair<int, int>& basePosition);
    
    // Specialized sway update for hook (only when not throwing)
    void updateHookSway(Sprite& hook, const std::pair<int, int>& basePosition, 
                       const HookAnimationState& hookState);
    
    // Fish animation frames
    void updateFishFrames(std::vector<Sprite>& fish, int numBeats);
    
    // Get current time counter for external use if needed
    float getTimeCounter() const { return m_timeCounter; }
    
private:
    float m_timeCounter;
    Uint64 m_animationStartTime;  // For absolute time calculations
    
    // Helper methods for sway calculations
    int calculateSway(float timeOffset = 0.0f) const;
    int calculateBob(float timeOffset = 0.0f) const;
};