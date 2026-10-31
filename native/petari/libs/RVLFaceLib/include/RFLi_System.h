#ifndef RVL_FACE_LIBRARY_INTERNAL_SYSTEM_H
#define RVL_FACE_LIBRARY_INTERNAL_SYSTEM_H
#include "RFL_System.h"
<<<<<<< HEAD
#include "RFLi_Controller.h"
#include "RFLi_Database.h"
#include "RFLi_HiddenDatabase.h"
#include "RFLi_NANDAccess.h"
#include "RFLi_NANDLoader.h"
#include "RFLi_Types.h"
#include <revolution/mem.h>
=======
#include "RFLi_Database.h"
#include "RFLi_NANDLoader.h"
#include "RFLi_Types.h"
#include "RFLi_Heap.h"
>>>>>>> upstream/main
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct RFLiManager {
    void* workBuffer;                      // at 0x0
<<<<<<< HEAD
    MEMiHeapHead* rootHeap;                // at 0x4
    MEMiHeapHead* systemHeap;              // at 0x8
    MEMiHeapHead* tmpHeap;                 // at 0xC
    RFLiDBManager dbMgr;                   // at 0x10
    RFLiLoader loader;                     // at 0xC0
    RFLiHDBManager hdbMgr;                 // at 0x16C
    RFLiCtrlBufManager ctrlMgr;            // at 0x1AAC
=======
    NebulaMiiHeap* rootHeap;
    NebulaMiiHeap* systemHeap;
    NebulaMiiHeap* tmpHeap;
    RFLiLoader loader;                     // at 0xC0
>>>>>>> upstream/main
    BOOL working;                          // at 0x1B34
    BOOL deluxeTex;                        // at 0x1B38
    u8 brokenType;                         // at 0x1B3C
    RFLErrcode lastErrCode;                // at 0x1B40
    RFLErrcode beforeCloseErr;             // at 0x1B44
    s32 lastReason;                        // at 0x1B48
    s32 beforeCloseReason;                 // at 0x1B4C
<<<<<<< HEAD
    RFLiAccessInfo info[RFLiFileType_Max]; // at 0x1B50
=======
>>>>>>> upstream/main
    RFLiCallback systemCb;                 // at 0x1F10
    RFLCallback iconDrawCb;                // at 0x1F14
    RFLCallback modelDrawCb;               // at 0x1F18
    char UNK_0x1F1C[0x4];
    u32 _1F20;
} RFLiManager;

RFLErrcode RFLiBootLoadAsync(void);
void* RFLiAlloc(u32 size);
void* RFLiAlloc32(u32 size);
void RFLiFree(void* block);
<<<<<<< HEAD
RFLiDBManager* RFLiGetDBManager(void);
RFLiHDBManager* RFLiGetHDBManager(void);
=======
>>>>>>> upstream/main
RFLiLoader* RFLiGetLoader(void);
BOOL RFLiGetWorking(void);
void RFLiSetWorking(BOOL working);
RFLiManager* RFLiGetManager(void);
<<<<<<< HEAD
RFLiAccessInfo* RFLiGetAccInfo(RFLiFileType type);
RFLiCtrlBufManager* RFLiGetCtrlBufManager(void);
=======
BOOL RFLiIsWorking(void);
void RFLiStartWorking(void);
void RFLiEndWorkingReason(RFLErrcode err, s32 reason);
void RFLiEndWorking(RFLErrcode err);
>>>>>>> upstream/main
s32 RFLiGetLastReason(void);
void RFLiSetFileBroken(RFLiFileBrokenType type);
BOOL RFLiNotFoundError(void);
BOOL RFLiNeedRepairError(void);
BOOL RFLiCriticalError(void);
BOOL RFLiGetUseDeluxTex(void);

#ifdef __cplusplus
}
#endif
#endif
