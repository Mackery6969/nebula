#pragma once

<<<<<<< HEAD
#include <revolution.h>

class NANDManagerThread;
class NANDRequestInfo;

typedef void(NANDReqFunc)(NANDRequestInfo*);

=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>

class NANDRequestInfo;

// One request on the game's save files: set up by one of the set* calls, started by MR::addRequestToNANDManager, done
// when the host has finished it (the game polls isDone). On the console a save thread ran each as NAND calls; the
// host's save service is asynchronous itself (save_request).
>>>>>>> upstream/main
class NANDRequestInfo {
public:
    NANDRequestInfo();

    void init();
    bool isDone() const;
<<<<<<< HEAD
    const char* setMove(const char*, const char*);
=======
    // The result the save sequences interpret (NAND_RESULT_*), once done.
    s32 getResult() const;
    void start();
>>>>>>> upstream/main
    const char* setWriteSeq(const char*, const void*, u32, u8, u8);
    const char* setReadSeq(const char*, void*, u32, u32*);
    NANDRequestInfo* setCheck(u32, u32, u32*);
    NANDRequestInfo* setDelete(const char*);

<<<<<<< HEAD
    /* 0x00 */ char mPath[NAND_MAX_PATH];
    /* 0x40 */ u32 _40;
    /* 0x44 */ s32 mType;
    /* 0x48 */ s32 mResult;
    /* 0x4C */ union {
        void* mReadBuf;
        const void* mWriteBuf;
    };
    /* 0x50 */ union {
        const char* mMoveDestDir;
        u32* mReadLength;
        u32* mCheckAnswer;
    };
    /* 0x54 */ NANDReqFunc* _54;
    /* 0x58 */ u8 mPermission;
    /* 0x59 */ u8 mAttribute;
    /* 0x5C */ u32 mFsBlock;
    /* 0x60 */ u32 mInode;
    /* 0x64 */ u8 _64[0x1C];
};

class NANDManager {
public:
    NANDManager();

    bool addRequest(NANDRequestInfo*);

private:
    /* 0x00 */ OSMutex mMutex;
    /* 0x18 */ NANDManagerThread* mManagerThread;
=======
private:
    enum Type {
        Type_Check,
        Type_Delete,
        Type_Write,
        Type_Read,
    };

    char mName[64];
    Type mType;
    void* mBuffer;
    u32 mSize;           // a write's length, a read's capacity
    u32* mReadLength;    // a read: the file's length
    u32* mCheckAnswer;   // a check: 0, room to create the save
    volatile s32 mStatus;  // NEBULA_SAVE_*: the host writes it from its own thread
>>>>>>> upstream/main
};

class NANDResultCode {
public:
    NANDResultCode(s32 code) : mCode(code) {
    }

    s32 getCode() const;
    bool isSuccess() const;
    bool isSaveDataCorrupted() const;
    bool isNANDCorrupted() const;
    bool isMaxBlocks() const;
    bool isMaxFiles() const;
    bool isNoExistFile() const;
    bool isBusyOrAllocFailed() const;
    bool isUnknown() const;

private:
    /* 0x0 */ s32 mCode;
};

namespace MR {
    void addRequestToNANDManager(NANDRequestInfo*);
};  // namespace MR
