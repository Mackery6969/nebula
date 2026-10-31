#pragma once

<<<<<<< HEAD
#include "Game/Util/BothDirList.hpp"
#include "Game/Util/Color.hpp"
#include <JSystem/JUtility/JUTXfb.hpp>
#include <revolution.h>

typedef void (*PreRenderCallback)();

class MainLoopFrameworkAlarm : public OSAlarm {
public:
    MainLoopFrameworkAlarm() : mLink(this) {
    }

    /* 0x30 */ u32 _30;
    /* 0x34 */ MR::BothDirLink< MainLoopFrameworkAlarm > mLink;

    static MR::BothDirList< MainLoopFrameworkAlarm > sList;
};

=======
#include "Game/Util/Color.hpp"
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>

typedef void (*PreRenderCallback)();

>>>>>>> upstream/main
class MainLoopFramework {
public:
    void setClearColor(Color8 color) {
        mClearColor = color;
    }

<<<<<<< HEAD
    MainLoopFramework(void* pXfb1, void* pXfb2, void* pXfb3, bool useAlpha) : mClearColor(0xffffffff) {
        ctor_subroutine(useAlpha);
        JUTXfb::createManager(pXfb1, pXfb2, pXfb3);
=======
    MainLoopFramework(bool useAlpha) : mClearColor(0xffffffff) {
        ctor_subroutine(useAlpha);
>>>>>>> upstream/main
    }

    ~MainLoopFramework();

    static MainLoopFramework* getManager() {
        return sManager;
    }

<<<<<<< HEAD
    static MainLoopFramework* createManager(const GXRenderModeObj*, void*, void*, void*, bool);
    void ctor_subroutine(bool);
    void prepareCopyDisp();
    void drawendXfb_single();
    void exchangeXfb_double();
    void exchangeXfb_triple();
    void copyXfb_triple();
=======
    static MainLoopFramework* createManager(const GXRenderModeObj*, bool);
    void ctor_subroutine(bool);
    void prepareCopyDisp();
>>>>>>> upstream/main
    void preGX();
    void endGX();
    void waitForRetrace();
    void beginRender();
    void endRender();
    void endFrame();
    void waitBlanking(int);
<<<<<<< HEAD
    void setTickRateFromFrame(u16);
    void clearEfb(GXColor);
    void clearEfb(int, int, int, int, GXColor);
    void calcCombinationRatio();
    static u32 frameToTick(f32);
=======
    void clearEfb(GXColor);
    void clearEfb(int, int, int, int, GXColor);
>>>>>>> upstream/main
    static void setForOSResetSystem();

    /* 0x00 */ Color8 mClearColor;
    /* 0x04 */ u32 mClearZ;
    /* 0x08 */ u16 mDispCopyGamma;
    /* 0x0A */ u16 mCopyClamp;
<<<<<<< HEAD
    u32 _C;
    /* 0x10 */ u16 mRetraceCount;
    /* 0x14 */ u32 mTickDuration;
=======
    /* 0x10 */ u16 mRetraceCount;
>>>>>>> upstream/main
    /* 0x18 */ bool mUseAlpha;
    /* 0x19 */ bool mUseVFilter;
    u8 _1A;
    u8 _1B;
    /* 0x1C */ f32 mCombinationRatio;
<<<<<<< HEAD
    /* 0x20 */ OSTick mTick;
    /* 0x24 */ s32 mLastFrameTime;
    /* 0x28 */ s32 mLastVideoTickDelta;
=======
>>>>>>> upstream/main
    /* 0x2C */ s32 mRenderInterval;
    /* 0x30 */ s32 mRenderCounter;
    /* 0x34 */ u8 mDoRenderFrame;
    u8 _35;
    u8 _36;
    u8 _37;
    /* 0x38 */ PreRenderCallback mPreRenderCallback;
<<<<<<< HEAD
    /* 0x3C */ s16 mSingleBufferIndex;
    u8 _3E;
=======
>>>>>>> upstream/main

    u32 _pad1;
    u32 _pad2;

    static MainLoopFramework* sManager;
};
