#pragma once

#include "Game/NameObj/NameObj.hpp"

class ScenePlayingResult : public NameObj {
public:
    /// @brief Creates a new `ScenePlayingResult`.
    ScenePlayingResult();

    /// @brief Destroys the `ScenePlayingResult`.
    virtual ~ScenePlayingResult();

    /// @brief Adds to the number of coins by the given term.
    /// @param num The number of coins to add.
    void incCoin(int num);

    /// @brief Returns the number of coins.
    /// @return The number of coins.
    s32 getCoinNum() const;

    /// @brief Adds to the number of Purple Coins by one.
    void incPurpleCoin();

<<<<<<< HEAD
#ifdef TARGET_PC
    /// @brief Online co-op: adds the coins another player in this instance took (sm64coopdx's collect_coin): the
    /// count only, without the life, the 1-Up or the player's own statistics.
    void incCoinOfOtherPlayer(int num);
#endif

=======
>>>>>>> upstream/main
    /// @brief Adds to the number of Star Bits by the given term.
    /// @param num The number of Star Bits to add.
    void incStarPiece(int num);

    /// @brief Returns the number of Star Bits.
    /// @return The number of Star Bits.
    s32 getStarPieceNum() const;

    /// @brief The number of coins.
    /* 0x0C */ s32 mCoinNum;

    /// @brief The number of Purple Coins.
    /* 0x10 */ s32 mPurpleCoinNum;

    /* 0x14 */ u32 _14;
};
