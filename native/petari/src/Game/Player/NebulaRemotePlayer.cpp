#ifdef TARGET_PC
#include "Game/Player/NebulaRemotePlayer.hpp"
#include "Game/Animation/XanimeCore.hpp"
#include "Game/Animation/XanimePlayer.hpp"
#include "Game/LiveActor/ModelManager.hpp"
#include "Game/Player/J3DModelX.hpp"
#include "Game/Player/MarioActor.hpp"
#include "Game/Player/MarioHolder.hpp"
#include "Game/Scene/SceneFunction.hpp"
#include "Game/Scene/SceneObjHolder.hpp"
#include "Game/Util/ActorShadowUtil.hpp"
#include "Game/Util/LiveActorUtil.hpp"
#include "Game/Util/ModelUtil.hpp"
#include "Game/Util/MtxUtil.hpp"
#include "Game/Util/ObjUtil.hpp"
#include "Game/Util/PlayerUtil.hpp"
#include <cstdio>
#include <cstring>
#include <revolution/os.h>

namespace {
    // Copies of each character a scene keeps: one for each other player a room of 16 may have.
    const s32 cMaxPerCharacter = 15;
    NebulaRemotePlayer* sCopies[cMaxPerCharacter * 2];
    s32 sCopyNum = 0;

    bool isUnit(const f32* pVec) {
        const f32 length = pVec[0] * pVec[0] + pVec[1] * pVec[1] + pVec[2] * pVec[2];
        return length > 0.25f && length < 4.0f;
    }
};  // namespace

NebulaRemotePlayer::NebulaRemotePlayer(bool isLuigi)
    : LiveActor(isLuigi ? "NebulaRemoteLuigi" : "NebulaRemoteMario"), mIsLuigi(isLuigi), mNumber(0), mXanimePlayer(nullptr),
      mFront(0.0f, 0.0f, 1.0f), mUp(0.0f, 1.0f, 0.0f) {
    mName[0] = '\0';
    mGroup[0] = '\0';
    mBck[0] = '\0';
    memset(&mState, 0, sizeof(mState));
}

void NebulaRemotePlayer::forgetAll() {
    sCopyNum = 0;
}

void NebulaRemotePlayer::tryCreate() {
    sCopyNum = 0;
    u32 roomSize = 0;
    __NebulaCoopList(nullptr, 0, &roomSize);
    if (roomSize < 2) {
        return;
    }
    // Mario's animations, which the copies play, are the local player's.
    if (!MR::isExistSceneObj(SceneObj_MarioHolder) || MR::getMarioHolder()->getMarioActor() == nullptr) {
        return;
    }
    s32 perCharacter = static_cast< s32 >(roomSize) - 1;
    if (perCharacter > cMaxPerCharacter) {
        perCharacter = cMaxPerCharacter;
    }
    for (s32 i = 0; i < perCharacter * 2; i++) {
        NebulaRemotePlayer* pCopy = new NebulaRemotePlayer(i >= perCharacter);
        pCopy->initWithoutIter();
        sCopies[sCopyNum++] = pCopy;
    }
    OSReport("[coop] %d copies for the other players\n", sCopyNum);
}

s32 NebulaRemotePlayer::getCopyNum() {
    return sCopyNum;
}

const NebulaRemotePlayer* NebulaRemotePlayer::getCopy(s32 index) {
    return index >= 0 && index < sCopyNum ? sCopies[index] : nullptr;
}

TVec3f NebulaRemotePlayer::getNametagPos() const {
    // Mario stands about 160 tall: the name a little over his cap.
    return TVec3f(mPosition.x + mUp.x * 200.0f, mPosition.y + mUp.y * 200.0f, mPosition.z + mUp.z * 200.0f);
}

void NebulaRemotePlayer::updateAll() {
    if (sCopyNum == 0) {
        return;
    }
    NebulaCoopRemotePlayer remote[cMaxPerCharacter * 2];
    u32 remoteNum = __NebulaCoopRemote(remote, cMaxPerCharacter * 2);
    if (remoteNum > static_cast< u32 >(cMaxPerCharacter * 2)) {
        remoteNum = cMaxPerCharacter * 2;
    }
    bool placed[cMaxPerCharacter * 2] = {};
    // The copies showing someone still here keep them.
    for (s32 i = 0; i < sCopyNum; i++) {
        NebulaRemotePlayer* pCopy = sCopies[i];
        bool kept = false;
        for (u32 j = 0; j < remoteNum && pCopy->mNumber != 0; j++) {
            const bool isLuigi = (remote[j].player.flags & NEBULA_COOP_PLAYER_LUIGI) != 0;
            if (!placed[j] && remote[j].number == pCopy->mNumber && isLuigi == pCopy->mIsLuigi) {
                pCopy->show(remote[j]);
                placed[j] = true;
                kept = true;
                break;
            }
        }
        if (!kept) {
            pCopy->hide();
        }
    }
    // The rest onto free copies of their character.
    for (u32 j = 0; j < remoteNum; j++) {
        if (placed[j]) {
            continue;
        }
        const bool isLuigi = (remote[j].player.flags & NEBULA_COOP_PLAYER_LUIGI) != 0;
        for (s32 i = 0; i < sCopyNum; i++) {
            if (sCopies[i]->mNumber == 0 && sCopies[i]->mIsLuigi == isLuigi) {
                sCopies[i]->show(remote[j]);
                break;
            }
        }
    }
}

void NebulaRemotePlayer::init(const JMapInfoIter& rIter) {
    // The player's own model: the local player's is already here; the other character's loads into the scene.
    initModelManagerWithAnm(mIsLuigi ? "Luigi" : "Mario", nullptr, false);
    MR::connectToScene(this, MR::MovementType_Player, MR::CalcAnimType_Player, MR::DrawBufferType_None, MR::DrawType_Player);

    // Mario's animations, as GhostPlayer plays them on the cosmic ghost.
    mXanimePlayer = new XanimePlayer(MR::getJ3DModel(this), MR::getPlayerXanimeResource());
    mXanimePlayer->duplicateSimpleGroup();
    mModelManager->mXanimePlayer = mXanimePlayer;
    mXanimePlayer->setDefaultAnimation("\212\356\226\173" /* 基本 */);
    mXanimePlayer->getCore()->enableJointTransform(MR::getJ3DModelData(this));
    mXanimePlayer->changeAnimation("\212\356\226\173" /* 基本 */);

    MR::initShadowVolumeSphere(this, 50.0f);
    MR::onCalcShadow(this, nullptr);
    MR::onCalcShadowDropGravity(this, nullptr);
    MR::invalidateClipping(this);
    makeActorDead();
}

void NebulaRemotePlayer::show(const NebulaCoopRemotePlayer& rRemote) {
    const bool isNew = mNumber != rRemote.number;
    mNumber = rRemote.number;
    snprintf(mName, sizeof(mName), "%s", rRemote.name);
    mState = rRemote.player;
    mState.group[sizeof(mState.group) - 1] = '\0';
    mState.animation[sizeof(mState.animation) - 1] = '\0';
    mPosition.set(mState.position[0], mState.position[1], mState.position[2]);
    mRotation.set(mState.rotation[0], mState.rotation[1], mState.rotation[2]);
    mVelocity.set(mState.velocity[0], mState.velocity[1], mState.velocity[2]);
    if (isUnit(mState.gravity)) {
        mGravity.set(mState.gravity[0], mState.gravity[1], mState.gravity[2]);
    }
    if (isUnit(mState.front) && isUnit(mState.up)) {
        mFront.set(mState.front[0], mState.front[1], mState.front[2]);
        mUp.set(mState.up[0], mState.up[1], mState.up[2]);
    }
    if (isNew) {
        // Another player on this copy: their animation starts afresh.
        mGroup[0] = '\0';
        mBck[0] = '\0';
    }
    if (MR::isDead(this)) {
        appear();
    }
}

void NebulaRemotePlayer::hide() {
    mNumber = 0;
    mName[0] = '\0';
    if (!MR::isDead(this)) {
        makeActorDead();
    }
}

void NebulaRemotePlayer::control() {
    if (mXanimePlayer == nullptr) {
        return;
    }
    // A new animation as the snapshot names it: its group, else (Mario's code played a bck of its own) the bck.
    if (strcmp(mGroup, mState.group) != 0 || strcmp(mBck, mState.animation) != 0) {
        snprintf(mGroup, sizeof(mGroup), "%s", mState.group);
        snprintf(mBck, sizeof(mBck), "%s", mState.animation);
        if (mGroup[0] != '\0') {
            mXanimePlayer->changeAnimation(mGroup);
        }
        const char* pPlaying = mXanimePlayer->getCurrentBckName();
        if (mBck[0] != '\0' && (pPlaying == nullptr || strcmp(pPlaying, mBck) != 0)) {
            mXanimePlayer->changeSimpleBck(mBck);
        }
    }
    for (u32 i = 0; i < 4; i++) {
        mXanimePlayer->changeTrackWeight(i, mState.track_weights[i]);
    }
    if (mXanimePlayer->getFrameCtrl() != nullptr) {
        MR::setBckRate(this, mState.animation_rate);
        MR::getBckCtrl(this)->setFrame(mState.animation_frame);
    }
}

void NebulaRemotePlayer::calcAndSetBaseMtx() {
    TPos3f mtx;
    MR::makeMtxUpFrontPos(&mtx, mUp, mFront, mPosition);
    MR::setBaseTRMtx(this, mtx);
    MR::getJ3DModel(this)->setBaseScale(mScale);
}

void NebulaRemotePlayer::draw() const {
    if (MR::isDead(this)) {
        return;
    }
    J3DModelX* pModel = reinterpret_cast< J3DModelX* >(MR::getJ3DModel(this));
    pModel->viewCalc2();
    GXInvalidateVtxCache();
    pModel->mFlags.clear();
    pModel->setDynamicDL(nullptr, 0);
    pModel->directDraw(nullptr);
}
#endif
