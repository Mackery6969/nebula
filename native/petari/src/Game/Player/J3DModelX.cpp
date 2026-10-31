#include "Game/Player/J3DModelX.hpp"
<<<<<<< HEAD
=======
#include "JSystem/J3DGraphBase/J3DNativeMaterial.hpp"
>>>>>>> upstream/main
#include "Game/System/ShapePacketUserData.hpp"
#include "Game/Util/DirectDraw.hpp"
#include "Game/Util/MathUtil.hpp"
#include "Game/Util/MemoryUtil.hpp"
#include "Game/Util/MtxUtil.hpp"
#include "Game/Util/SchedulerUtil.hpp"
#include "Game/Util/ScreenUtil.hpp"
#include <JSystem/J3DGraphAnimator/J3DMtxBuffer.hpp>
#include <JSystem/J3DGraphBase/J3DMaterial.hpp>
#include <JSystem/J3DGraphBase/J3DShapeDraw.hpp>
#include <JSystem/J3DGraphBase/J3DShapeMtx.hpp>
#include <JSystem/JKernel/JKRHeap.hpp>
#include <JSystem/JUtility/JUTVideo.hpp>
<<<<<<< HEAD
#include <revolution/gd.h>
=======
>>>>>>> upstream/main

class J3DMtxBuffer2 : public J3DMtxBuffer {
public:
    void rotationMtx(MtxPtr mtx) {
        mpDrawMtxArr[1][mCurrentViewNo] = reinterpret_cast< Mtx* >(mtx);
    }
    void calcNrmMtx2();
    void calcDrawMtx2(u32, const Vec&, const Mtx&, J3DMtxBuffer*);
    void calcDrawMtx3(u32, const Vec&, const Mtx&, J3DMtxBuffer*, const TVec3f&, const TVec3f&);
};

class J3DShapeX : public J3DShape {};

class J3DShapePacketX : public J3DShapePacket {};

void J3DModelX::viewCalc2() {
    J3DMtxBuffer2* buffer = static_cast< J3DMtxBuffer2* >(mMtxBuffer);
    buffer->swapDrawMtx();
    buffer->swapNrmMtx();
    buffer->calcDrawMtx(getMtxCalcMode(), mBaseScale, mBaseTransformMtx);
    buffer->calcNrmMtx2();

    calcBBoardMtx();
    calcBumpMtx();
<<<<<<< HEAD
    DCStoreRangeNoSync(getDrawMtxPtr(), mModelData->getDrawMtxNum() * sizeof(Mtx));
=======
>>>>>>> upstream/main
    prepareShapePackets();
}

void J3DModelX::setDrawView(u32 view) {
    static_cast< J3DMtxBuffer2* >(mMtxBuffer)->rotationMtx(*mExtraMtxBuffer[view + _DC]);
}

void J3DModelX::setDrawViewBuffer(MtxPtr mtx) {
    mMtxBuffer->mpDrawMtxArr[1][mMtxBuffer->mCurrentViewNo] = reinterpret_cast< Mtx* >(mtx);
}

void J3DModelX::copyAnmMtxBuffer(const J3DModelX* model) {
    mMtxBuffer->mpAnmMtx = model->mMtxBuffer->mpAnmMtx;
}

void J3DModelX::viewCalc3(u32 view, MtxPtr mtx) {
    J3DMtxBuffer2* buffer = static_cast< J3DMtxBuffer2* >(mMtxBuffer);
    buffer->rotationMtx(*mExtraMtxBuffer[view + _DC]);

    if (mtx) {
        buffer->calcDrawMtx(getMtxCalcMode(), mBaseScale, reinterpret_cast< const Mtx& >(mtx));
    } else {
        buffer->calcDrawMtx(getMtxCalcMode(), mBaseScale, mBaseTransformMtx);
    }

    if (!(view & 1)) {
        buffer->swapNrmMtx();
        buffer->calcNrmMtx2();
    }

    calcBBoardMtx();
    calcBumpMtx();
<<<<<<< HEAD
    DCStoreRangeNoSync(getDrawMtxPtr(), mModelData->getDrawMtxNum() * sizeof(Mtx));
=======
>>>>>>> upstream/main
    prepareShapePackets();
}

void J3DModelX::viewCalcRef(u32 view, J3DModel* model) {
    J3DMtxBuffer2* buffer = static_cast< J3DMtxBuffer2* >(mMtxBuffer);
    buffer->rotationMtx(*mExtraMtxBuffer[view + _DC]);
    buffer->calcDrawMtx2(getMtxCalcMode(), mBaseScale, mBaseTransformMtx, model->getMtxBuffer());
<<<<<<< HEAD
    DCStoreRangeNoSync(getDrawMtxPtr(), mModelData->getDrawMtxNum() * sizeof(Mtx));
=======
>>>>>>> upstream/main
    prepareShapePackets();
}

void J3DModelX::viewCalcRefPos(u32 view, J3DModel* model, const TVec3f& pos, const TVec3f& normal) {
    J3DMtxBuffer2* buffer = static_cast< J3DMtxBuffer2* >(mMtxBuffer);
    buffer->rotationMtx(*mExtraMtxBuffer[view + _DC]);
    buffer->calcDrawMtx3(getMtxCalcMode(), mBaseScale, mBaseTransformMtx, model->getMtxBuffer(), pos, normal);
<<<<<<< HEAD
    DCStoreRangeNoSync(getDrawMtxPtr(), mModelData->getDrawMtxNum() * sizeof(Mtx));
=======
>>>>>>> upstream/main
    prepareShapePackets();
}

void J3DMtxBuffer2::calcNrmMtx2() {
    u16 count = mJointTree->getDrawMtxNum();

    for (u16 i = 0; i < count; i++) {
        J3DPSCalcInverseTranspose(*getDrawMtx(i), *getNrmMtx(i));
    }

<<<<<<< HEAD
    DCStoreRange(getNrmMtxPtr(), count * sizeof(Mtx33));
=======
>>>>>>> upstream/main
}

void J3DMtxBuffer2::calcDrawMtx2(u32 mode, const Vec& scale, const Mtx& base, J3DMtxBuffer* buffer) {
    Mtx viewBase;
    J3DCalcViewBaseMtx(j3dSys.getViewMtx(), scale, base, viewBase);

    for (u16 i = 0, count = mJointTree->getDrawFullWgtMtxNum(); i < count; i++) {
<<<<<<< HEAD
        PSMTXConcat(viewBase, buffer->getAnmMtx(mJointTree->getDrawMtxIndex(i)), *getDrawMtx(i));
=======
        NebulaMtxConcat(viewBase, buffer->getAnmMtx(mJointTree->getDrawMtxIndex(i)), *getDrawMtx(i));
>>>>>>> upstream/main
    }

    if (mJointTree->getDrawMtxNum() > mJointTree->getDrawFullWgtMtxNum()) {
        J3DPSMtxArrayConcat(viewBase, buffer->getWeightAnmMtx(0), *getDrawMtx(mJointTree->getDrawFullWgtMtxNum()), mJointTree->getWEvlpMtxNum());
    }
}

void J3DMtxBuffer2::calcDrawMtx3(u32 mode, const Vec& scale, const Mtx& base, J3DMtxBuffer* buffer, const TVec3f& pos, const TVec3f& normal) {
    Mtx viewBase;
    J3DCalcViewBaseMtx(j3dSys.getViewMtx(), scale, base, viewBase);
    u16 count = mJointTree->getDrawFullWgtMtxNum();

    for (u16 i = 0; i < count; i++) {
        Mtx jointMtx;
        TPos3f orientation;
        Mtx flatten;
        TVec3f trans;
        TVec3f planar;

        MR::extractMtxTrans(buffer->getAnmMtx(mJointTree->getDrawMtxIndex(i)), &trans);
        MR::vecKillElement(trans - pos, normal, &planar);
        trans = pos + planar;

<<<<<<< HEAD
        PSMTXCopy(buffer->getAnmMtx(mJointTree->getDrawMtxIndex(i)), jointMtx);
        MR::setMtxTrans(jointMtx, 0.0f, 0.0f, 0.0f);
        MR::makeMtxUpNoSupportPos(&orientation, normal, TVec3f(0.0f, 0.0f, 0.0f));
        PSMTXConcat(orientation.toMtxPtr(), jointMtx, jointMtx);
        PSMTXScale(flatten, 1.0f, 0.1f, 1.0f);
        PSMTXConcat(flatten, jointMtx, jointMtx);
        PSMTXInverse(orientation.toMtxPtr(), orientation.toMtxPtr());
        PSMTXConcat(orientation.toMtxPtr(), jointMtx, jointMtx);
        MR::setMtxTrans(jointMtx, trans.x, trans.y, trans.z);
        PSMTXConcat(viewBase, jointMtx, *getDrawMtx(i));
=======
        NebulaMtxCopy(buffer->getAnmMtx(mJointTree->getDrawMtxIndex(i)), jointMtx);
        MR::setMtxTrans(jointMtx, 0.0f, 0.0f, 0.0f);
        MR::makeMtxUpNoSupportPos(&orientation, normal, TVec3f(0.0f, 0.0f, 0.0f));
        NebulaMtxConcat(orientation.toMtxPtr(), jointMtx, jointMtx);
        NebulaMtxScale(flatten, 1.0f, 0.1f, 1.0f);
        NebulaMtxConcat(flatten, jointMtx, jointMtx);
        NebulaMtxInverse(orientation.toMtxPtr(), orientation.toMtxPtr());
        NebulaMtxConcat(orientation.toMtxPtr(), jointMtx, jointMtx);
        MR::setMtxTrans(jointMtx, trans.x, trans.y, trans.z);
        NebulaMtxConcat(viewBase, jointMtx, *getDrawMtx(i));
>>>>>>> upstream/main
    }
}

void J3DModelX::directDraw(J3DModel* model) {
    j3dSys.setModel(this);
    if (J3DModel::checkFlag(4)) {
        j3dSys.onFlag(4);
    } else {
        j3dSys.offFlag(4);
    }
    if (J3DModel::checkFlag(8)) {
        j3dSys.onFlag(8);
    } else {
        j3dSys.offFlag(8);
    }
    mModelData->syncJ3DSysFlags();
    j3dSys.setTexture(mModelData->getTexture());

    bool fog = false;
    if (_1D0) {
        fog = true;
        _1D0--;
    }
    for (u16 i = 0; i < mModelData->getMaterialNum(); i++) {
        J3DMaterial* material = mModelData->getMaterialNodePointer(i);
        _1C0 = i;
        if (model) {
            drawIn(material, fog, model->getBaseTRMtx(), model);
        } else {
            drawIn(material, fog, mBaseTransformMtx, nullptr);
        }
    }
<<<<<<< HEAD

    J3DShape::resetVcdVatCache();
=======
>>>>>>> upstream/main
}

void J3DModelX::drawIn(J3DMaterial* material, bool fog, MtxPtr base, J3DModel* model) {
    if (!material) {
        return;
    }
    if (!material->getShape()) {
        return;
    }
    if (material->getShape()->checkFlag(1)) {
        return;
    }
    J3DMatPacket* matPacket = getMatPacket(material->getIndex());
    J3DShapePacket* packet = model ? model->getShapePacket(material->getShape()->getIndex()) : getShapePacket(material->getShape()->getIndex());
    j3dSys.setMatPacket(matPacket);
<<<<<<< HEAD
    matPacket->callDL();
=======
    J3DNativeMaterial::send(matPacket->getMaterial(), matPacket->mpTexture, matPacket->getDisplayListObj());
>>>>>>> upstream/main

    if (fog) {
        TVec3f pos;
        MR::extractMtxTrans(mBaseTransformMtx, &pos);
        TDDraw::mixFogColor(pos, _1D4, _1D8);
    }

    packet->getShape()->loadPreDrawSetting();
    if (packet->getDisplayListObj()) {
<<<<<<< HEAD
        packet->getDisplayListObj()->callDL();
    }
    if (MR::getJ3DShapePacketUserData(packet)) {
        MR::getJ3DShapePacketUserData(packet)->callDL();
=======
        J3DNativeMaterial::sendDiffered(packet->getDisplayListObj());
    }
    if (MR::getJ3DShapePacketUserData(packet)) {
        MR::getJ3DShapePacketUserData(packet)->load();
>>>>>>> upstream/main
    }

    for (u32 i = 0; i <= 15; i++) {
        if (*reinterpret_cast< u32* >(&mFlags) & (1 << i)) {
<<<<<<< HEAD
            GXCallDisplayList(mDisplayLists[i], mDisplayListSizes[i]);
=======
            mStateLists[i].send();
>>>>>>> upstream/main
        }
    }

    if (_1B8) {
        _1B4 = _1B8;
        _1B8 = nullptr;
    }
<<<<<<< HEAD
    if (_1B4 && _1BC) {
        GXCallDisplayList(_1B4, _1BC);
=======
    if (_1B4) {
        _1B4->send();
>>>>>>> upstream/main
    }

    u32 index = _1C0;
    if (_1C8[index]) {
        _1C4[index] = _1C8[index];
        _1C8[index] = nullptr;
    }
<<<<<<< HEAD
    if (_1C4[index] && _1CC[index]) {
        GXCallDisplayList(_1C4[index], _1CC[index]);
=======
    if (_1C4[index]) {
        _1C4[index]->send();
>>>>>>> upstream/main
    }
    if (_120) {
        _120(_128, _1C0);
    }

    J3DShape* original;
    if (_12C) {
        J3DShapePacket* replacement = _12C->getShapePacket(material->getShape()->getIndex());
        original = packet->getShape();
        packet->setShape(replacement->getShape());
        packet->mpShape->mDrawMtx = original->mDrawMtx;
        packet->mpShape->mDrawMtxData = original->mDrawMtxData;
        packet->mpShape->mCurrentMtx = material->mCurrentMtx;
    }

    shapePacketDrawFast(static_cast< J3DShapePacketX* >(packet));
    if (_12C) {
        packet->mpShape = original;
    }
}

bool J3DModelX::simpleDrawSetup(J3DMaterial* material) {
    if (!material) {
        return false;
    }
    if (!material->getShape()) {
        return false;
    }
    if (material->getShape()->checkFlag(1)) {
        return false;
    }

<<<<<<< HEAD
    J3DShape::resetVcdVatCache();
    J3DMatPacket* matPacket = getMatPacket(material->getIndex());
    J3DShapePacket* packet = getShapePacket(material->getShape()->getIndex());
    j3dSys.setMatPacket(matPacket);
    matPacket->callDL();
    packet->getShape()->loadPreDrawSetting();
    if (packet->getDisplayListObj()) {
        packet->getDisplayListObj()->callDL();
=======
    J3DMatPacket* matPacket = getMatPacket(material->getIndex());
    J3DShapePacket* packet = getShapePacket(material->getShape()->getIndex());
    j3dSys.setMatPacket(matPacket);
    J3DNativeMaterial::send(matPacket->getMaterial(), matPacket->mpTexture, matPacket->getDisplayListObj());
    packet->getShape()->loadPreDrawSetting();
    if (packet->getDisplayListObj()) {
        J3DNativeMaterial::sendDiffered(packet->getDisplayListObj());
>>>>>>> upstream/main
    }
    return true;
}

void J3DModelX::simpleDrawShape(J3DMaterial* material) {
    getShapePacket(material->getShape()->getIndex())->drawFast();
}

<<<<<<< HEAD
void J3DModelX::storeDisplayList(GDLObj* obj, u32 index) {
    GDPadCurr32();
    u32 size = (GDGetGDLObjOffset(obj) + 31) & ~31;
    mDisplayLists[index] = new (32) u8[size];
    MR::copyMemory(mDisplayLists[index], obj->start, size);
    DCStoreRange(mDisplayLists[index], size);
    mDisplayListSizes[index] = GDGetGDLObjOffset(obj);
    GDInitGDLObj(obj, obj->start, obj->length);
}

J3DModelX::J3DModelX(J3DModelData* data, u32 flags, u32 bufferFlags) : J3DModel(data, flags, bufferFlags) {
    MR::ProhibitSchedulerAndInterrupts guard(false);
=======
void J3DModelX::storeStateList(J3DStateList& rBuilder, u32 index) {
    mStateLists[index].copy(rBuilder);
    rBuilder.clear();
}

J3DModelX::J3DModelX(J3DModelData* data, u32 flags, u32 bufferFlags) : J3DModel(data, flags, bufferFlags) {
>>>>>>> upstream/main
    _DC = 0;
    _DD = 0;
    _120 = nullptr;
    _128 = nullptr;
    _12C = nullptr;
    mShapeCallback = nullptr;
    _1E4 = 0;
    _1E5 = 0;

<<<<<<< HEAD
    GDLObj obj;
    u8 buffer[512] ATTRIBUTE_ALIGN(32);
    GDInitGDLObj(&obj, buffer, sizeof(buffer));
    GDSetCurrent(&obj);

    GDSetCullMode(GX_CULL_FRONT);
    storeDisplayList(&obj, 0);

    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GDSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GDSetGenMode2(0, 1, 1, 0, GX_CULL_BACK);
    GDSetTevDirect(GX_TEVSTAGE0);
    GDSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetBlendModeEtc(GX_BM_NONE, GX_BL_ZERO, GX_BL_INVSRCALPHA, GX_LO_CLEAR, GX_FALSE, GX_TRUE, GX_FALSE);
    GDSetDstAlpha(GX_TRUE, 0xFF);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ADDHALF, GX_CS_DIVIDE_2, GX_TRUE,
                             GX_TEVPREV, GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeDisplayList(&obj, 1);

    GDSetCullMode(GX_CULL_FRONT);
    GDSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GDSetBlendModeEtc(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR, GX_TRUE, GX_FALSE, GX_FALSE);
    GDSetGenMode2(1, 0, 1, 0, GX_CULL_FRONT);
    GDSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GDSetTevColorCalc(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ADDHALF, GX_CS_DIVIDE_2, GX_TRUE,
                             GX_TEVPREV, GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeDisplayList(&obj, 2);

    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GDSetZMode(GX_TRUE, GX_GEQUAL, GX_FALSE);
    GDSetGenMode2(0, 1, 1, 0, GX_CULL_BACK);
    GDSetTevDirect(GX_TEVSTAGE0);
    GDSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetBlendModeEtc(GX_BM_NONE, GX_BL_ZERO, GX_BL_INVSRCALPHA, GX_LO_CLEAR, GX_FALSE, GX_TRUE, GX_FALSE);
    GDSetDstAlpha(GX_TRUE, 0x90);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ADDHALF, GX_CS_DIVIDE_2, GX_TRUE,
                             GX_TEVPREV, GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeDisplayList(&obj, 3);

    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GDSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GDSetGenMode2(0, 1, 1, 0, GX_CULL_FRONT);
    GDSetTevDirect(GX_TEVSTAGE0);
    GDSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetBlendModeEtc(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR, GX_TRUE, GX_FALSE, GX_FALSE);
    static GXColor highlightColor = {255, 255, 0, 255};
    GDSetTevColor(GX_TEVREG0, highlightColor);
    GDSetTevColorCalc(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeDisplayList(&obj, 4);

    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GDSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    GDSetGenMode2(0, 1, 1, 0, GX_CULL_FRONT);
    GDSetBlendModeEtc(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR, GX_FALSE, GX_FALSE, GX_FALSE);
    GDSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetTevColorCalc(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    GDSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    storeDisplayList(&obj, 5);

    GDSetTexLookupMode(GX_TEXMAP0, GX_REPEAT, GX_REPEAT, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GDSetTexImgAttr(GX_TEXMAP0, MR::getScreenWidth(), JUTVideo::getManager()->getEfbHeight(), GX_TF_RGB565);
    GDSetTexImgPtr(GX_TEXMAP0, const_cast< u8* >(reinterpret_cast< const u8* >(MR::getScreenResTIMG())) + MR::getScreenResTIMG()->mImageDataOffset);
    GDSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_NRM, GX_TRUE, GX_IDENTITY);
    storeDisplayList(&obj, 6);

    GXColor ambientColor = {0, 0, 0, 0};
    GXColor materialColor = {0, 0, 0, 255};
    GDSetChanAmbColor(GX_ALPHA0, ambientColor);
    GDSetChanMatColor(GX_ALPHA0, materialColor);
    GDSetBlendModeEtc(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP, GX_FALSE, GX_TRUE, GX_FALSE);
    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GDSetChanCtrl(GX_COLOR0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetChanCtrl(GX_ALPHA0, GX_TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_KONST, GX_CA_ZERO, GX_CA_RASA, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                             GX_TEVPREV, GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeDisplayList(&obj, 7);

    GDSetCullMode(GX_CULL_FRONT);
    GDSetBlendModeEtc(GX_BM_BLEND, GX_BL_ZERO, GX_BL_ZERO, GX_LO_NOOP, GX_FALSE, GX_TRUE, GX_FALSE);
    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GDSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXColor subtractColor = {0, 0, 0, 252};
    GDSetTevColor(GX_TEVREG0, subtractColor);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeDisplayList(&obj, 8);

    GDSetCullMode(GX_CULL_BACK);
    GDSetBlendModeEtc(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP, GX_FALSE, GX_TRUE, GX_FALSE);
    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GDSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXColor addColor = {0, 0, 0, 4};
    GDSetTevColor(GX_TEVREG0, addColor);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeDisplayList(&obj, 9);

    GDSetBlendModeEtc(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_INVDSTALPHA, GX_LO_NOOP, GX_TRUE, GX_TRUE, GX_FALSE);
    GDSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GDSetDstAlpha(GX_TRUE, 0);
    storeDisplayList(&obj, 10);

    GDSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GDSetGenMode2(1, 0, 1, 0, GX_CULL_FRONT);
    GDSetAlphaCompare(GX_GREATER, 0x20, GX_AOP_AND, GX_ALWAYS, 0);
    GDSetBlendModeEtc(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_NOOP, GX_TRUE, GX_TRUE, GX_FALSE);
    GDSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetTevColorCalc(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, GX_TRUE, GX_TEVPREV);
    GDSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    storeDisplayList(&obj, 11);

    GDSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GDSetGenMode2(1, 0, 1, 0, GX_CULL_BACK);
    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GDSetBlendModeEtc(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP, GX_TRUE, GX_FALSE, GX_FALSE);
    GDSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetTevColorCalc(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_C0, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    GDSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    storeDisplayList(&obj, 12);

    GDSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GDSetZMode(GX_TRUE, GX_GEQUAL, GX_FALSE);
    GDSetGenMode2(0, 1, 1, 0, GX_CULL_BACK);
    GDSetTevDirect(GX_TEVSTAGE0);
    GDSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GDSetBlendModeEtc(GX_BM_BLEND, GX_BL_ONE, GX_BL_SRCALPHA, GX_LO_CLEAR, GX_TRUE, GX_FALSE, GX_FALSE);
    static GXColor tintColor = {35, 25, 25, 5};
    GDSetTevColor(GX_TEVREG0, tintColor);
    GDSetTevColorCalc(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GDSetTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeDisplayList(&obj, 13);

    GDSetDstAlpha(GX_TRUE, 0x40);
    storeDisplayList(&obj, 14);

    GDSetBlendModeEtc(GX_BM_BLEND, GX_BL_INVDSTALPHA, GX_BL_DSTALPHA, GX_LO_CLEAR, GX_TRUE, GX_FALSE, GX_FALSE);
    storeDisplayList(&obj, 15);
=======
    NebulaStateOp storage[16];
    J3DStateList list(storage, 16);

    list.setCullMode(GX_CULL_FRONT);
    storeStateList(list, 0);

    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    list.setZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    list.setGenMode2(0, 1, 1, 0, GX_CULL_BACK);
    list.setTevDirect(GX_TEVSTAGE0);
    list.setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setBlendModeEtc(GX_BM_NONE, GX_BL_ZERO, GX_BL_INVSRCALPHA, GX_LO_CLEAR, GX_FALSE, GX_TRUE, GX_FALSE);
    list.setDstAlpha(GX_TRUE, 0xFF);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ADDHALF, GX_CS_DIVIDE_2, GX_TRUE,
                             GX_TEVPREV, GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeStateList(list, 1);

    list.setCullMode(GX_CULL_FRONT);
    list.setZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    list.setBlendModeEtc(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR, GX_TRUE, GX_FALSE, GX_FALSE);
    list.setGenMode2(1, 0, 1, 0, GX_CULL_FRONT);
    list.setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    list.setTevColorCalc(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ADDHALF, GX_CS_DIVIDE_2, GX_TRUE,
                             GX_TEVPREV, GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeStateList(list, 2);

    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    list.setZMode(GX_TRUE, GX_GEQUAL, GX_FALSE);
    list.setGenMode2(0, 1, 1, 0, GX_CULL_BACK);
    list.setTevDirect(GX_TEVSTAGE0);
    list.setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setBlendModeEtc(GX_BM_NONE, GX_BL_ZERO, GX_BL_INVSRCALPHA, GX_LO_CLEAR, GX_FALSE, GX_TRUE, GX_FALSE);
    list.setDstAlpha(GX_TRUE, 0x90);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ADDHALF, GX_CS_DIVIDE_2, GX_TRUE,
                             GX_TEVPREV, GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeStateList(list, 3);

    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    list.setZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    list.setGenMode2(0, 1, 1, 0, GX_CULL_FRONT);
    list.setTevDirect(GX_TEVSTAGE0);
    list.setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setBlendModeEtc(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR, GX_TRUE, GX_FALSE, GX_FALSE);
    static GXColor highlightColor = {255, 255, 0, 255};
    list.setTevColor(GX_TEVREG0, highlightColor);
    list.setTevColorCalc(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeStateList(list, 4);

    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    list.setZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    list.setGenMode2(0, 1, 1, 0, GX_CULL_FRONT);
    list.setBlendModeEtc(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR, GX_FALSE, GX_FALSE, GX_FALSE);
    list.setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setTevColorCalc(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    list.setTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    storeStateList(list, 5);

    list.setTexLookupMode(GX_TEXMAP0, GX_REPEAT, GX_REPEAT, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    list.setTexImgAttr(GX_TEXMAP0, MR::getScreenWidth(), JUTVideo::getManager()->getEfbHeight(), GX_TF_RGB565);
    list.setTexImgPtr(GX_TEXMAP0, const_cast< u8* >(reinterpret_cast< const u8* >(MR::getScreenResTIMG())) + MR::getScreenResTIMG()->mImageDataOffset);
    list.setTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_NRM, GX_TRUE, GX_IDENTITY);
    storeStateList(list, 6);

    GXColor ambientColor = {0, 0, 0, 0};
    GXColor materialColor = {0, 0, 0, 255};
    list.setChanAmbColor(GX_ALPHA0, ambientColor);
    list.setChanMatColor(GX_ALPHA0, materialColor);
    list.setBlendModeEtc(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP, GX_FALSE, GX_TRUE, GX_FALSE);
    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    list.setChanCtrl(GX_COLOR0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setChanCtrl(GX_ALPHA0, GX_TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_KONST, GX_CA_ZERO, GX_CA_RASA, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                             GX_TEVPREV, GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeStateList(list, 7);

    list.setCullMode(GX_CULL_FRONT);
    list.setBlendModeEtc(GX_BM_BLEND, GX_BL_ZERO, GX_BL_ZERO, GX_LO_NOOP, GX_FALSE, GX_TRUE, GX_FALSE);
    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    list.setZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXColor subtractColor = {0, 0, 0, 252};
    list.setTevColor(GX_TEVREG0, subtractColor);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeStateList(list, 8);

    list.setCullMode(GX_CULL_BACK);
    list.setBlendModeEtc(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP, GX_FALSE, GX_TRUE, GX_FALSE);
    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    list.setZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXColor addColor = {0, 0, 0, 4};
    list.setTevColor(GX_TEVREG0, addColor);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeStateList(list, 9);

    list.setBlendModeEtc(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_INVDSTALPHA, GX_LO_NOOP, GX_TRUE, GX_TRUE, GX_FALSE);
    list.setZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    list.setDstAlpha(GX_TRUE, 0);
    storeStateList(list, 10);

    list.setZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    list.setGenMode2(1, 0, 1, 0, GX_CULL_FRONT);
    list.setAlphaCompare(GX_GREATER, 0x20, GX_AOP_AND, GX_ALWAYS, 0);
    list.setBlendModeEtc(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_NOOP, GX_TRUE, GX_TRUE, GX_FALSE);
    list.setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setTevColorCalc(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, GX_TRUE, GX_TEVPREV);
    list.setTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    storeStateList(list, 11);

    list.setZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    list.setGenMode2(1, 0, 1, 0, GX_CULL_BACK);
    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    list.setBlendModeEtc(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP, GX_TRUE, GX_FALSE, GX_FALSE);
    list.setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setTevColorCalc(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_C0, GX_CC_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    list.setTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    storeStateList(list, 12);

    list.setAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    list.setZMode(GX_TRUE, GX_GEQUAL, GX_FALSE);
    list.setGenMode2(0, 1, 1, 0, GX_CULL_BACK);
    list.setTevDirect(GX_TEVSTAGE0);
    list.setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    list.setBlendModeEtc(GX_BM_BLEND, GX_BL_ONE, GX_BL_SRCALPHA, GX_LO_CLEAR, GX_TRUE, GX_FALSE, GX_FALSE);
    static GXColor tintColor = {35, 25, 25, 5};
    list.setTevColor(GX_TEVREG0, tintColor);
    list.setTevColorCalc(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    list.setTevAlphaCalcAndSwap(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV,
                             GX_TEV_SWAP0, GX_TEV_SWAP0);
    storeStateList(list, 13);

    list.setDstAlpha(GX_TRUE, 0x40);
    storeStateList(list, 14);

    list.setBlendModeEtc(GX_BM_BLEND, GX_BL_INVDSTALPHA, GX_BL_DSTALPHA, GX_LO_CLEAR, GX_TRUE, GX_FALSE, GX_FALSE);
    storeStateList(list, 15);
>>>>>>> upstream/main

    _1E0 = -1;
    _1DC = 0x32323232;
    _1D8 = 0xFF0000FF;
    mFlags.clear();
    _1D4 = 0.0f;
    _1D0 = 0;
    _1B4 = nullptr;
    _1B8 = nullptr;
<<<<<<< HEAD
    _1BC = 0;

    u32 count = mModelData->getMaterialNum();
    _1C4 = new u8*[count];
    _1C8 = new u8*[count];
    _1CC = new u16[count];
    for (u32 i = 0; i < count; i++) {
        _1C4[i] = nullptr;
        _1C8[i] = nullptr;
        _1CC[i] = 0;
=======

    u32 count = mModelData->getMaterialNum();
    _1C4 = new const J3DStateList*[count];
    _1C8 = new const J3DStateList*[count];
    for (u32 i = 0; i < count; i++) {
        _1C4[i] = nullptr;
        _1C8[i] = nullptr;
>>>>>>> upstream/main
    }
    _DC = 0;
}

void J3DModelX::shapePacketDrawFast(J3DShapePacketX* packet) const {
    if (!(packet->mFlags & 0x10) && packet->getShape()) {
        packet->prepareDraw();
        if (packet->getTexMtxObj()) {
            J3DDifferedTexMtx::sTexGenBlock = packet->getShape()->mMaterial->mTexGenBlock;
            J3DDifferedTexMtx::sTexMtxObj = packet->getTexMtxObj();
        } else {
            J3DDifferedTexMtx::sTexGenBlock = nullptr;
        }
        shapeDrawFast(static_cast< J3DShapeX* >(packet->getShape()));
    }
}

void J3DModelX::shapeDrawFast(J3DShapeX* shape) const {
<<<<<<< HEAD
    if (J3DShape::sOldVcdVatCmd != shape->mVcdVatCmd) {
        GXCallDisplayList(shape->mVcdVatCmd, 0xC0);
        J3DShape::sOldVcdVatCmd = shape->mVcdVatCmd;
    }

=======
    shape->prepareGeometry();
>>>>>>> upstream/main
    if (J3DShape::sEnvelopeFlag && !shape->mHasPNMTXIdx) {
        shape->mCurrentMtx.load();
    }

    shape->setArrayAndBindPipeline();

    if (!(shape->mFlags & 0x200)) {
        if (J3DShapeMtx::sLODFlag) {
            J3DShapeMtx::resetMtxLoadCache();
        }
        u16 count = shape->mMtxGroupNum;
        for (u16 i = 0; i < count; i++) {
            if (shape->mShapeMtx[i]) {
                shape->mShapeMtx[i]->load();
            }
            if (mShapeCallback) {
                mShapeCallback(shape);
            }
            if (shape->mShapeDraw[i]) {
                shape->mShapeDraw[i]->draw();
            }
        }
    } else {
<<<<<<< HEAD
        J3DFifoLoadPosMtxImm(*j3dSys.getShapePacket()->getBaseMtxPtr(), 0);
        J3DFifoLoadNrmMtxImm(*j3dSys.getShapePacket()->getBaseMtxPtr(), 0);
=======
        J3DNativeShape::loadPosition(*j3dSys.getShapePacket()->getBaseMtxPtr(), 0);
        J3DNativeShape::loadNormal(*j3dSys.getShapePacket()->getBaseMtxPtr(), 0);
>>>>>>> upstream/main
        u16 count = shape->mMtxGroupNum;
        for (u16 i = 0; i < count; i++) {
            if (shape->mShapeDraw[i]) {
                shape->mShapeDraw[i]->draw();
            }
        }
    }
}

J3DModelX::~J3DModelX() {
}
