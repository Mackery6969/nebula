#include "Game/Map/OceanRingDrawer.hpp"
<<<<<<< HEAD
=======
#include <JSystem/J3DGraphBase/J3DNativeState.hpp>
#include <JSystem/J3DGraphBase/J3DNativeShape.hpp>
#include <JSystem/J3DGraphBase/J3DNativeDraw.hpp>
>>>>>>> upstream/main
#include "Game/Map/OceanRing.hpp"
#include "Game/Map/WaterAreaHolder.hpp"
#include "Game/Util/CameraUtil.hpp"
#include "Game/Util/Color.hpp"
#include "Game/Util/DrawUtil.hpp"
#include "Game/Util/MathUtil.hpp"
#include "Game/Util/ObjUtil.hpp"
#include "Game/Util/PlayerUtil.hpp"
#include "Game/Util/RailUtil.hpp"
#include "Game/Util/SchedulerUtil.hpp"
#include "Game/Util/ScreenUtil.hpp"
#include <JSystem/JKernel/JKRHeap.hpp>
<<<<<<< HEAD
#include <revolution/gd/GDBase.h>
#include <revolution/gx/GXVert.h>
=======
>>>>>>> upstream/main

namespace {
    const f32 sPointNumInPart = 0.0f;
    const f32 sDistancePartDL = 13000.0f;
    const f32 sFlowSpeedTexRate = 0.0f;
    const f32 sFlowSpeedTexRateMin = 0.0f;
    const f32 sTexRate0 = 0.0f;
    const f32 sTexRate1 = 0.0f;
    const f32 sTexRate2 = 0.0f;
    const f32 sTexSpeed0U = -0.003f;
    const f32 sTexSpeed0V = -0.001f;
    const f32 sTexSpeed1U = -0.002f;
    const f32 sTexSpeed1V = 0.001f;
    const f32 sTexSpeed2V = 0.003f;
    const f32 sIndirectScale = 0.1f;
    const f32 sBloomCameraOffsetZ = 3000.0f;
    const f32 sDistancePartDrawBloom = 4000.0f;
    const f32 sBloomCameraUpMin = 0.0f;
    const f32 sBloomCameraUpMax = 0.0f;
    const f32 sBloomCameraDepthMin = 0.0f;
    const f32 sBloomCameraDepthMax = 0.0f;

    static GXColor color1 = {0x28, 0x28, 0x28, 0x14};
    static GXColor color2 = {0x76, 0xD7, 0xFF, 0xFF};
    static u8 unknownVal = 1;
};  // namespace

OceanRingPartDrawer::OceanRingPartDrawer(const OceanRing* pRing, int a3, int a4, bool a5, f32* pTexCoordU0, f32* pTexCoordU1, f32* pTexCoordU2)
    : mPosition(0.0f, 0.0f, 0.0f) {
    mOceanRing = pRing;
    _10 = a3;
    _14 = a4;
    _18 = a5;
    _1C = *pTexCoordU0;
    _20 = *pTexCoordU1;
    _24 = *pTexCoordU2;
<<<<<<< HEAD
    mDispListLength = 0;
    mDispList = nullptr;

=======
>>>>>>> upstream/main
    initDisplayList(pTexCoordU0, pTexCoordU1, pTexCoordU2);

    for (int i = 0; i < a4; i++) {
        int v12 = a3 + i;

        if (i == a4 - 1 && _18) {
            v12 = 0;
        }

        mPosition.add(((mOceanRing->mWaterPoints[(v12 * mOceanRing->mStride - 1) + mOceanRing->mStride / 2])->mOrigPos));
    }

    mPosition.scale(1.0f / a4);
}

void OceanRingPartDrawer::initDisplayList(f32* pTexCoordU0, f32* pTexCoordU1, f32* pTexCoordU2) {
<<<<<<< HEAD
    MR::ProhibitSchedulerAndInterrupts prohibit(false);

    u32 x = _14 * 0x50;
    u32 size = (((x * mOceanRing->mStride + 3) >> 5) + 2) << 5;
    mDispList = new (0x20) u8[size];
    DCInvalidateRange(mDispList, size);
    GDLObj obj;
    GDInitGDLObj(&obj, mDispList, size);
    __GDCurrentDL = &obj;
    drawGD(pTexCoordU0, pTexCoordU1, pTexCoordU2);
    GDPadCurr32();
    mDispListLength = obj.ptr - obj.start;
    DCStoreRange(mDispList, size);
=======
    mMesh.init(NEBULA_SHAPE_COLOR0 | NEBULA_SHAPE_TEXCOORD0 | NEBULA_SHAPE_TEXCOORD0 << 1 | NEBULA_SHAPE_TEXCOORD0 << 2, 2 * mOceanRing->mStride * (_14 - 1));
    drawGD(pTexCoordU0, pTexCoordU1, pTexCoordU2);
    mMesh.send();
>>>>>>> upstream/main
}

void OceanRingPartDrawer::draw() const {
    if (mPosition.distance(*MR::getPlayerPos()) < ::sDistancePartDL) {
        drawDynamic();
    } else {
<<<<<<< HEAD
        GXCallDisplayList(mDispList, mDispListLength);
    }
}

void OceanRingPartDrawer::drawGD(f32* pTexCoordU0, f32* pTexCoordU1, f32* pTexCoordU2) const {
=======
        mMesh.draw();
    }
}

void OceanRingPartDrawer::drawGD(f32* pTexCoordU0, f32* pTexCoordU1, f32* pTexCoordU2) {
>>>>>>> upstream/main
    f32 f30 = 0.05f;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    f32 f23;
    f32 f22;
    f32 f21 = 1.0f;

    f28 = f30 * f21;
    f29 = 0.1f * f21;
    f27 = _1C;
    f25 = _20;
    f23 = _24;
    f26 = _1C;
    f24 = _20;
    f22 = _24;
    for (s32 i = 0; i < _14 - 1; i++) {
        s32 index = _10 + i;
        s32 indexPlusOne = index + 1;
        if (i == _14 - 2 && _18) {
            indexPlusOne = 0;
        }

        mOceanRing->getPoint(0, index);
        mOceanRing->getPoint(0, indexPlusOne);

        f26 += f28;
        f24 += f28;
        f22 += f29;
        f32 f20 = 0.0f;
        f32 f19 = 0.0f;
        f32 f18 = 0.0f;
        f32 f17 = 0.0f;
        f32 f16 = 0.0f;
        f32 f15 = 0.0f;
<<<<<<< HEAD
        GDBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, (u16)mOceanRing->mStride << 1);

        for (s32 j = 0; j < mOceanRing->mStride; j++) {
            WaterPoint* pPoint = mOceanRing->getPoint(j, index);
            GDPosition3f32(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            GDColor4u8(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
            GDWrite_f32(f27);
            GDWrite_f32(f17);
            GDWrite_f32(f25);
            GDWrite_f32(f16);
            GDWrite_f32(f23);
            GDWrite_f32(f15);

            pPoint = mOceanRing->getPoint(j, indexPlusOne);
            GDPosition3f32(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            GDColor4u8(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
            GDWrite_f32(f26);
            GDWrite_f32(f20);
            GDWrite_f32(f24);
            GDWrite_f32(f19);
            GDWrite_f32(f22);
            GDWrite_f32(f18);
=======
        mMesh.begin(GX_TRIANGLESTRIP, (u16)mOceanRing->mStride << 1);

        for (s32 j = 0; j < mOceanRing->mStride; j++) {
            WaterPoint* pPoint = mOceanRing->getPoint(j, index);
            mMesh.position(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            mMesh.color(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
            mMesh.texCoord(0, f27, f17);
            mMesh.texCoord(1, f25, f16);
            mMesh.texCoord(2, f23, f15);
            mMesh.corner();

            pPoint = mOceanRing->getPoint(j, indexPlusOne);
            mMesh.position(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            mMesh.color(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
            mMesh.texCoord(0, f26, f20);
            mMesh.texCoord(1, f24, f19);
            mMesh.texCoord(2, f22, f18);
            mMesh.corner();
>>>>>>> upstream/main

            f17 += f28;
            f16 += f28;
            f15 += f29;
            f20 += f28;
            f19 += f28;
            f18 += f29;
        }

        f27 = f26;
        f25 = f24;
        f23 = f22;
        if (i != _14 - 1) {
            *pTexCoordU0 += 0.05f * f21;
            *pTexCoordU1 += 0.05f * f21;
            *pTexCoordU2 += 0.1f * f21;
        }
    }
}

void OceanRingPartDrawer::drawDynamic() const {
    f32 f30 = 0.05f;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    f32 f23;
    f32 f22;
    f32 f21 = 1.0f;

    f28 = f30 * f21;
    f29 = 0.1f * f21;
    f27 = _1C;
    f25 = _20;
    f23 = _24;
    f26 = _1C;
    f24 = _20;
    f22 = _24;
    for (s32 i = 0; i < _14 - 1; i++) {
        s32 index = _10 + i;
        s32 indexPlusOne = index + 1;
        if (i == _14 - 2 && _18) {
            indexPlusOne = 0;
        }

        mOceanRing->getPoint(0, index);
        mOceanRing->getPoint(0, indexPlusOne);

        f26 += f28;
        f24 += f28;
        f22 += f29;
        f32 f20 = 0.0f;
        f32 f19 = 0.0f;
        f32 f18 = 0.0f;
        f32 f17 = 0.0f;
        f32 f16 = 0.0f;
        f32 f15 = 0.0f;

<<<<<<< HEAD
        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, mOceanRing->mStride * 2);
        for (s32 j = 0; j < mOceanRing->mStride; j++) {
            WaterPoint* pPoint = mOceanRing->getPoint(j, index);
            GXPosition3f32(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            GXColor4u8(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
            GXTexCoord2f32(f27, f17);
            GXTexCoord2f32(f25, f16);
            GXTexCoord2f32(f23, f15);

            pPoint = mOceanRing->getPoint(j, indexPlusOne);
            GXPosition3f32(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            GXColor4u8(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
            GXTexCoord2f32(f26, f20);
            GXTexCoord2f32(f24, f19);
            GXTexCoord2f32(f22, f18);
=======
        J3DNativeDraw::begin(GX_TRIANGLESTRIP, mOceanRing->mStride * 2, NEBULA_SHAPE_COLOR0 | NEBULA_SHAPE_TEXCOORD0 | NEBULA_SHAPE_TEXCOORD0 << 1 | NEBULA_SHAPE_TEXCOORD0 << 2);
        for (s32 j = 0; j < mOceanRing->mStride; j++) {
            WaterPoint* pPoint = mOceanRing->getPoint(j, index);
            J3DNativeDraw::position(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            J3DNativeDraw::color(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
            J3DNativeDraw::texCoord(f27, f17);
            J3DNativeDraw::texCoord(f25, f16);
            J3DNativeDraw::texCoord(f23, f15);

            pPoint = mOceanRing->getPoint(j, indexPlusOne);
            J3DNativeDraw::position(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            J3DNativeDraw::color(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
            J3DNativeDraw::texCoord(f26, f20);
            J3DNativeDraw::texCoord(f24, f19);
            J3DNativeDraw::texCoord(f22, f18);
>>>>>>> upstream/main

            f17 += f28;
            f16 += f28;
            f15 += f29;
            f20 += f28;
            f19 += f28;
            f18 += f29;
        }

<<<<<<< HEAD
        GXEnd();
=======
>>>>>>> upstream/main

        f27 = f26;
        f25 = f24;
        f23 = f22;
    }
}

void OceanRingPartDrawer::drawDynamicBloom() const {
    f32 f30 = 0.05f;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    f32 f21 = 1.0f;

    f28 = f30 * f21;
    f29 = 0.1f * f21;
    f27 = _1C;
    f25 = _20;
    f26 = _1C;
    f24 = _20;
    for (s32 i = 0; i < _14 - 1; i++) {
        s32 index = _10 + i;
        s32 indexPlusOne = index + 1;
        if (i == _14 - 2 && _18) {
            indexPlusOne = 0;
        }

        mOceanRing->getPoint(0, index);
        mOceanRing->getPoint(0, indexPlusOne);

        f26 += f28;
        f24 += f28;
        f32 f20 = 0.0f;
        f32 f19 = 0.0f;
        f32 f17 = 0.0f;
        f32 f16 = 0.0f;

<<<<<<< HEAD
        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, mOceanRing->mStride * 2);
        for (s32 j = 0; j < mOceanRing->mStride; j++) {
            WaterPoint* pPoint = mOceanRing->getPoint(j, index);
            WaterPoint* pPoint2 = mOceanRing->getPoint(j, indexPlusOne);
            GXPosition3f32(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            GXTexCoord2f32(f27, f17);
            GXTexCoord2f32(f25, f16);

            GXPosition3f32(pPoint2->mPosition.x, pPoint2->mPosition.y, pPoint2->mPosition.z);
            GXTexCoord2f32(f26, f20);
            GXTexCoord2f32(f24, f19);
=======
        J3DNativeDraw::begin(GX_TRIANGLESTRIP, mOceanRing->mStride * 2, NEBULA_SHAPE_TEXCOORD0 | NEBULA_SHAPE_TEXCOORD0 << 1);
        for (s32 j = 0; j < mOceanRing->mStride; j++) {
            WaterPoint* pPoint = mOceanRing->getPoint(j, index);
            WaterPoint* pPoint2 = mOceanRing->getPoint(j, indexPlusOne);
            J3DNativeDraw::position(pPoint->mPosition.x, pPoint->mPosition.y, pPoint->mPosition.z);
            J3DNativeDraw::texCoord(f27, f17);
            J3DNativeDraw::texCoord(f25, f16);

            J3DNativeDraw::position(pPoint2->mPosition.x, pPoint2->mPosition.y, pPoint2->mPosition.z);
            J3DNativeDraw::texCoord(f26, f20);
            J3DNativeDraw::texCoord(f24, f19);
>>>>>>> upstream/main

            f17 += f28;
            f16 += f28;
            f20 += f28;
            f19 += f28;
        }

<<<<<<< HEAD
        GXEnd();
=======
>>>>>>> upstream/main

        f27 = f26;
        f25 = f24;
    }
}

OceanRingDrawer::OceanRingDrawer(const OceanRing* pOceanRing) {
    mRing = pOceanRing;
    mDrawerCount = 0;
    mPartDrawers = nullptr;
    _C = 0.0f;
    _10 = 0.0f;
    _14 = 0.0f;
    _18 = 0.0f;
    _1C = 0.0f;
    _20 = 0.0f;
    mWaterTex = nullptr;
    mWaterIndTex = nullptr;
<<<<<<< HEAD
    mDispListLength = 0;
    mDispList = nullptr;
=======
>>>>>>> upstream/main

    mWaterTex = new JUTTexture(MR::loadTexFromArc("WaterWave.arc", "Water.bti"), 0);
    mWaterIndTex = new JUTTexture(MR::loadTexFromArc("WaterWave.arc", "WaterIndirect.bti"), 0);
    initParts();
    initDisplayList();
}

void OceanRingDrawer::update() {
    _C = MR::repeat(_C + ::sTexSpeed0U, 0.0f, 1.0f);
    _10 = MR::repeat(_10 + ::sTexSpeed0V, 0.0f, 1.0f);
    _14 = MR::repeat(_14 + ::sTexSpeed1U, 0.0f, 1.0f);
    _18 = MR::repeat(_18 + ::sTexSpeed1V, 0.0f, 1.0f);
    _1C = MR::repeat(_1C + 0.0f, 0.0f, 1.0f);
    _20 = MR::repeat(_20 + ::sTexSpeed2V, 0.0f, 1.0f);
}

void OceanRingDrawer::draw() const {
    loadMaterial();

    if (mRing->mIsClipped) {
<<<<<<< HEAD
        GXCallDisplayList(mDispList, mDispListLength);
=======
        mMesh.draw();
>>>>>>> upstream/main
    } else {
        for (s32 i = 0; i < mDrawerCount; i++) {
            getDrawer(i)->draw();
        }
    }
}

void OceanRingDrawer::drawBloom() const {
    loadMaterialBloom();
    TVec3f zDir = MR::getCamZdir();
    zDir.scale(::sBloomCameraOffsetZ);
    TVec3f camPos = MR::getCamPos();
    zDir.add(camPos);

    if (!mRing->mIsClipped) {
        for (s32 i = 0; i < mDrawerCount; i++) {
            OceanRingPartDrawer* drwr = getDrawer(i);

            if (drwr->mPosition.distance(zDir) < ::sDistancePartDrawBloom) {
                drwr->drawDynamicBloom();
            }
        }
    }
}

void OceanRingDrawer::initParts() {
    mDrawerCount = (mRing->mSegCount / 10) + 1;
    s32 count2 = mRing->mSegCount / mDrawerCount;
    f32 a = 0.0f, b = 0.0f, c = 0.0f;
    mPartDrawers = new OceanRingPartDrawer*[mDrawerCount];
    for (s32 i = 0; i < mDrawerCount - 1; i++) {
        mPartDrawers[i] = new OceanRingPartDrawer(mRing, count2 * i, count2 + 1, false, &a, &b, &c);
    }

    s32 v;
    bool flag;
    s32 w;

    s32 last = mDrawerCount - 1;
    w = count2 * last;
    v = mRing->mSegCount - w;
    flag = false;

    if (MR::isLoopRail(mRing)) {
        flag = true;
        v += 1;
    }

    mPartDrawers[last] = new OceanRingPartDrawer(mRing, w, v, flag, &a, &b, &c);
}

void OceanRingDrawer::initDisplayList() {
<<<<<<< HEAD
    MR::ProhibitSchedulerAndInterrupts prohibit(false);

=======
>>>>>>> upstream/main
    s32 flag = 0;

    if (MR::isLoopRail(mRing)) {
        flag = 1;
    }

<<<<<<< HEAD
    u32 x = 0x50 * (flag + mRing->mSegCount);
    u32 length = (((x + 3) >> 5) + 2) << 5;
    mDispList = new (0x20) u8[length];
    DCInvalidateRange(mDispList, length);
    GDLObj obj;
    GDInitGDLObj(&obj, mDispList, length);
    __GDCurrentDL = &obj;
    drawGD();
    GDPadCurr32();
    mDispListLength = obj.ptr - obj.start;
    DCStoreRange(mDispList, length);
}

void OceanRingDrawer::drawGD() const {
=======
    mMesh.init(NEBULA_SHAPE_COLOR0 | NEBULA_SHAPE_TEXCOORD0 | NEBULA_SHAPE_TEXCOORD0 << 1 | NEBULA_SHAPE_TEXCOORD0 << 2, 2 * (flag + mRing->mSegCount));
    drawGD();
    mMesh.send();
}

void OceanRingDrawer::drawGD() {
>>>>>>> upstream/main
    s32 flag = 0;

    if (MR::isLoopRail(mRing)) {
        flag = 1;
    }

    f32 scalex;
    f32 U0 = 0.0f;
    f32 U1 = 0.0f;
    f32 U2 = 0.0f;
    f32 V01 = 0.0f;
    f32 V11 = 0.0f;
    f32 V21 = 0.0f;
    s32 m = mRing->mStride;
    m -= 1;
    f32 V00 = 0.05f * m;
    f32 V10 = 0.05f * m;
    f32 V20 = 0.1f * m;

<<<<<<< HEAD
    GDBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, (flag + mRing->mSegCount) * 2);
=======
    mMesh.begin(GX_TRIANGLESTRIP, (flag + mRing->mSegCount) * 2);
>>>>>>> upstream/main

    scalex = 1.0f;
    f32 V0 = V00 * scalex;
    f32 V1 = V10 * scalex;
    f32 V2 = V20 * scalex;

    WaterPoint* pPoint;
    WaterPoint* pPoint2;
    for (u32 i = 0; i < flag + mRing->mSegCount; i++) {
        s32 index = i;
        if (index == mRing->mSegCount) {
            index = 0;
        }

        s32 ddd = index * mRing->mStride;
        pPoint = mRing->mWaterPoints[mRing->mStride + ddd - 1];
        pPoint2 = mRing->mWaterPoints[ddd];
<<<<<<< HEAD
        GDPosition3f32(pPoint->mOrigPos.x, pPoint->mOrigPos.y, pPoint->mOrigPos.z);
        GDColor4u8(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
        GDWrite_f32(U0);
        GDWrite_f32(V0);
        GDWrite_f32(U1);
        GDWrite_f32(V1);
        GDWrite_f32(U2);
        GDWrite_f32(V2);

        GDPosition3f32(pPoint2->mOrigPos.x, pPoint2->mOrigPos.y, pPoint2->mOrigPos.z);
        GDColor4u8(0xFF, 0xFF, 0xFF, pPoint2->mAlpha);
        GDWrite_f32(U0);
        GDWrite_f32(V01);
        GDWrite_f32(U1);
        GDWrite_f32(V11);
        GDWrite_f32(U2);
        GDWrite_f32(V21);
=======
        mMesh.position(pPoint->mOrigPos.x, pPoint->mOrigPos.y, pPoint->mOrigPos.z);
        mMesh.color(0xFF, 0xFF, 0xFF, pPoint->mAlpha);
        mMesh.texCoord(0, U0, V0);
        mMesh.texCoord(1, U1, V1);
        mMesh.texCoord(2, U2, V2);
        mMesh.corner();

        mMesh.position(pPoint2->mOrigPos.x, pPoint2->mOrigPos.y, pPoint2->mOrigPos.z);
        mMesh.color(0xFF, 0xFF, 0xFF, pPoint2->mAlpha);
        mMesh.texCoord(0, U0, V01);
        mMesh.texCoord(1, U1, V11);
        mMesh.texCoord(2, U2, V21);
        mMesh.corner();
>>>>>>> upstream/main

        U0 += (0.05f * scalex);
        U1 += (0.05f * scalex);
        U2 += (0.1f * scalex);
    }
}

void OceanRingDrawer::loadMaterial() const {
<<<<<<< HEAD
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_POS_XYZ, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
    GXLoadPosMtxImm(MR::getCameraViewMtx(), 0);
    GXSetCurrentMtx(0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(4);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, 0x21, 0, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, 0x24, 0, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD3, GX_TG_MTX3x4, GX_TG_POS, 0x27, 0, 0x7D);
=======
    J3DNativeShape::sendPosition(MR::getCameraViewMtx(), 0);
    J3DNativeState::setCurrentMtx(0);
    J3DNativeState::setNumChans(1);
    J3DNativeState::setChanCtrl(GX_COLOR0A0, 0, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    J3DNativeState::setNumTexGens(4);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, 0, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, 0x21, 0, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, 0x24, 0, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD3, GX_TG_MTX3x4, GX_TG_POS, 0x27, 0, 0x7D);
>>>>>>> upstream/main

    TMtx34f mtx;
    mtx.identity();
    mtx.mMtx[0][2] = _C;
    mtx.mMtx[1][2] = _10;
<<<<<<< HEAD
    GXLoadTexMtxImm(mtx.toMtxPtr(), 0x1E, GX_MTX2x4);
    mtx.mMtx[0][2] = _14;
    mtx.mMtx[1][2] = _18;
    GXLoadTexMtxImm(mtx.toMtxPtr(), 0x21, GX_MTX2x4);
    mtx.mMtx[0][2] = _1C;
    mtx.mMtx[1][2] = _20;
    GXLoadTexMtxImm(mtx.toMtxPtr(), 0x24, GX_MTX2x4);
=======
    J3DNativeShape::sendTexture(mtx.toMtxPtr(), 0x1E, GX_MTX2x4);
    mtx.mMtx[0][2] = _14;
    mtx.mMtx[1][2] = _18;
    J3DNativeShape::sendTexture(mtx.toMtxPtr(), 0x21, GX_MTX2x4);
    mtx.mMtx[0][2] = _1C;
    mtx.mMtx[1][2] = _20;
    J3DNativeShape::sendTexture(mtx.toMtxPtr(), 0x24, GX_MTX2x4);
>>>>>>> upstream/main
    MR::loadTexProjectionMtx(39);

    mWaterTex->load(GX_TEXMAP0);
    JUTTexture tex(MR::getScreenResTIMG(), 0);
    tex.load(GX_TEXMAP1);
    mWaterIndTex->load(GX_TEXMAP2);

    if (::unknownVal != 0 && MR::isCameraInWater()) {
<<<<<<< HEAD
        GXSetNumIndStages(0);
        GXSetTevDirect(GX_TEVSTAGE0);
        GXSetTevDirect(GX_TEVSTAGE1);
        GXSetTevDirect(GX_TEVSTAGE2);
        GXSetTevDirect(GX_TEVSTAGE3);
    } else {
        GXSetNumIndStages(1);
        GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD2, GX_TEXMAP2);
        GXSetTevIndWarp(GX_TEVSTAGE3, GX_INDTEXSTAGE0, 1, 0, GX_ITM_0);
=======
        J3DNativeState::setNumIndStages(0);
        J3DNativeState::setTevDirect(GX_TEVSTAGE0);
        J3DNativeState::setTevDirect(GX_TEVSTAGE1);
        J3DNativeState::setTevDirect(GX_TEVSTAGE2);
        J3DNativeState::setTevDirect(GX_TEVSTAGE3);
    } else {
        J3DNativeState::setNumIndStages(1);
        J3DNativeState::setIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD2, GX_TEXMAP2);
        J3DNativeState::setTevIndWarp(GX_TEVSTAGE3, GX_INDTEXSTAGE0, 1, 0, GX_ITM_0);
>>>>>>> upstream/main

        Mtx23 new_mtx;
        new_mtx[0][0] = ::sIndirectScale;
        new_mtx[0][1] = 0.0f;
        new_mtx[0][2] = 0.0f;
        new_mtx[1][0] = 0.0f;
        new_mtx[1][1] = ::sIndirectScale;
        new_mtx[1][2] = 0.0f;

<<<<<<< HEAD
        GXSetIndTexMtx(GX_ITM_0, new_mtx, 0);
    }

    GXSetNumTevStages(4);
    GXSetTevColor(GX_TEVREG0, (GXColor&)::color1);
    GXSetTevColor(GX_TEVREG1, (GXColor&)::color2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, 0, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, 0, GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_A0, GX_CC_C0, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_COMP_R8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_KONST, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP1, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_GREATER, 0);
    GXSetZMode(1, GX_LEQUAL, 0);
    GXSetZCompLoc(1);
    GXSetCullMode(GX_CULL_NONE);
    GXSetClipMode(GX_CLIP_ENABLE);
}

void OceanRingDrawer::loadMaterialBloom() const {
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
    GXLoadPosMtxImm(MR::getCameraViewMtx(), 0);
    GXSetCurrentMtx(0);
    GXSetNumChans(0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, 0x21, 0, 0x7D);
=======
        J3DNativeState::setIndTexMtx(GX_ITM_0, new_mtx, 0);
    }

    J3DNativeState::setNumTevStages(4);
    J3DNativeState::setTevColor(GX_TEVREG0, (GXColor&)::color1);
    J3DNativeState::setTevColor(GX_TEVREG1, (GXColor&)::color2);
    J3DNativeState::setTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    J3DNativeState::setTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    J3DNativeState::setTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    J3DNativeState::setTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    J3DNativeState::setTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    J3DNativeState::setTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
    J3DNativeState::setTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
    J3DNativeState::setTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, 0, GX_TEVPREV);
    J3DNativeState::setTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    J3DNativeState::setTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, 0, GX_TEVPREV);
    J3DNativeState::setTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_A0, GX_CC_C0, GX_CC_CPREV);
    J3DNativeState::setTevColorOp(GX_TEVSTAGE2, GX_TEV_COMP_R8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    J3DNativeState::setTevAlphaIn(GX_TEVSTAGE2, GX_CA_KONST, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    J3DNativeState::setTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    J3DNativeState::setTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP1, GX_COLOR0A0);
    J3DNativeState::setTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_CPREV);
    J3DNativeState::setTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
    J3DNativeState::setTevAlphaIn(GX_TEVSTAGE3, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    J3DNativeState::setTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
    J3DNativeState::setBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    J3DNativeState::setAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_GREATER, 0);
    J3DNativeState::setZMode(1, GX_LEQUAL, 0);
    J3DNativeState::setZCompLoc(1);
    J3DNativeState::setCullMode(GX_CULL_NONE);
    J3DNativeState::setClipMode(GX_CLIP_ENABLE);
}

void OceanRingDrawer::loadMaterialBloom() const {
    J3DNativeShape::sendPosition(MR::getCameraViewMtx(), 0);
    J3DNativeState::setCurrentMtx(0);
    J3DNativeState::setNumChans(0);
    J3DNativeState::setNumTexGens(2);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, 0, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, 0x21, 0, 0x7D);
>>>>>>> upstream/main

    TMtx34f mtx;
    mtx.identity();
    mtx.mMtx[0][2] = _C;
    mtx.mMtx[1][2] = _10;
<<<<<<< HEAD
    GXLoadTexMtxImm(mtx.toMtxPtr(), 0x1E, GX_MTX2x4);
    mtx.mMtx[0][2] = _14;
    mtx.mMtx[1][2] = _18;
    GXLoadTexMtxImm(mtx.toMtxPtr(), 0x21, GX_MTX2x4);
    mWaterTex->load(GX_TEXMAP0);
    GXSetNumIndStages(0);
=======
    J3DNativeShape::sendTexture(mtx.toMtxPtr(), 0x1E, GX_MTX2x4);
    mtx.mMtx[0][2] = _14;
    mtx.mMtx[1][2] = _18;
    J3DNativeShape::sendTexture(mtx.toMtxPtr(), 0x21, GX_MTX2x4);
    mWaterTex->load(GX_TEXMAP0);
    J3DNativeState::setNumIndStages(0);
>>>>>>> upstream/main
    TVec3f zDir = MR::getCamZdir();
    f32 v6 = ((MR::clamp(zDir.y, -0.2f, 0.1f) - -0.2f) / 0.3f);
    f32 v8 = ((MR::clamp(WaterAreaFunction::getCameraWaterDepth(), 150.0f, 300.0f) - 150.0f) / 150.0f);

    if (v6 >= v8) {
        v8 = v8;
    } else {
        v8 = v6;
    }

    f32 v18 = (150.0f + (105.0f * v8));
    u8 v19 = (s32)(v18);
<<<<<<< HEAD
    GXSetNumTevStages(2);
    Color8 v14(80, 80, 80, v19);
    Color8 v13(80, 80, 80, 255);
    GXSetTevColor(GX_TEVREG0, (GXColor&)v14);
    GXSetTevColor(GX_TEVREG1, (GXColor&)v13);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_C1);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1u, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1u, GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1u, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_APREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1u, GX_TEVPREV);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_GREATER, 0);
    GXSetZMode(1u, GX_LEQUAL, 1u);
    GXSetZCompLoc(1u);
    GXSetCullMode(GX_CULL_NONE);
    GXSetClipMode(GX_CLIP_ENABLE);
=======
    J3DNativeState::setNumTevStages(2);
    Color8 v14(80, 80, 80, v19);
    Color8 v13(80, 80, 80, 255);
    J3DNativeState::setTevColor(GX_TEVREG0, (GXColor&)v14);
    J3DNativeState::setTevColor(GX_TEVREG1, (GXColor&)v13);
    J3DNativeState::setTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    J3DNativeState::setTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_C1);
    J3DNativeState::setTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1u, GX_TEVPREV);
    J3DNativeState::setTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
    J3DNativeState::setTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1u, GX_TEVPREV);
    J3DNativeState::setTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
    J3DNativeState::setTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_CPREV);
    J3DNativeState::setTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1u, GX_TEVPREV);
    J3DNativeState::setTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_APREV);
    J3DNativeState::setTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1u, GX_TEVPREV);
    J3DNativeState::setBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    J3DNativeState::setAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_GREATER, 0);
    J3DNativeState::setZMode(1u, GX_LEQUAL, 1u);
    J3DNativeState::setZCompLoc(1u);
    J3DNativeState::setCullMode(GX_CULL_NONE);
    J3DNativeState::setClipMode(GX_CLIP_ENABLE);
>>>>>>> upstream/main
}
