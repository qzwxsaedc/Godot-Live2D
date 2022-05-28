//
// Created by qzwxsaedc on 2022/5/27.
//

#ifndef LIBGDLIVE2D_ALLOCATOR_H
#define LIBGDLIVE2D_ALLOCATOR_H

#include <CubismFramework.hpp>
#include <ICubismAllocator.hpp>

namespace Live2DWrapper {
    class Allocator : public Csm::ICubismAllocator {
        void* Allocate(const Csm::csmSizeType size);
        void Deallocate(void* memory);
        void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment);
        void DeallocateAligned(void* alignedMemory);
    };
}


#endif //LIBGDLIVE2D_ALLOCATOR_H
