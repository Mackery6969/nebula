#pragma once

<<<<<<< HEAD
#include <JSystem/JGeometry.hpp>
#include <JSystem/JUtility/JUTTexture.hpp>
#include <revolution.h>
=======
#include "Game/Util/NativeMesh.hpp"
#include <JSystem/JGeometry.hpp>
#include <JSystem/JUtility/JUTTexture.hpp>
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
>>>>>>> upstream/main

class OceanRing;

class OceanRingPartDrawer {
public:
    OceanRingPartDrawer(const OceanRing*, int, int, bool, f32*, f32*, f32*);

    void initDisplayList(f32*, f32*, f32*);
    void draw() const;
<<<<<<< HEAD
    void drawGD(f32*, f32*, f32*) const;
=======
    void drawGD(f32*, f32*, f32*);
>>>>>>> upstream/main
    void drawDynamic() const;
    void drawDynamicBloom() const;

    /* 0x00 */ const OceanRing* mOceanRing;
    /* 0x04 */ TVec3f mPosition;
    /* 0x10 */ int _10;
    /* 0x14 */ int _14;
    /* 0x18 */ bool _18;
    /* 0x19 */ u8 _19;
    /* 0x1A */ u8 _1A;
    /* 0x1B */ u8 _1B;
    /* 0x1C */ f32 _1C;
    /* 0x20 */ f32 _20;
    /* 0x24 */ f32 _24;
<<<<<<< HEAD
    /* 0x28 */ u32 mDispListLength;
    /* 0x2C */ u8* mDispList;
=======
    // The ring's surface as the renderer draws it from far away.
    NativeMesh mMesh;
>>>>>>> upstream/main
};

class OceanRingDrawer {
public:
    OceanRingDrawer(const OceanRing*);

    void update();
    void draw() const;
    void drawBloom() const;
    void initParts();
    void initDisplayList();
<<<<<<< HEAD
    void drawGD() const;
=======
    void drawGD();
>>>>>>> upstream/main
    void loadMaterial() const;
    void loadMaterialBloom() const;

    OceanRingPartDrawer* getDrawer(int idx) const {
        return mPartDrawers[idx];
    }

    inline f32 someInline(f32 a1, f32 a2, f32 a3) const {
        return (a1 - a2) / a3;
    }

    /* 0x00 */ const OceanRing* mRing;
    /* 0x04 */ s32 mDrawerCount;
    /* 0x08 */ OceanRingPartDrawer** mPartDrawers;
    /* 0x0C */ f32 _C;
    /* 0x10 */ f32 _10;
    /* 0x14 */ f32 _14;
    /* 0x18 */ f32 _18;
    /* 0x1C */ f32 _1C;
    /* 0x20 */ f32 _20;
    /* 0x24 */ JUTTexture* mWaterTex;
    /* 0x28 */ JUTTexture* mWaterIndTex;
<<<<<<< HEAD
    /* 0x2C */ u32 mDispListLength;
    /* 0x30 */ u8* mDispList;
=======
    // The ring's surface as the renderer draws it from far away.
    NativeMesh mMesh;
>>>>>>> upstream/main
};
