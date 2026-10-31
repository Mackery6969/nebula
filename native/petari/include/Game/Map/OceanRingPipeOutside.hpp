#pragma once

#include "Game/LiveActor/LiveActor.hpp"
<<<<<<< HEAD
=======
#include "Game/Util/NativeMesh.hpp"
>>>>>>> upstream/main

class OceanRingPipe;
class JUTTexture;

class OceanRingPipeOutside : public LiveActor {
public:
    OceanRingPipeOutside(const OceanRingPipe*);

    virtual ~OceanRingPipeOutside();
    virtual void init(const JMapInfoIter&);
    virtual void movement();
    virtual void draw() const;

    void initDisplayList();
    void loadMaterial() const;
<<<<<<< HEAD
    void sendGD() const;
=======
    void sendGD();
>>>>>>> upstream/main

    /* 0x8C */ const OceanRingPipe* mRingPipe;
    /* 0x90 */ f32 mTexU;
    /* 0x94 */ JUTTexture* mWaterPipeIndirectTex;
    /* 0x98 */ JUTTexture* mWaterPipeHighLightTex;
<<<<<<< HEAD
    /* 0x9C */ u32 mDispListLength;
    /* 0xA0 */ u8* mDispList;
=======
    // The pipe's surface as the renderer draws it.
    NativeMesh mMesh;
>>>>>>> upstream/main
};