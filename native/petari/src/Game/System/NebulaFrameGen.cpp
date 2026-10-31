#include "Game/System/NebulaFrameGen.hpp"

#ifdef TARGET_PC
#include <nebula_game_api.h>
#include <revolution/os.h>
<<<<<<< HEAD

namespace {
    // When a full-screen menu, page or window was last up (OSGetTime; 0 never).
    OSTime sFullscreenUiTime = 0;
=======
#include "Game/System/NebulaClock.hpp"
#include "Game/System/NebulaHost.hpp"

namespace {
    // When a full-screen menu, page or window was last up (NebulaClock; 0 never).
    s64 sFullscreenUiTime = 0;
>>>>>>> upstream/main
    // A menu noted within this many milliseconds holds generation off (its notes come every frame).
    const s32 cFullscreenUiHoldMs = 50;
};  // namespace

namespace NebulaFrameGen {
    void noteFullscreenUi() {
<<<<<<< HEAD
        sFullscreenUiTime = OSGetTime();
    }

    void report(bool playing) {
        const bool menu = sFullscreenUiTime != 0 && OSTicksToMilliseconds(OSGetTime() - sFullscreenUiTime) < cFullscreenUiHoldMs;
        __NebulaGameContext(NEBULA_CONTEXT_FRAME_GENERATION, playing && !menu ? 1 : 0);
=======
        sFullscreenUiTime = NebulaClock::nowNs();
    }

    void report(bool playing) {
        const bool menu = sFullscreenUiTime != 0 && (NebulaClock::nowNs() - sFullscreenUiTime) / 1000000 < cFullscreenUiHoldMs;
        NebulaHost::gameContext(NEBULA_CONTEXT_FRAME_GENERATION, playing && !menu ? 1 : 0);
>>>>>>> upstream/main
    }
};  // namespace NebulaFrameGen
#endif
