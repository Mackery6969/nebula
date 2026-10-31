#include "JSystem/JKernel/JKRDvdFile.hpp"
<<<<<<< HEAD
#include "JSystem/JUtility/JUTException.hpp"
#include <stdint.h>
=======
#include "Game/System/NebulaKernel.hpp"
>>>>>>> upstream/main

JKRDvdFile::JKRDvdFile() : mDvdLink(this) {
    initiate();
}

JKRDvdFile::JKRDvdFile(s32 entryNum) : mDvdLink(this) {
    initiate();
    mIsAvailable = open(entryNum);
    if (!mIsAvailable) {
        return;
    } else {
        return;
    }
}

JKRDvdFile::~JKRDvdFile() {
    close();
}

void JKRDvdFile::initiate(void) {
<<<<<<< HEAD
    mDvdFile = this;
    OSInitMutex(&mMutex1);
    OSInitMutex(&mMutex2);
    OSInitMessageQueue(&mMessageQueue2, &mMessage2, 1);
    OSInitMessageQueue(&mMessageQueue1, &mMessage1, 1);
    mOSThread = NULL;
    field_0x50 = 0;
=======
    mMutex1.init();
    mMutex2.init();
    field_0x50 = NULL;
>>>>>>> upstream/main
    field_0x58 = 0;
}

bool JKRDvdFile::open(const char* name) {
    if (!mIsAvailable) {
<<<<<<< HEAD
        mIsAvailable = DVDOpen(name, &mFileInfo);
        if (mIsAvailable) {
            sDvdList.append(&mDvdLink);
            getStatus();
=======
        mIsAvailable = NebulaDisc::open(name, &mFile);
        if (mIsAvailable) {
            sDvdList.append(&mDvdLink);
>>>>>>> upstream/main
        }
    }
    return mIsAvailable;
}

bool JKRDvdFile::open(s32 entryNum) {
    if (!mIsAvailable) {
<<<<<<< HEAD
        mIsAvailable = DVDFastOpen(entryNum, &mFileInfo);
        if (mIsAvailable) {
            sDvdList.append(&mDvdLink);
            getStatus();
=======
        mIsAvailable = NebulaDisc::open(entryNum, &mFile);
        if (mIsAvailable) {
            sDvdList.append(&mDvdLink);
>>>>>>> upstream/main
        }
    }
    return mIsAvailable;
}

<<<<<<< HEAD
void JKRDvdFile::close() {
    if (mIsAvailable) {
        if (DVDClose(&mFileInfo) != 0) {
            mIsAvailable = false;
            sDvdList.remove(&mDvdLink);
        } else {
            JUTException::panic(__FILE__, 213, "cannot close DVD file\n");
        }
    }
}

s32 JKRDvdFile::readData(void* param_1, s32 length, s32 param_3) {
    OSLockMutex(&mMutex1);
    if (mOSThread) {
        OSUnlockMutex(&mMutex1);
        return -1;
    }

    mOSThread = OSGetCurrentThread();

    s32 result = -1;
    if (DVDReadAsyncPrio(&mFileInfo, param_1, length, param_3, JKRDvdFile::doneProcess, 2)) {
        result = sync();
    }

    mOSThread = NULL;
    OSUnlockMutex(&mMutex1);

=======
// A disc file holds nothing on the host: closing it only takes it off the list.
void JKRDvdFile::close() {
    if (mIsAvailable) {
        mIsAvailable = false;
        sDvdList.remove(&mDvdLink);
    }
}

// One reader at a time; it sleeps while the host reads (NebulaDisc::read).
s32 JKRDvdFile::readData(void* pDst, s32 length, s32 offset) {
    mMutex1.lock();
    const s32 result = NebulaDisc::read(&mFile, pDst, length, offset);
    mMutex1.unlock();
>>>>>>> upstream/main
    return result;
}

s32 JKRDvdFile::writeData(void const* param_0, s32 length, s32 param_2) {
    return -1;
}

<<<<<<< HEAD
s32 JKRDvdFile::sync(void) {
    OSMessage message;
    OSLockMutex(&mMutex1);
    OSReceiveMessage(&mMessageQueue2, &message, 1);
    mOSThread = NULL;
    OSUnlockMutex(&mMutex1);
    return (intptr_t)message;
}

void JKRDvdFile::doneProcess(s32 id, DVDFileInfo* fileInfo) {
#ifdef TARGET_PC
    // mDvdFile follows mFileInfo; 0x3C was the console's sizeof(DVDFileInfo).
    JKRDvdFile* dvdFile = *(JKRDvdFile**)((u8*)fileInfo - offsetof(JKRDvdFile, mFileInfo) + offsetof(JKRDvdFile, mDvdFile));
#else
    JKRDvdFile* dvdFile = *(JKRDvdFile**)((u8*)fileInfo + 0x3c);
#endif
    OSSendMessage(&dvdFile->mMessageQueue2, (OSMessage)(intptr_t)id, OS_MESSAGE_NOBLOCK);
}

=======
>>>>>>> upstream/main
JSUList< JKRDvdFile > JKRDvdFile::sDvdList;
