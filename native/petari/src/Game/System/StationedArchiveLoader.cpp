#include "Game/System/StationedArchiveLoader.hpp"
#include "Game/System/ResourceHolderManager.hpp"
#include "Game/System/StationedFileInfo.hpp"
#include "Game/Util/FileUtil.hpp"
#include "Game/Util/MemoryUtil.hpp"
#include "Game/Util/SingletonHolder.hpp"
#include <JSystem/JKernel/JKRExpHeap.hpp>
<<<<<<< HEAD
=======
#include "Game/System/NebulaDisc.hpp"
>>>>>>> upstream/main

StationedArchiveLoader::Condition::~Condition() {
}

JKRHeap* StationedArchiveLoader::Condition::getProperHeap(const MR::StationedFileInfo* pInfo) const {
    return nullptr;
}

JKRHeap* StationedArchiveLoader::getProperHeap(const MR::StationedFileInfo* pInfo) {
    switch (pInfo->mHeapType) {
    case MR::StationedFileInfo::HEAP_TYPE_NAPA:
        return MR::getStationedHeapNapa();
    case MR::StationedFileInfo::HEAP_TYPE_GDDR:
        return MR::getStationedHeapGDDR3();
    default:
        return nullptr;
    }
}

void StationedArchiveLoader::loadResourcesFromTable(const StationedArchiveLoader::Condition& rCondition) {
    for (const MR::StationedFileInfo* pInfo = MR::getStationedFileInfoTable(); pInfo->mArchive != nullptr; pInfo++) {
        if (!rCondition.isExecute(pInfo)) {
            continue;
        }

        JKRHeap* pHeap = rCondition.getProperHeap(pInfo);

        if (pHeap == nullptr) {
            pHeap = StationedArchiveLoader::getProperHeap(pInfo);
        }

        switch (pInfo->mLoadType) {
        case MR::StationedFileInfo::LOAD_TYPE_FILE:
            MR::loadToMainRAM(pInfo->mArchive, nullptr, pHeap, JKRDvdRipper::ALLOC_DIRECTION_FORWARD);
            break;
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT:
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT_RESOURCE_MARIO:
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT_RESOURCE_LUIGI:
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT_RESOURCE:
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT_RESOURCE_LAYOUT:
            MR::mountArchive(pInfo->mArchive, pHeap);
            break;
        }
    }
}

void StationedArchiveLoader::createAndAddResourcesFromTable(const Condition& rCondition) {
    for (const MR::StationedFileInfo* pInfo = MR::getStationedFileInfoTable(); pInfo->mArchive != nullptr; pInfo++) {
        if (!rCondition.isExecute(pInfo)) {
            continue;
        }

        switch (pInfo->mLoadType) {
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT_RESOURCE_MARIO:
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT_RESOURCE_LUIGI:
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT_RESOURCE:
            SingletonHolder< ResourceHolderManager >::get()->createAndAddStationed(pInfo->mArchive);
            break;
        case MR::StationedFileInfo::LOAD_TYPE_MOUNT_RESOURCE_LAYOUT:
            SingletonHolder< ResourceHolderManager >::get()->createAndAddLayoutHolderStationed(pInfo->mArchive);
            break;
        default:
            break;
        }
    }
}

void StationedArchiveLoader::loadScenarioData(JKRHeap* pHeap) {
<<<<<<< HEAD
    DVDDir dir;
    DVDDirEntry dirEntry;

    DVDOpenDir("/StageData", &dir);

    while (DVDReadDir(&dir, &dirEntry)) {
        if (!dirEntry.isDir) {
=======
    NebulaDisc::FolderEntry dirEntry;

    for (u32 i = 0; NebulaDisc::folderEntry("/StageData", i, &dirEntry); i++) {
        if (!dirEntry.mIsFolder) {
>>>>>>> upstream/main
            continue;
        }

        char name[256];
<<<<<<< HEAD
        MR::makeScenarioArchiveFileName(name, sizeof(name), dirEntry.name);
=======
        MR::makeScenarioArchiveFileName(name, sizeof(name), dirEntry.mName);
>>>>>>> upstream/main

        if (MR::isFileExist(name, false)) {
            MR::mountArchive(name, pHeap);
        }
    }
<<<<<<< HEAD

    DVDCloseDir(&dir);
=======
>>>>>>> upstream/main
}
