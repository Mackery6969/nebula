#pragma once

<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
>>>>>>> upstream/main

class CameraTargetActor;
class CameraTargetObj;
class CameraTargetPlayer;
class LiveActor;
class MarioActor;

class CameraTargetHolder {
public:
    CameraTargetHolder();
    virtual ~CameraTargetHolder() {
    }

    void movement();
    CameraTargetObj* get();
    void set(CameraTargetObj*);
    void set(const LiveActor*);
    void set(const MarioActor*);
    bool isOnGround() const;
    bool isMoving() const;

    /* 0x4 */ CameraTargetObj* mTarget;
    /* 0x8 */ CameraTargetActor* mTargetActor;
    /* 0xC */ CameraTargetPlayer* mTargetPlayer;
};
