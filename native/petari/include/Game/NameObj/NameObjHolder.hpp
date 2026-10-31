#pragma once

#include "Game/Util/Array.hpp"
<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
>>>>>>> upstream/main

class NameObj;

typedef void (NameObj::*NameObjMethod)(void);
typedef void (NameObj::*NameObjMethodConst)(void) const;

class NameObjHolder {
public:
    NameObjHolder(int num);

    void add(NameObj* pObj);
    void suspendAllObj();
    void resumeAllObj();
    void syncWithFlags();
    void callMethodAllObj(NameObjMethod pMethod);
    void clearArray();
    NameObj* find(const char* pName);

private:
    /* 0x00 */ MR::Vector< MR::AssignableArray< NameObj* > > mObjArray1;
    /* 0x0C */ MR::Vector< MR::FixedArray< NameObj*, 16 > > mObjArray2;
};
