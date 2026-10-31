#pragma once

<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
>>>>>>> upstream/main

class J3DMaterial;
class J3DShapePacket;
class J3DModel;

class ShapePacketUserData {
public:
    ShapePacketUserData();

    void init(J3DMaterial* pMaterial);
<<<<<<< HEAD
    void callDL() const;
=======
    // The texture coordinates and current matrices its material generates differently, for the next draw.
    void load() const;
>>>>>>> upstream/main
    void loadTexMtx(J3DMaterial* pMaterial, int slot, u16 index) const;

    struct TexMtxInfo {
        TexMtxInfo() : mType(), mPostMtx(GX_PTIDENTITY) {
        }

        /* 0x00 */ s32 mType;
        /* 0x04 */ u32 mPostMtx;
    };

    /* 0x00 */ TexMtxInfo mTexMtxInfo[8];
    /* 0x40 */ u32 mTexGenNum;
<<<<<<< HEAD
    /* 0x44 */ u32 mDisplayListSize;
    /* 0x48 */ u8* mDisplayList;
=======
    bool mEnvMap;
    u8 mTexGenType[8];
    u8 mTexGenSource[8];
>>>>>>> upstream/main
};

namespace MR {
    ShapePacketUserData* getJ3DShapePacketUserData(const J3DShapePacket* pShapePacket);
    void initJ3DShapePacketUserData(J3DModel* pModel);
};  // namespace MR
