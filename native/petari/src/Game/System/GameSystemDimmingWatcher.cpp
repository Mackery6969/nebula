#include "Game/System/GameSystemDimmingWatcher.hpp"
#include "Game/Util/TriggerChecker.hpp"
<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
#include "Game/System/NebulaFrameClock.hpp"
>>>>>>> upstream/main

GameSystemDimmingWatcher::GameSystemDimmingWatcher() : mChecker(nullptr), _4(true), _5(false) {
    mChecker = new TriggerChecker();
}

void GameSystemDimmingWatcher::update() {
    mChecker->update(_4 || _5);

    if (mChecker->getOnTrigger()) {
<<<<<<< HEAD
        VIEnableDimming(true);
    } else if (mChecker->getOffTrigger()) {
        VIEnableDimming(false);
=======
        NebulaFrameClock::enableDimming(true);
    } else if (mChecker->getOffTrigger()) {
        NebulaFrameClock::enableDimming(false);
>>>>>>> upstream/main
    }
}
