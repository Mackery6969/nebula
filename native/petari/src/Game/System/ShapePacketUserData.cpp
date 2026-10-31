#include "Game/System/ShapePacketUserData.hpp"
<<<<<<< HEAD
=======
#include "JSystem/J3DGraphBase/J3DNativeShape.hpp"
>>>>>>> upstream/main
#include "Game/Util/CameraUtil.hpp"
#include "Game/Util/ModelUtil.hpp"
#include "Game/Util/SchedulerUtil.hpp"
#include "JSystem/J3DGraphAnimator/J3DModel.hpp"
#include "JSystem/J3DGraphAnimator/J3DModelData.hpp"
#include "JSystem/J3DGraphBase/J3DMaterial.hpp"
#include "JSystem/J3DGraphBase/J3DShape.hpp"
#include "JSystem/J3DGraphBase/J3DSys.hpp"
#include "JSystem/J3DGraphBase/J3DTexture.hpp"
#include "JSystem/JKernel/JKRHeap.hpp"
<<<<<<< HEAD
#include <revolution/gd.h>
#include <revolution/gd/GDTransform.h>

ShapePacketUserData::ShapePacketUserData() : mTexGenNum(), mDisplayListSize(), mDisplayList() {
}

void ShapePacketUserData::init(J3DMaterial* pMaterial) {
    MR::ProhibitSchedulerAndInterrupts prohibit(false);
    const bool isEnvMap = MR::isUseTexMtxEnvMap(pMaterial);
    u32 size = isEnvMap ? 0x20 : 0;
=======

ShapePacketUserData::ShapePacketUserData() : mTexGenNum(), mEnvMap(), mTexGenType(), mTexGenSource() {
}

void ShapePacketUserData::init(J3DMaterial* pMaterial) {
    const bool isEnvMap = MR::isUseTexMtxEnvMap(pMaterial);
    mEnvMap = isEnvMap;
>>>>>>> upstream/main
    mTexGenNum = pMaterial->getTexGenNum();
    for (u32 i = 0; i < mTexGenNum; i++) {
        J3DTexCoord* pTexCoord = pMaterial->getTexCoord(i);
        if (pTexCoord->getTexGenSrc() == GX_TG_POS) {
            mTexMtxInfo[i].mType = 1;
            mTexMtxInfo[i].mPostMtx = GX_PTTEXMTX0 + i * 3;
<<<<<<< HEAD
            size += 0x20;
        } else if (pTexCoord->getTexGenSrc() == GX_TG_NRM) {
            mTexMtxInfo[i].mType = 2;
            mTexMtxInfo[i].mPostMtx = GX_PTTEXMTX0 + i * 3;
            size += 0x20;
        } else if (isEnvMap && pTexCoord->getTexGenMtx() != GX_IDENTITY) {
            mTexMtxInfo[i].mType = 3;
            mTexMtxInfo[i].mPostMtx = GX_PTTEXMTX0 + i * 3;
            size += 0x20;
=======
        } else if (pTexCoord->getTexGenSrc() == GX_TG_NRM) {
            mTexMtxInfo[i].mType = 2;
            mTexMtxInfo[i].mPostMtx = GX_PTTEXMTX0 + i * 3;
        } else if (isEnvMap && pTexCoord->getTexGenMtx() != GX_IDENTITY) {
            mTexMtxInfo[i].mType = 3;
            mTexMtxInfo[i].mPostMtx = GX_PTTEXMTX0 + i * 3;
>>>>>>> upstream/main
        } else {
            mTexMtxInfo[i].mType = 0;
            mTexMtxInfo[i].mPostMtx = GX_PTIDENTITY;
        }
    }

<<<<<<< HEAD
    mDisplayList = new (32) u8[size + 0x40];
    DCInvalidateRange(mDisplayList, size + 0x40);
    GDLObj displayList;
    GDInitGDLObj(&displayList, mDisplayList, size + 0x40);
    GDSetCurrent(&displayList);
    if (isEnvMap) {
        GDSetCurrentMtx(0, GX_IDENTITY, GX_IDENTITY, GX_IDENTITY, GX_IDENTITY, GX_IDENTITY, GX_IDENTITY, GX_IDENTITY, GX_IDENTITY);
    }

    for (u32 i = 0; i < mTexGenNum; i++) {
        if (mTexMtxInfo[i].mType != 0) {
            J3DTexCoord* pTexCoord = pMaterial->getTexCoord(i);
            GDSetTexCoordGen(static_cast< GXTexCoordID >(i), static_cast< GXTexGenType >(pTexCoord->getTexGenType()),
                             static_cast< GXTexGenSrc >(pTexCoord->getTexGenSrc()), GX_FALSE, mTexMtxInfo[i].mPostMtx);
        }
    }

    GDPadCurr32();
    mDisplayListSize = GDGetGDLObjOffset(&displayList);
    DCStoreRange(mDisplayList, size + 0x40);
}

void ShapePacketUserData::callDL() const {
    GXCallDisplayList(mDisplayList, mDisplayListSize);
=======
    for (u32 i = 0; i < mTexGenNum; i++) {
        J3DTexCoord* pTexCoord = pMaterial->getTexCoord(i);
        mTexGenType[i] = pTexCoord->getTexGenType();
        mTexGenSource[i] = pTexCoord->getTexGenSrc();
    }
}

void ShapePacketUserData::load() const {
    if (mEnvMap) {
        static const u8 kIdentity[8] = {GX_IDENTITY, GX_IDENTITY, GX_IDENTITY, GX_IDENTITY,
                                        GX_IDENTITY, GX_IDENTITY, GX_IDENTITY, GX_IDENTITY};
        J3DNativeShape::setCurrent(0, kIdentity);
    }
    for (u32 i = 0; i < mTexGenNum; i++) {
        if (mTexMtxInfo[i].mType != 0) {
            J3DNativeShape::setTexGen(i, mTexGenType[i], mTexGenSource[i], false, static_cast< u8 >(mTexMtxInfo[i].mPostMtx));
        }
    }
>>>>>>> upstream/main
}

void ShapePacketUserData::loadTexMtx(J3DMaterial* pMaterial, int slot, u16 index) const {
    TPos3f mtx;
    for (u32 i = 0; i < mTexGenNum; i++) {
        if (mTexMtxInfo[i].mType != 0) {
            if (mTexMtxInfo[i].mType == 2) {
                mtx.set(j3dSys.getModelDrawMtx(index));
                mtx.setTrans(0.0f, 0.0f, 0.0f);
<<<<<<< HEAD
                GXLoadTexMtxImm(mtx, GX_TEXMTX0 + slot * 3, static_cast< GXTexMtxType >(pMaterial->getTexCoord(i)->getTexGenType()));
=======
                J3DNativeShape::loadTexture(mtx, GX_TEXMTX0 + slot * 3, static_cast< GXTexMtxType >(pMaterial->getTexCoord(i)->getTexGenType()));
>>>>>>> upstream/main
            }

            J3DTexMtx* const pTexMtx = pMaterial->getTexMtx(i);
            pTexMtx->calcPostTexMtx(MR::getCameraInvViewMtx());
<<<<<<< HEAD
            GXLoadTexMtxImm(pTexMtx->getMtx(), mTexMtxInfo[i].mPostMtx, GX_MTX3x4);
=======
            J3DNativeShape::loadTexture(pTexMtx->getMtx(), mTexMtxInfo[i].mPostMtx, GX_MTX3x4);
>>>>>>> upstream/main
        }
    }
}

namespace MR {
    ShapePacketUserData* getJ3DShapePacketUserData(const J3DShapePacket* pShapePacket) {
        if (pShapePacket->getUserArea() != nullptr) {
            return static_cast< ShapePacketUserData* >(pShapePacket->getUserArea());
        }

        return nullptr;
    }

    void initJ3DShapePacketUserData(J3DModel* pModel) {
        J3DModelData* pModelData = pModel->getModelData();
        const u16 materialNum = pModelData->getMaterialNum();
        for (u16 i = 0; i < materialNum; i++) {
            J3DMaterial* pMaterial = pModelData->getMaterialNodePointer(i);
            J3DShapePacket* pShapePacket = pModel->getShapePacket(pMaterial->getShape()->getIndex());
            if (isEnvelope(pMaterial) && (isUseTexMtxProjMap(pMaterial) || isUseTexMtxEnvMap(pMaterial))) {
                ShapePacketUserData* pUserData = new ShapePacketUserData();
                pUserData->init(pMaterial);
                if (pUserData != nullptr) {
                    pShapePacket->setUserArea(reinterpret_cast< uintptr_t >(pUserData));
                }
            }
        }
    }
};  // namespace MR
