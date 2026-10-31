#pragma once

#include "Game/LiveActor/LiveActor.hpp"
<<<<<<< HEAD
#include <JSystem/JGeometry.hpp>

typedef void (*PosAttrFunc)(f32, f32, f32);
typedef void (*TexAttrFunc)(f32, f32);

=======
#include "Game/Util/NativeMesh.hpp"
#include <JSystem/JGeometry.hpp>

>>>>>>> upstream/main
class ElectricRailMovingPoint : public LiveActor {
public:
    ElectricRailMovingPoint(const char* pName = "\210\332\223\256\223\144\214\202\203\214\201\133\203\213\223\137" /* 移動電撃レール点 */);

    virtual ~ElectricRailMovingPoint();
    virtual void init(const JMapInfoIter&);
    virtual void attackSensor(HitSensor*, HitSensor*);

    u8 _8C;
    u8 _8D;
};

class ElectricRailMoving : public LiveActor {
public:
    ElectricRailMoving(const char*);

    virtual ~ElectricRailMoving();
    virtual void init(const JMapInfoIter&);
    virtual void draw() const;
    virtual void attackSensor(HitSensor*, HitSensor*);

    void disappear();
    bool isTouchRail(const HitSensor*, TVec3f*, TVec3f*) const;
    void initMapToolInfo(const JMapInfoIter&);
    void initSensor();
    void initRail(const JMapInfoIter&);
    void initPoints();
    void initDisplayList();
    void drawRailGX(f32) const;
<<<<<<< HEAD
    void drawPlane(f32, f32, f32, f32) const;
    void drawPlaneGX(f32, f32, f32, f32) const;
    void setVertexAttribute(int, int, f32, f32, f32, f32, PosAttrFunc, TexAttrFunc) const;
=======
    void drawPlane(NativeMesh&, f32, f32, f32, f32) const;
    void drawPlaneGX(f32, f32, f32, f32) const;
    // A segment's two corners into `pMesh`, or drawn now when it is null.
    void setVertexAttribute(int, int, f32, f32, f32, f32, NativeMesh* pMesh) const;
>>>>>>> upstream/main
    void updateHitSensorPos();
    bool updatePointPos();
    void updatePointPosAndModel();
    bool isValidCoord(f32) const;
    f32 getRepeatedCoord(f32) const;
    void calcPointPos(TVec3f*, f32) const;
    void move();
    void exeDisappear();
    void exeDisappeared();
    void exeWait();

    /* 0x8C */ s32 mSegmentNum;
    /* 0x90 */ f32 mMovementSpeed;
    /* 0x94 */ f32 mSegmentLength;
    /* 0x98 */ s32 mStackHeight;
    TPos3f _9C;
    u8 _CC;
    u8 _CD;
    u8 _CE;
    u8 _CF;
    /* 0xD0 */ TVec3f* mSensorOffsets;
    /* 0xD4 */ ElectricRailMovingPoint* mMovingPoints;
    s32 _D8;
<<<<<<< HEAD
    void* _DC;
    u32 _E0;
=======
    // The rail as the renderer draws it while it waits.
    NativeMesh mMesh;
>>>>>>> upstream/main
    s32 _E4;
    TVec3f* _E8;
    f32 _EC;
    f32 _F0;
};
