#pragma once

#include <JSystem/JGeometry/TVec.hpp>
<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
>>>>>>> upstream/main

class SkeletalFishBossFunc {
public:
    static s32 getNearestGuardPosID(const TVec3f&);
    static bool copyGuardPos(TVec3f*, s32);
};