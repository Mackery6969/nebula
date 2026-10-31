#pragma once

#include "Game/System/ArchiveHolder.hpp"
<<<<<<< HEAD
=======
#include "Game/System/NebulaKernel.hpp"
>>>>>>> upstream/main
#include "Game/System/FileHolder.hpp"
#include "Game/System/FileLoaderThread.hpp"
#include <JSystem/JKernel/JKRDvdRipper.hpp>
#include <JSystem/JKernel/JKRHeap.hpp>
#include <cstdio>
<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
>>>>>>> upstream/main

class JKRArchive;

class FileLoader {
public:
    FileLoader();

    void requestLoadToMainRAM(const char*, u8*, JKRHeap*, JKRDvdRipper::EAllocDirection, bool);
    void requestMountArchive(const char*, JKRHeap*, bool);
    bool isLoaded(const char*) const;
    bool isMountedArchive(const char*) const;
    void* receiveFile(const char*);
    JKRMemArchive* receiveArchive(const char*);
    void receiveAllRequestedFile();
    JKRMemArchive* createAndAddArchive(void*, JKRHeap*, const char*);
    void getMountedArchiveAndHeap(const char*, JKRArchive**, JKRHeap**) const;
    void clearRequestFileInfo(bool);
    void removeHolderIfIsEqualHeap(JKRHeap*);
    void removeFile(const char*);
    bool isNeedToLoad(const char*) const;
    const RequestFileInfo* getRequestFileInfoConst(const char*) const;
    RequestFileInfo* addRequest(const char*);

    /* 0x00 */ FileLoaderThread* mLoaderThread;
<<<<<<< HEAD
    /* 0x04 */ OSMutex mMutex;
=======
    /* 0x04 */ NebulaMutex mMutex;
>>>>>>> upstream/main
    /* 0x1C */ RequestFileInfo* mRequestFileInfos;
    /* 0x20 */ s32 mRequestedFileCount;
    /* 0x24 */ FileHolder* mFileHolder;
    /* 0x28 */ ArchiveHolder* mArchiveHolder;
};
