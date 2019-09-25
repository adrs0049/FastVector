////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  AlignedMemory.cpp                                             //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "AlignmentAllocator.h"
#include "AlignedMemory.h"

#include <cassert>

namespace Memory {

void* allocate_aligned_memory(size_t align, size_t size)
{
    assert(align >= sizeof(void*));
    assert(is_power_of_two(align));

    if (size == 0)
        return nullptr;

    void* ptr = nullptr;
    int rc = posix_memalign(&ptr, align, size);

    if (rc != 0)
        return nullptr;

    return ptr;
}

void deallocate_aligned_memory(void * ptr) noexcept
{
    return free(ptr);
}

} //end namespace
