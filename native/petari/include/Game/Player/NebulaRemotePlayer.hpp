#pragma once

#ifdef TARGET_PC
#include "Game/LiveActor/LiveActor.hpp"
#include <nebula_game_api.h>

class XanimePlayer;

// Online co-op (docs/COOP_PLAN.md "Remote players"): another player in this instance, drawn as their Mario or Luigi
// where their snapshots put them, playing the animation they play. It has no physics, collision or state of its own
// (a ghost, Phase 1); its model is the player's own (Mario's or Luigi's, as MarioActor loads it) with Mario's
// animations (MarioAnimator's table), driven as GhostPlayer drives the cosmic ghost's.
class NebulaRemotePlayer : public LiveActor {
public:
    explicit NebulaRemotePlayer(bool isLuigi);

    // GameScene, after the stage's own actors are placed (Mario's animations are ready then): the copies for this
    // scene, as many of each character as the room may have others, when there is a room.
    static void tryCreate();
    // GameScene, as a scene starts: the last scene's copies are gone.
    static void forgetAll();
    // Each game scene frame: the others in this instance as the host has them, each onto a copy of their character
    // (the same one while they stay), the copies nobody uses hidden.
    static void updateAll();

    virtual void init(const JMapInfoIter& rIter);
    virtual void control();
    virtual void calcAndSetBaseMtx();
    virtual void draw() const;

    void show(const NebulaCoopRemotePlayer& rRemote);
    void hide();

    /* 0x90 */ bool mIsLuigi;
    u32 mNumber;  // the player shown (NebulaCoopRemotePlayer::number), 0 for none
    char mName[32];
    XanimePlayer* mXanimePlayer;
    NebulaCoopPlayer mState;  // the latest snapshot shown
    char mGroup[48];          // the animation last started, as the snapshot names it
    char mBck[32];
    TVec3f mFront;
    TVec3f mUp;
};
#endif
