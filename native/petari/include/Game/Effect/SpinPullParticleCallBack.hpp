#pragma once

#include "Game/Effect/MultiEmitterParticleCallBack.hpp"
<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
>>>>>>> upstream/main

class SpinPullParticleCallBack : public MultiEmitterParticleCallBack {
public:
    SpinPullParticleCallBack();

    virtual void execute(JPABaseEmitter*, JPABaseParticle*);

    f32 _4;
};

class MerameraParticleCallBack : public SpinPullParticleCallBack {
public:
    virtual void execute(JPABaseEmitter*, JPABaseParticle*);
};