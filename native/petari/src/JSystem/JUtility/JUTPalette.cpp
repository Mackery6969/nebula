#include "JSystem/JUtility/JUTPalette.hpp"
<<<<<<< HEAD
#include <revolution/os.h>

void JUTPalette::storeTLUT(_GXTlut param_0, ResTLUT* tlut) {
    if (tlut == nullptr) {
        OSPanic(__FILE__, 35, "JUTTexture: TLUT is NULL\n");
=======
#include <JSystem/J3DGraphBase/J3DNativeTexture.hpp>
#include <revolution/os.h>
#include "JSystem/JUtility/JUTException.hpp"

void JUTPalette::storeTLUT(_GXTlut param_0, ResTLUT* tlut) {
    if (tlut == nullptr) {
        JUTException::panic_f(__FILE__, 35, "JUTTexture: TLUT is NULL\n");
>>>>>>> upstream/main
    }
    mName = param_0;
    mFormat = tlut->mFormat;
    mTransparency = tlut->mTransparency;
    mLutNum = tlut->mLutNum;
    mColorTable = tlut + 8;
<<<<<<< HEAD
    GXInitTlutObj(&mObj, mColorTable, static_cast< GXTlutFmt >(mFormat), mLutNum);
=======
    J3DNativeTexture::initTlut(&mObj, mColorTable, static_cast< GXTlutFmt >(mFormat), mLutNum);
>>>>>>> upstream/main
}

void JUTPalette::storeTLUT(_GXTlut param_0, _GXTlutFmt param_1, JUTTransparency param_2, u16 param_3, void* param_4) {
    mName = param_0;
    mFormat = param_1;
    mTransparency = param_2;
    mLutNum = param_3;
    mColorTable = static_cast< ResTLUT* >(param_4);
<<<<<<< HEAD
    GXInitTlutObj(&mObj, mColorTable, static_cast< GXTlutFmt >(mFormat), mLutNum);
=======
    J3DNativeTexture::initTlut(&mObj, mColorTable, static_cast< GXTlutFmt >(mFormat), mLutNum);
>>>>>>> upstream/main
}

bool JUTPalette::load() {
    bool check = mLutNum != 0;
    if (check) {
<<<<<<< HEAD
        GXLoadTlut(&mObj, mName);
=======
        J3DNativeTexture::loadTlut(&mObj, mName);
>>>>>>> upstream/main
    }

    return check;
}
