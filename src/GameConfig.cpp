#include "GameConfig.hpp"
#include "AudioLogic.hpp"

GameConfig& GameConfig::getInstance()
{
    static GameConfig instance;
    return instance;
}

void GameConfig::initializeBeatTimings()
{
    if (!gameplayConfig.noteBeats.empty())
        return; // Already initialized
    
    AudioLogic audioLogic;
    gameplayConfig.noteBeats = {
        audioLogic.msFromMscs(0,3,46), audioLogic.msFromMscs(0,7,75), audioLogic.msFromMscs(0,9,38), 
        audioLogic.msFromMscs(0,10,61), audioLogic.msFromMscs(0,12,24), audioLogic.msFromMscs(0,13,06),
        audioLogic.msFromMscs(0,13,87), audioLogic.msFromMscs(0,15,30), audioLogic.msFromMscs(0,17,95), 
        audioLogic.msFromMscs(0,20,0), audioLogic.msFromMscs(0,21,22), audioLogic.msFromMscs(0,23,26),
        audioLogic.msFromMscs(0,27,14), audioLogic.msFromMscs(0,28,57), audioLogic.msFromMscs(0,30,40), 
        audioLogic.msFromMscs(0,31,93), audioLogic.msFromMscs(0,32,65), audioLogic.msFromMscs(0,34,69),
        audioLogic.msFromMscs(0,35,91), audioLogic.msFromMscs(0,37,95), audioLogic.msFromMscs(0,41,83), 
        audioLogic.msFromMscs(0,43,26), audioLogic.msFromMscs(0,45,10), audioLogic.msFromMscs(0,46,52),
        audioLogic.msFromMscs(0,48,57)
    };
}