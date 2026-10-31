#pragma once

<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
>>>>>>> upstream/main

class DemoCameraInfo;

namespace DemoCameraFunction {
    bool isCameraTargetMario(const DemoCameraInfo*);
    bool setStringNullIfEmpty(const char**);
    void makeAnimCameraName(char*, u32, const DemoCameraInfo*);
};  // namespace DemoCameraFunction
