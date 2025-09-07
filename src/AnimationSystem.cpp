#include "AnimationSystem.hpp"
#include "GameConfig.hpp"

#include <cmath>
#include <algorithm>

AnimationSystem::AnimationSystem() : m_timeCounter(0.0f), m_animationStartTime(0) {
}

void AnimationSystem::initialize() {
    m_timeCounter = 0.0f;
    m_animationStartTime = SDL_GetPerformanceCounter();
}

void AnimationSystem::updateTiming() {
    // Legacy method - calculate from absolute time
    Uint64 currentTime = SDL_GetPerformanceCounter();
    updateTiming(currentTime);
}

void AnimationSystem::updateTiming(Uint64 currentTime) {
    // Use absolute time to avoid accumulating errors
    Uint64 elapsed = currentTime - m_animationStartTime;
    double elapsedSeconds = (double)elapsed / SDL_GetPerformanceFrequency();
    m_timeCounter = (float)elapsedSeconds;
}

void AnimationSystem::startHookThrow(Sprite& hook, int handX, int handY, int throwDuration) {
    // This method will be called to initialize hook throw parameters
    // The actual state management happens in the calling code
}

void AnimationSystem::updateHookAnimation(Sprite& hook, HookAnimationState& state) {
    if (state.isThrowing) {
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - state.throwStartTime;
        
        float progress = static_cast<float>(elapsed) / state.throwDuration;
        if (progress >= 1.0f) {
            progress = 1.0f;
            
            if (!state.isReturning) {
                state.isReturning = true;
                // Reset timer for return journey to avoid teleporting
                state.throwStartTime = SDL_GetTicks();
                // Swap start and target for return journey
                std::swap(state.hookStartX, state.hookTargetX);
                std::swap(state.hookStartY, state.hookTargetY);
                // Recalculate progress for smooth transition
                progress = 0.0f;
            }
            else {
                state.isThrowing = false;
                state.isReturning = false;
                hook.setLoc(state.hookStartX, state.hookStartY); // back to original location
            }
        }
        
        // Always update position during throwing
        if (state.isThrowing) {
            int newX = static_cast<int>(state.hookStartX + (state.hookTargetX - state.hookStartX) * progress);
            int newY = static_cast<int>(state.hookStartY + (state.hookTargetY - state.hookStartY) * progress);
            hook.setLoc(newX, newY);
        }
    }
}

bool AnimationSystem::isHookThrowing(const HookAnimationState& state) const {
    return state.isThrowing;
}

void AnimationSystem::startFisherThrow(FisherAnimationState& state) {
    state.thrown = true;
    state.thrownTimer = 2;
}

void AnimationSystem::updateFisherAnimation(Sprite& fisher, FisherAnimationState& state) {
    // Hand throwing sprite animation
    if (state.thrown) {
        fisher.setFrame(1, 2);
        state.thrownTimer--;
        
        if (state.thrownTimer <= 0) {
            state.thrown = false;
            fisher.setFrame(1, 1);
        }
    }
    else {
        fisher.setFrame(1, 1);
    }
}

void AnimationSystem::updateSwayEffects(std::vector<Sprite>& fish, 
                                       const std::vector<std::pair<int, int>>& fishBasePositions) {
    // Apply sway effects to fish with individual offsets (like the original)
    for (size_t i = 0; i < fish.size() && i < fishBasePositions.size(); ++i) {
        int fishSway = calculateSway(static_cast<float>(i));
        int fishBob = calculateBob(static_cast<float>(i));
        
        // Apply sway relative to base position (not accumulating)
        fish[i].setLoc(fishBasePositions[i].first + fishSway, 
                      fishBasePositions[i].second + fishBob);
    }
}

void AnimationSystem::updateSwayEffects(Sprite& sprite, const std::pair<int, int>& basePosition) {
    int sway = calculateSway();
    int bob = calculateBob();
    
    sprite.setLoc(basePosition.first + sway, basePosition.second + bob);
}

void AnimationSystem::updateHookSway(Sprite& hook, const std::pair<int, int>& basePosition, 
                                    const HookAnimationState& hookState) {
    // Apply sway only when not performing hook throwing animation
    if (!hookState.isThrowing) {
        updateSwayEffects(hook, basePosition);
    }
}

void AnimationSystem::updateFishFrames(std::vector<Sprite>& fish, int numBeats) {
    // Animate fish frames (same as original logic)
    for (int i = 0; i < numBeats && i < static_cast<int>(fish.size()); i++) {
        fish[i]++;
        if (fish[i].getCol() == 4) {
            fish[i].resetFrame();
        }
    }
}

int AnimationSystem::calculateSway(float timeOffset) const {
    return static_cast<int>(sin(m_timeCounter + timeOffset) * 1.1);
}

int AnimationSystem::calculateBob(float timeOffset) const {
    return static_cast<int>(cos(m_timeCounter + timeOffset) * 1.1);
}