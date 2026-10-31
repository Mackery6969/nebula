#include "Game/Screen/CopyFilterNegater.hpp"
<<<<<<< HEAD
=======
#include <JSystem/J3DGraphBase/J3DNativeState.hpp>
>>>>>>> upstream/main
#include <JSystem/JUtility/JUTVideo.hpp>

CopyFilterNegater::CopyFilterNegater() {
    mRenderObj = JUTVideo::getManager()->getRenderMode();
<<<<<<< HEAD
    GXSetCopyFilter(GX_FALSE, mRenderObj->sample_pattern, GX_FALSE, mRenderObj->vfilter);
}

CopyFilterNegater::~CopyFilterNegater() {
    GXSetCopyFilter(GX_FALSE, mRenderObj->sample_pattern, GX_TRUE, mRenderObj->vfilter);
=======
    J3DNativeState::setCopyFilter(GX_FALSE, mRenderObj->sample_pattern, GX_FALSE, mRenderObj->vfilter);
}

CopyFilterNegater::~CopyFilterNegater() {
    J3DNativeState::setCopyFilter(GX_FALSE, mRenderObj->sample_pattern, GX_TRUE, mRenderObj->vfilter);
>>>>>>> upstream/main
}
