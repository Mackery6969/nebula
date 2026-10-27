#pragma once

#include "Game/Screen/SimpleLayout.hpp"
#include "Game/System/NerveExecutor.hpp"

class EncouragePal60Window;
class TriggerChecker;

namespace TitleSequenceProductSub {
    class LogoLayout;
};  // namespace TitleSequenceProductSub

class TitleSequenceProduct : public NerveExecutor {
public:
    /// @brief Creates a new `TitleSequenceProduct`.
    TitleSequenceProduct();

    void appear();
    void kill();
    bool isActive() const;
    void updateButtonReaction(TriggerChecker* pButtonChecker, const char* pAnimName);
    void updatePressStartReaction();
    void exeDisplayEncouragePal60Window();
    void exeBgmPrepare();
    void exeLogoFadein();
    void exeLogoWait();
    void exeLogoDisplay();
    void exeDecide();
    void exeDead();
#ifdef TARGET_PC
    // The logo alone over the co-op main menu, as the title shows it (no music, no Press A and B): it appears, then
    // waits with its lights; updateLogo each frame while it shows.
    void appearLogo();
    void updateLogo();
    void killLogo();
    LayoutActor* getLogo() const;
#endif

private:
    /* 0x08 */ TitleSequenceProductSub::LogoLayout* mLogoLayout;
    /* 0x0C */ SimpleLayout* mPressStartLayout;
    /* 0x10 */ EncouragePal60Window* mEncouragePal60Window;
    /* 0x14 */ TriggerChecker* mAButtonChecker;
    /* 0x18 */ TriggerChecker* mBButtonChecker;
    /* 0x1C */ bool mIsDisplayEncouragePal60Window;
#ifdef TARGET_PC
    bool mIsLogoWaiting;  // the main menu's logo past its Appear
#endif
};

namespace TitleSequenceProductSub {
    class LogoLayout : public SimpleLayout {
    public:
        /// @brief Creates a new `LogoLayout`.
        LogoLayout();
    };
};  // namespace TitleSequenceProductSub
