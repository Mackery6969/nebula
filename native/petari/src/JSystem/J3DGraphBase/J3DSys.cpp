#include "JSystem/J3DGraphBase/J3DSys.hpp"
<<<<<<< HEAD
#include "JSystem/J3DGraphBase/J3DFifo.hpp"
=======
#include <JSystem/J3DGraphBase/J3DNativeTexture.hpp>
#include <JSystem/J3DGraphBase/J3DNativeState.hpp>
#include "JSystem/J3DGraphBase/J3DNativeShape.hpp"
>>>>>>> upstream/main
#include "JSystem/J3DGraphBase/J3DTevs.hpp"
#include <revolution/gx/GXGeometry.h>
#include <revolution/os/OSFastCast.h>

J3DSys j3dSys;

Mtx J3DSys::mCurrentMtx;
Vec J3DSys::mCurrentS;
Vec J3DSys::mParentS;

static u8 NullTexData[0x10] ATTRIBUTE_ALIGN(32) = {0};

J3DTexCoordScaleInfo J3DSys::sTexCoordScaleTable[8];

static Mtx j3dIdentityMtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
};

static Mtx23 IndMtx = {
    0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
};

u32 j3dDefaultViewNo;

static GXColor ColorBlack = {0x00, 0x00, 0x00, 0x00};

static GXColor ColorWhite = {0xFF, 0xFF, 0xFF, 0xFF};

J3DSys::J3DSys() {
    makeTexCoordTable();
    makeTevSwapTable();
    makeAlphaCmpTable();
    makeZModeTable();

    mFlags = 0;
<<<<<<< HEAD
    PSMTXIdentity(mViewMtx);
=======
    NebulaMtxIdentity(mViewMtx);
>>>>>>> upstream/main
    mDrawMode = 1;
    mMaterialMode = 0;
    mModel = nullptr;
    mShape = nullptr;

    for (int i = 0; i < 2; i++)
        mDrawBuffer[i] = nullptr;

    mTexture = nullptr;
    mMatPacket = nullptr;
    mShapePacket = nullptr;
    mModelDrawMtx = nullptr;
    mModelNrmMtx = nullptr;
    mVtxPos = nullptr;
    mVtxNrm = nullptr;
    mVtxCol = nullptr;

    for (int i = 0; i < 8; i++) {
        sTexCoordScaleTable[i].field_0x00 = 1;
        sTexCoordScaleTable[i].field_0x02 = 1;
        sTexCoordScaleTable[i].field_0x04 = 0;
        sTexCoordScaleTable[i].field_0x06 = 0;
    }
}

<<<<<<< HEAD
void J3DSys::loadPosMtxIndx(int addr, u16 indx) const {
    J3DFifoLoadIndx(0x20, indx, 0xB000 | ((u16)(addr * 0x0C)));
}

void J3DSys::loadNrmMtxIndx(int addr, u16 indx) const {
    J3DFifoLoadNrmMtxIndx3x3(indx, addr * 3);
}

void J3DSys::setTexCacheRegion(GXTexCacheSize size) {
    const u32 kSize[] = {
        0x00008000,
        0x00020000,
        0x00080000,
        0x00000000,
    };

    const u32 kRegionNum[] = {8, 4, 1, 0};

    mTexCacheRegionNum = kRegionNum[size];

    if (checkFlag(0x80000000)) {
        u32 regionNum = kRegionNum[size];
        for (u32 i = 0; i < regionNum; i++) {
            if (i & 1) {
                GXInitTexCacheRegion(&mTexCacheRegion[i], GX_FALSE, i * kSize[size] + 0x80000, size, i * kSize[size], size);
                J3DFifoLoadTexCached(static_cast< GXTexMapID >(i), i * kSize[size] + 0x80000, size, i * kSize[size], size);
            } else {
                GXInitTexCacheRegion(&mTexCacheRegion[i], GX_FALSE, i * kSize[size], size, i * kSize[size] + 0x80000, size);
                J3DFifoLoadTexCached(static_cast< GXTexMapID >(i), i * kSize[size], size, i * kSize[size] + 0x80000, size);
            }
        }
    } else {
        u32 regionNum = kRegionNum[size];
        for (u32 i = 0; i < regionNum; i++) {
            GXInitTexCacheRegion(&mTexCacheRegion[i], GX_FALSE, i * kSize[size], size, i * kSize[size] + 0x80000, size);
            J3DFifoLoadTexCached(static_cast< GXTexMapID >(i), i * kSize[size], size, i * kSize[size] + 0x80000, size);
        }
    }
}

void J3DSys::drawInit() {
    GXInvalidateVtxCache();
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetCoPlanar(GX_FALSE);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetColorUpdate(GX_TRUE);
    GXSetDither(GX_TRUE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetZCompLoc(GX_TRUE);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR1, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_CLR_RGBA, GX_RGBA4, 7);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_CLR_RGBA, GX_RGBA4, 7);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_CLR_RGBA, GX_RGBA4, 7);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX3, GX_CLR_RGBA, GX_RGBA4, 7);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX4, GX_CLR_RGBA, GX_RGBA4, 7);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX5, GX_CLR_RGBA, GX_RGBA4, 7);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX6, GX_CLR_RGBA, GX_RGBA4, 7);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX7, GX_CLR_RGBA, GX_RGBA4, 7);

    GXSetNumIndStages(0);
    reinitTexture();

    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE7, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE8, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE9, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE10, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE11, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE12, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE13, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE14, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE15, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

    for (u32 i = 0; i < 10; i++)
        GXLoadTexMtxImm(j3dIdentityMtx, GX_TEXMTX0 + i * 3, GX_MTX3x4);
=======
// The model's draw and normal matrices `indx` into matrix slot `addr` (the next shape draw carries them).
void J3DSys::loadPosMtxIndx(int addr, u16 indx) const {
    J3DNativeShape::loadPosition(mModelDrawMtx[indx], addr * 3, indx + 1);
}

void J3DSys::loadNrmMtxIndx(int addr, u16 indx) const {
    J3DNativeShape::loadNormal33(mModelNrmMtx[indx], addr * 3, indx + 1);
}


void J3DSys::drawInit() {
    J3DNativeState::setCurrentMtx(GX_PNMTX0);
    J3DNativeState::setCullMode(GX_CULL_BACK);
    J3DNativeState::setCoPlanar(GX_FALSE);
    J3DNativeState::setClipMode(GX_CLIP_ENABLE);
    J3DNativeState::setColorUpdate(GX_TRUE);
    J3DNativeState::setDither(GX_TRUE);
    J3DNativeState::setBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    J3DNativeState::setAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    J3DNativeState::setZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    J3DNativeState::setZCompLoc(GX_TRUE);

    J3DNativeState::setNumIndStages(0);
    reinitTexture();

    J3DNativeState::setTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE6, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE7, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE8, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE9, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE10, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE11, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE12, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE13, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE14, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE15, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

    for (u32 i = 0; i < 10; i++)
        J3DNativeShape::sendTexture(j3dIdentityMtx, GX_TEXMTX0 + i * 3, GX_MTX3x4);
>>>>>>> upstream/main

    Mtx23 indTexMtx = {
        {0.5f, 0.0f, 0.0f},
        {0.0f, 0.5f, 0.0f},
    };

    u8 i;
    for (i = 0; i < 3; i++)
<<<<<<< HEAD
        GXSetIndTexMtx((GXIndTexMtxID)(GX_ITM_0 + i), indTexMtx, 1);

    GXSetChanMatColor(GX_COLOR0A0, j3dDefaultColInfo);
    GXSetChanMatColor(GX_COLOR1A1, j3dDefaultColInfo);

    GXSetNumChans(j3dDefaultNumChans);
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);

    for (i = 0; i < GX_MAX_TEXMAP; i++)
        GXSetTexCoordGen2((GXTexCoordID)i, (GXTexGenType)j3dDefaultTexCoordInfo[i].mTexGenType, (GXTexGenSrc)j3dDefaultTexCoordInfo[i].mTexGenSrc,
                          j3dDefaultTexCoordInfo[i].mTexGenMtx, GX_FALSE, 0x7D);

    for (i = 0; i < GX_MAX_INDTEXSTAGE; i++)
        GXSetIndTexCoordScale((GXIndTexStageID)i, GX_ITS_1, GX_ITS_1);

    for (i = 0; i < GX_MAX_TEVREG; i++)
        GXSetTevKColor((GXTevKColorID)i, j3dDefaultTevKColor);

    for (i = 0; i < GX_MAX_TEVREG; i++)
        GXSetTevColorS10((GXTevRegID)i, j3dDefaultTevColor);

    for (i = 0; i < GX_MAX_TEVSTAGE; i++) {
        GXSetTevColorIn((GXTevStageID)i, GX_CC_RASC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
        GXSetTevColorOp((GXTevStageID)i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn((GXTevStageID)i, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        GXSetTevAlphaOp((GXTevStageID)i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    }

    for (i = 0; i < GX_MAX_TEVSTAGE; i++)
        GXSetTevSwapMode((GXTevStageID)i, GX_TEV_SWAP0, GX_TEV_SWAP0);

    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);

    for (i = 0; i < GX_MAX_TEVSTAGE; i++)
        GXSetTevIndirect((GXTevStageID)i, GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE,
                         GX_ITBA_OFF);

    // OSInitFastCast();
=======
        J3DNativeState::setIndTexMtx((GXIndTexMtxID)(GX_ITM_0 + i), indTexMtx, 1);

    J3DNativeState::setChanMatColor(GX_COLOR0A0, j3dDefaultColInfo);
    J3DNativeState::setChanMatColor(GX_COLOR1A1, j3dDefaultColInfo);

    J3DNativeState::setNumChans(j3dDefaultNumChans);
    J3DNativeState::setNumTexGens(1);
    J3DNativeState::setNumTevStages(1);
    J3DNativeState::setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    J3DNativeState::setChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);

    for (i = 0; i < GX_MAX_TEXMAP; i++)
        J3DNativeState::setTexCoordGen2((GXTexCoordID)i, (GXTexGenType)j3dDefaultTexCoordInfo[i].mTexGenType, (GXTexGenSrc)j3dDefaultTexCoordInfo[i].mTexGenSrc,
                          j3dDefaultTexCoordInfo[i].mTexGenMtx, GX_FALSE, 0x7D);

    for (i = 0; i < GX_MAX_INDTEXSTAGE; i++)
        J3DNativeState::setIndTexCoordScale((GXIndTexStageID)i, GX_ITS_1, GX_ITS_1);

    for (i = 0; i < GX_MAX_TEVREG; i++)
        J3DNativeState::setTevKColor((GXTevKColorID)i, j3dDefaultTevKColor);

    for (i = 0; i < GX_MAX_TEVREG; i++)
        J3DNativeState::setTevColorS10((GXTevRegID)i, j3dDefaultTevColor);

    for (i = 0; i < GX_MAX_TEVSTAGE; i++) {
        J3DNativeState::setTevColorIn((GXTevStageID)i, GX_CC_RASC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
        J3DNativeState::setTevColorOp((GXTevStageID)i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        J3DNativeState::setTevAlphaIn((GXTevStageID)i, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        J3DNativeState::setTevAlphaOp((GXTevStageID)i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    }

    for (i = 0; i < GX_MAX_TEVSTAGE; i++)
        J3DNativeState::setTevSwapMode((GXTevStageID)i, GX_TEV_SWAP0, GX_TEV_SWAP0);

    J3DNativeState::setTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    J3DNativeState::setTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    J3DNativeState::setTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    J3DNativeState::setTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);

    for (i = 0; i < GX_MAX_TEVSTAGE; i++)
        J3DNativeState::setTevIndirect((GXTevStageID)i, GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE,
                         GX_ITBA_OFF);

>>>>>>> upstream/main

#ifdef __MWERKS__
    asm
        {
        li      r3, 4
        oris    r3, r3, 4
        mtspr   0x392, r3

        li      r3, 5
        oris    r3, r3, 5
        mtspr   0x393, r3

        li      r3, 6
        oris    r3, r3, 6
        mtspr   0x394, r3

        li      r3, 7
        oris    r3, r3, 7
        mtspr   0x395, r3
        }
#endif

<<<<<<< HEAD
    setTexCacheRegion(GX_TEXCACHE_32K);
}

void J3DSys::reinitGX() {
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
    GXSetNumIndStages(0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetCoPlanar(GX_FALSE);
=======
}

void J3DSys::reinitGX() {
    J3DNativeState::setNumChans(0);
    J3DNativeState::setNumTexGens(1);
    J3DNativeState::setNumTevStages(1);
    J3DNativeState::setNumIndStages(0);
    J3DNativeState::setCullMode(GX_CULL_BACK);
    J3DNativeState::setCoPlanar(GX_FALSE);
>>>>>>> upstream/main
    reinitLighting();
    reinitTransform();
    reinitTexture();
    reinitTevStages();
    reinitIndStages();
    reinitPixelProc();
}

void J3DSys::reinitLighting() {
<<<<<<< HEAD
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanAmbColor(GX_COLOR0A0, ColorBlack);
    GXSetChanAmbColor(GX_COLOR1A1, ColorBlack);
    GXSetChanMatColor(GX_COLOR0A0, ColorWhite);
    GXSetChanMatColor(GX_COLOR1A1, ColorWhite);
}

void J3DSys::reinitTransform() {
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD4, GX_TG_MTX2x4, GX_TG_TEX4, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD5, GX_TG_MTX2x4, GX_TG_TEX5, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD6, GX_TG_MTX2x4, GX_TG_TEX6, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD7, GX_TG_MTX2x4, GX_TG_TEX7, GX_IDENTITY, GX_FALSE, 0x7D);
=======
    J3DNativeState::setChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    J3DNativeState::setChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    J3DNativeState::setChanAmbColor(GX_COLOR0A0, ColorBlack);
    J3DNativeState::setChanAmbColor(GX_COLOR1A1, ColorBlack);
    J3DNativeState::setChanMatColor(GX_COLOR0A0, ColorWhite);
    J3DNativeState::setChanMatColor(GX_COLOR1A1, ColorWhite);
}

void J3DSys::reinitTransform() {
    J3DNativeState::setCurrentMtx(GX_PNMTX0);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY, GX_FALSE, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY, GX_FALSE, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY, GX_FALSE, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD4, GX_TG_MTX2x4, GX_TG_TEX4, GX_IDENTITY, GX_FALSE, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD5, GX_TG_MTX2x4, GX_TG_TEX5, GX_IDENTITY, GX_FALSE, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD6, GX_TG_MTX2x4, GX_TG_TEX6, GX_IDENTITY, GX_FALSE, 0x7D);
    J3DNativeState::setTexCoordGen2(GX_TEXCOORD7, GX_TG_MTX2x4, GX_TG_TEX7, GX_IDENTITY, GX_FALSE, 0x7D);
>>>>>>> upstream/main
}

void J3DSys::reinitTexture() {
    GXTexObj texObj;
<<<<<<< HEAD
    GXInitTexObj(&texObj, NullTexData, 4, 4, GX_TF_IA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXLoadTexObj(&texObj, GX_TEXMAP1);
    GXLoadTexObj(&texObj, GX_TEXMAP2);
    GXLoadTexObj(&texObj, GX_TEXMAP3);
    GXLoadTexObj(&texObj, GX_TEXMAP4);
    GXLoadTexObj(&texObj, GX_TEXMAP5);
    GXLoadTexObj(&texObj, GX_TEXMAP6);
    GXLoadTexObj(&texObj, GX_TEXMAP7);
}

void J3DSys::reinitTevStages() {
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE7, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE8, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE9, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE10, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE11, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE12, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE13, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE14, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE15, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

    GXSetTevColor(GX_TEVREG0, ColorWhite);
    GXSetTevColor(GX_TEVREG1, ColorWhite);
    GXSetTevColor(GX_TEVREG2, ColorWhite);
    GXSetTevKColor(GX_KCOLOR0, ColorWhite);
    GXSetTevKColor(GX_KCOLOR1, ColorWhite);
    GXSetTevKColor(GX_KCOLOR2, ColorWhite);
    GXSetTevKColor(GX_KCOLOR3, ColorWhite);
=======
    J3DNativeTexture::registerImage(NullTexData, GX_TF_IA8, 4, 4, 1);
    J3DNativeTexture::init(&texObj, NullTexData, 4, 4, GX_TF_IA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    J3DNativeTexture::load(&texObj, GX_TEXMAP0);
    J3DNativeTexture::load(&texObj, GX_TEXMAP1);
    J3DNativeTexture::load(&texObj, GX_TEXMAP2);
    J3DNativeTexture::load(&texObj, GX_TEXMAP3);
    J3DNativeTexture::load(&texObj, GX_TEXMAP4);
    J3DNativeTexture::load(&texObj, GX_TEXMAP5);
    J3DNativeTexture::load(&texObj, GX_TEXMAP6);
    J3DNativeTexture::load(&texObj, GX_TEXMAP7);
}

void J3DSys::reinitTevStages() {
    J3DNativeState::setTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE6, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE7, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE8, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE9, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE10, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE11, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE12, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE13, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE14, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    J3DNativeState::setTevOrder(GX_TEVSTAGE15, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

    J3DNativeState::setTevColor(GX_TEVREG0, ColorWhite);
    J3DNativeState::setTevColor(GX_TEVREG1, ColorWhite);
    J3DNativeState::setTevColor(GX_TEVREG2, ColorWhite);
    J3DNativeState::setTevKColor(GX_KCOLOR0, ColorWhite);
    J3DNativeState::setTevKColor(GX_KCOLOR1, ColorWhite);
    J3DNativeState::setTevKColor(GX_KCOLOR2, ColorWhite);
    J3DNativeState::setTevKColor(GX_KCOLOR3, ColorWhite);
>>>>>>> upstream/main

    u32 i;
    u32 numStages = GX_MAX_TEVSTAGE;
    for (i = 0; i < numStages; i++) {
<<<<<<< HEAD
        GXSetTevColorIn((GXTevStageID)i, GX_CC_RASC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
        GXSetTevColorOp((GXTevStageID)i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn((GXTevStageID)i, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        GXSetTevAlphaOp((GXTevStageID)i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevKColorSel((GXTevStageID)i, GX_TEV_KCSEL_1_4);
        GXSetTevKAlphaSel((GXTevStageID)i, GX_TEV_KASEL_1);
        GXSetTevSwapMode((GXTevStageID)i, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }

    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
=======
        J3DNativeState::setTevColorIn((GXTevStageID)i, GX_CC_RASC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
        J3DNativeState::setTevColorOp((GXTevStageID)i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        J3DNativeState::setTevAlphaIn((GXTevStageID)i, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        J3DNativeState::setTevAlphaOp((GXTevStageID)i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        J3DNativeState::setTevKColorSel((GXTevStageID)i, GX_TEV_KCSEL_1_4);
        J3DNativeState::setTevKAlphaSel((GXTevStageID)i, GX_TEV_KASEL_1);
        J3DNativeState::setTevSwapMode((GXTevStageID)i, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }

    J3DNativeState::setTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    J3DNativeState::setTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    J3DNativeState::setTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    J3DNativeState::setTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    J3DNativeState::setAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
>>>>>>> upstream/main
}

void J3DSys::reinitIndStages() {
    u32 i;
    for (i = 0; i < GX_MAX_TEVSTAGE; i++) {
<<<<<<< HEAD
        GXSetTevDirect((GXTevStageID)i);
    }

    GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP0);
    GXSetIndTexOrder(GX_INDTEXSTAGE1, GX_TEXCOORD1, GX_TEXMAP1);
    GXSetIndTexOrder(GX_INDTEXSTAGE2, GX_TEXCOORD2, GX_TEXMAP2);
    GXSetIndTexOrder(GX_INDTEXSTAGE3, GX_TEXCOORD3, GX_TEXMAP3);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE1, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE2, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE3, GX_ITS_1, GX_ITS_1);
    GXSetIndTexMtx(GX_ITM_0, IndMtx, 1);
    GXSetIndTexMtx(GX_ITM_1, IndMtx, 1);
    GXSetIndTexMtx(GX_ITM_2, IndMtx, 1);
}

void J3DSys::reinitPixelProc() {
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetColorUpdate(GX_TRUE);
    // SMG1 change -- GX_FALSE to GX_TRUE
    GXSetAlphaUpdate(GX_TRUE);
    GXSetDither(GX_TRUE);
    GXSetFog(GX_FOG_NONE, 0.0f, 1.0f, 0.1f, 1.0f, ColorBlack);
    GXSetFogRangeAdj(GX_FALSE, 0, nullptr);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetZCompLoc(GX_TRUE);
=======
        J3DNativeState::setTevDirect((GXTevStageID)i);
    }

    J3DNativeState::setIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP0);
    J3DNativeState::setIndTexOrder(GX_INDTEXSTAGE1, GX_TEXCOORD1, GX_TEXMAP1);
    J3DNativeState::setIndTexOrder(GX_INDTEXSTAGE2, GX_TEXCOORD2, GX_TEXMAP2);
    J3DNativeState::setIndTexOrder(GX_INDTEXSTAGE3, GX_TEXCOORD3, GX_TEXMAP3);
    J3DNativeState::setIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
    J3DNativeState::setIndTexCoordScale(GX_INDTEXSTAGE1, GX_ITS_1, GX_ITS_1);
    J3DNativeState::setIndTexCoordScale(GX_INDTEXSTAGE2, GX_ITS_1, GX_ITS_1);
    J3DNativeState::setIndTexCoordScale(GX_INDTEXSTAGE3, GX_ITS_1, GX_ITS_1);
    J3DNativeState::setIndTexMtx(GX_ITM_0, IndMtx, 1);
    J3DNativeState::setIndTexMtx(GX_ITM_1, IndMtx, 1);
    J3DNativeState::setIndTexMtx(GX_ITM_2, IndMtx, 1);
}

void J3DSys::reinitPixelProc() {
    J3DNativeState::setBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    J3DNativeState::setColorUpdate(GX_TRUE);
    // SMG1 change -- GX_FALSE to GX_TRUE
    J3DNativeState::setAlphaUpdate(GX_TRUE);
    J3DNativeState::setDither(GX_TRUE);
    J3DNativeState::setFog(GX_FOG_NONE, 0.0f, 1.0f, 0.1f, 1.0f, ColorBlack);
    J3DNativeState::setFogRangeAdj(GX_FALSE, 0, nullptr);
    J3DNativeState::setZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    J3DNativeState::setZCompLoc(GX_TRUE);
>>>>>>> upstream/main
}
