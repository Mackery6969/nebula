#pragma once

<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
#include "Game/System/NebulaKernel.hpp"
>>>>>>> upstream/main

class JKRHeap;

namespace MR {
    template < int T >
    class MutexHolder {
    public:
<<<<<<< HEAD
        static OSMutex sMutex;
    };

    template < int T >
    OSMutex MutexHolder< T >::sMutex;
=======
        static NebulaMutex sMutex;
    };

    template < int T >
    NebulaMutex MutexHolder< T >::sMutex;
>>>>>>> upstream/main
};  // namespace MR
