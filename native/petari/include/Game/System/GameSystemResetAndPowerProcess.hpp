#pragma once

#include "Game/Screen/LayoutActor.hpp"

<<<<<<< HEAD
class TriggerChecker;
=======
>>>>>>> upstream/main
class ValueControl;

class GameSystemResetAndPowerProcess : public LayoutActor {
public:
    enum ResetOperation {
        ResetOperation_ApplicationReset,
        ResetOperation_Restart,
        ResetOperation_ReturnToMenu,
<<<<<<< HEAD
        ResetOperation_ShutdownSystem,
        ResetOperation_RebootSystem,
=======
>>>>>>> upstream/main
    };

    /// @brief Creates a new `GameSystemResetAndPowerProcess`.
    GameSystemResetAndPowerProcess();

    virtual ~GameSystemResetAndPowerProcess();

    virtual void init(const JMapInfoIter&);
    virtual void draw() const;
    virtual void control();

    bool isActive() const NO_INLINE;
    void setResetOperationApplicationReset();
    void setResetOperationReturnToMenu();
    void requestReset(bool);
    void requestGoWiiMenu(bool);
    void notifyCheckDiskResult(bool);
    void exePolling();
    void exeWaitResetPermitted();
    void exePrepareReset();
    void exeReset();
    void exeWaitPrepareFadein();
    void exeFadein();
    void exitApplication();
    bool tryPermitReset() NO_INLINE;
<<<<<<< HEAD
    bool tryAcceptPowerOff();
    bool isResetAcceptAudio() const;
    static void handleOSPowerCallback();
    static void handleCheckDiskAsync(s32, DVDCommandBlock*);

    /* 0x20 */ TriggerChecker* mResetTriggerChecker;
    /* 0x24 */ ValueControl* mFadeinoutControl;
    /* 0x28 */ DVDCommandBlock mCommandBlock;
    /* 0x58 */ volatile s32 mResetOperation;
    /* 0x5C */ bool _5C;
    /* 0x5D */ volatile bool mIsValidPowerOff;
=======
    bool isResetAcceptAudio() const;

    /* 0x24 */ ValueControl* mFadeinoutControl;
    /* 0x58 */ volatile s32 mResetOperation;
    /* 0x5C */ bool _5C;
>>>>>>> upstream/main
    /* 0x5E */ bool _5E;
};
