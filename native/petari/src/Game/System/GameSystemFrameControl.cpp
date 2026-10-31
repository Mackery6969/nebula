#include "Game/System/GameSystemFrameControl.hpp"
#include "Game/System/MainLoopFramework.hpp"

GameSystemFrameControl::GameSystemFrameControl() : mIs60fps(true) {
    setMovement60fps();
}

void GameSystemFrameControl::setMovement60fps() {
    mIs60fps = true;

<<<<<<< HEAD
    if (VIGetTvFormat() == VI_PAL) {
        MainLoopFramework::sManager->setTickRateFromFrame(1);
    } else {
        MainLoopFramework* pFramework = MainLoopFramework::sManager;
        pFramework->mRetraceCount = 1;
        pFramework->mTickDuration = 0;
    }
=======
    // The output runs at 60 Hz (never a 50 Hz PAL television).
    MainLoopFramework* pFramework = MainLoopFramework::sManager;
    pFramework->mRetraceCount = 1;
>>>>>>> upstream/main
}
