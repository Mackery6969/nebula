#pragma once

#include "Game/Screen/LayoutActor.hpp"

class CoinCounter;
class MarioMeter;
<<<<<<< HEAD
#ifdef TARGET_PC
class NebulaStarCounter;
#endif
=======
>>>>>>> upstream/main
class PlayerLeft;
class StarCounter;
class StarPieceCounter;

class CounterLayoutController : public LayoutActor {
public:
    /// @brief Creates a new `CounterLayoutController`.
    CounterLayoutController();

    virtual void init(const JMapInfoIter& rIter);
    virtual void appear();
    virtual void kill();
    virtual void control();

    void forceAppear();
    void setHPMeter(MarioMeter*);
    void requestedTicoEat(bool);
    static bool isInvalidSystemStateShowCounter();
    static bool isWaitToDisappearCounter(const LayoutActor*);
    void showAllLayout();
    void hideAllLayout();
    bool isHiddenOrWaitAllLayout() const;
    bool isPlayerMoving() const;
    void appearAllCounterWithoutStar();
    void killAllCoounter();
    bool tryEndTicoEatStarPiece();
    void exePlayerMoving();
    void exePlayerNotMoving();

    /* 0x20 */ s32 mPlayerNotMovingFrame;
    /* 0x24 */ bool _24;
    /* 0x28 */ CoinCounter* mCoinCounter;
    /* 0x2C */ StarPieceCounter* mStarPieceCounter;
    /* 0x30 */ PlayerLeft* mPlayerLeft;
    /* 0x34 */ StarCounter* mStarCounter;
    /* 0x38 */ MarioMeter* mHPMeter;
<<<<<<< HEAD
#ifdef TARGET_PC
    // The Personal Star Counter's two under the save's star counter (this room's, every star's): shown and hidden
    // with it.
    NebulaStarCounter* mRoomStarCounter;
    NebulaStarCounter* mAllTimeStarCounter;
#endif
=======
>>>>>>> upstream/main
};
