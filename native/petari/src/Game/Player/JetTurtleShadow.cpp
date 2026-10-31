#include "Game/Player/JetTurtleShadow.hpp"
<<<<<<< HEAD
=======
#include "Game/System/NebulaKernel.hpp"
#include <JSystem/J3DGraphBase/J3DNativeView.hpp>
>>>>>>> upstream/main
#include "Game/Player/J3DModelX.hpp"
#include "Game/Util/LiveActorUtil.hpp"
#include "Game/Util/ModelUtil.hpp"
#include "Game/Util/MutexHolder.hpp"

JetTurtleShadow::JetTurtleShadow(const char* pName) : LiveActor(pName) {
}

void JetTurtleShadow::calcType0(MtxPtr pMtx) {
    J3DModel* pModel = MR::getJ3DModel(this);
    MR::setBaseTRMtx(this, pMtx);

<<<<<<< HEAD
    OSLockMutex(&MR::MutexHolder< 0 >::sMutex);
=======
    (MR::MutexHolder< 0 >::sMutex).lock();
>>>>>>> upstream/main

    pModel->calc();
    pModel->viewCalc();

<<<<<<< HEAD
    OSUnlockMutex(&MR::MutexHolder< 0 >::sMutex);
=======
    (MR::MutexHolder< 0 >::sMutex).unlock();
>>>>>>> upstream/main
}

void JetTurtleShadow::drawType0() const {
    J3DModelX* pModel = static_cast< J3DModelX* >(MR::getJ3DModel(this));

    pModel->mFlags.clear();
    pModel->mFlags._14 = true;

<<<<<<< HEAD
    GXSetZScaleOffset(1.0f, 0.0f);
=======
    J3DNativeView::setZScaleOffset(1.0f, 0.0f);
>>>>>>> upstream/main

    pModel->directDraw(nullptr);
}

void JetTurtleShadow::drawType1() const {
    J3DModelX* pModel = static_cast< J3DModelX* >(MR::getJ3DModel(this));

    pModel->mFlags.clear();
    pModel->mFlags._19 = true;

    pModel->directDraw(nullptr);
}

void JetTurtleShadow::init(const JMapInfoIter& rIter) {
    initModelManagerWithAnm("Koura", nullptr, false);

    makeActorDead();
}
