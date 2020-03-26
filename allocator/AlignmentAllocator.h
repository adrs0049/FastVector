////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  AlignmentAllocator.h                                          //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef ALIGNED_ALLOCATOR_H
#define ALIGNED_ALLOCATOR_H

//
// TODO: This header file currently does two different things! That needs to be
// sorted out.
//

#include "AlignedMemory.h"

#include <stdlib.h>
#include <malloc.h>
#include <memory>
#include <type_traits>

namespace Memory {

void* allocate_aligned_memory(size_t align, size_t size) ALLOC_ALIGN(1) ALLOC_SIZE(2) MALLOC;
void deallocate_aligned_memory(void* ptr) noexcept;

template<typename T, Alignment Align = Alignment::AVX>
class AlignedAllocator;

template<Alignment Align>
class AlignedAllocator<void, Align>
{
    public:
        using pointer       = void*;
        using const_pointer = const void*;
        using value_type    = void;

        template <class U> struct rebind { typedef AlignedAllocator<U, Align> other; };
};

template<typename T, Alignment Align>
class AlignedAllocator
{
    public:
        using value_type        = T;
        using pointer           = T*;
        using const_pointer     = const T*;
        using reference         = T&;
        using const_reference   = const T&;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;

        using propagate_on_container_move_assignment = std::true_type;

        template<class U>
        struct rebind { using other = AlignedAllocator<U, Align>; };

    public:
        AlignedAllocator() noexcept
        {}

        template<class U>
        AlignedAllocator(const AlignedAllocator<U, Align>&) noexcept
        {}

        size_type max_size() const noexcept
        { return (size_type(~0) - size_type(Align)) / sizeof(T); }

        pointer address(reference x) const noexcept
        { return std::addressof(x); }

        const_pointer address(const_reference x) const noexcept
        { return std::addressof(x); }

        pointer allocate(size_type n, typename AlignedAllocator<void, Align>::const_pointer = 0)
        {
            const size_type alignment = static_cast<size_type>( Align );
            void * ptr = allocate_aligned_memory(alignment, n * sizeof(T));
            if (ptr == nullptr)
                throw std::bad_alloc();

            return reinterpret_cast<pointer>(ptr);
        }

        void deallocate(pointer p, size_type) noexcept
        { return deallocate_aligned_memory(p);}

        template<class U, class ...Args>
        void construct(U* p, Args&&... args)
        { ::new(reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...); }

        void destroy(pointer p)
        { p->~T(); }
};

template<typename T, Alignment Align>
class AlignedAllocator<const T, Align>
{
    public:
        using value_type        = T;
        using pointer           = const T*;
        using const_pointer     = const T*;
        using reference         = const T&;
        using const_reference   = const T&;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;

        using propagate_on_container_move_assignment = std::true_type;

        template<class U>
        struct rebind { using other = AlignedAllocator<U, Align>; };

    public:
        AlignedAllocator() noexcept
        {}

        template<class U>
        AlignedAllocator(const AlignedAllocator<U, Align>&) noexcept
        {}

        size_type max_size() const noexcept
        { return (size_type(~0) - size_type(Align)) / sizeof(T); }

        const_pointer address(const_reference x) const noexcept
        { return std::addressof(x); }

        pointer allocate(size_type n, typename AlignedAllocator<void, Align>::const_pointer = 0)
        {
            const size_type alignment = static_cast<size_type>( Align );
            void * ptr = allocate_aligned_memory(alignment, n * sizeof(T));
            if (ptr == nullptr)
                throw std::bad_alloc();

            return reinterpret_cast<pointer>(ptr);
        }

        void deallocate(pointer p, size_type) noexcept
        { return deallocate_aligned_memory(p);}

        template<class U, class ...Args>
        void construct(U* p, Args&&... args)
        { ::new(reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...); }

        void destroy(pointer p)
        { p->~T(); }
};


template<typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool operator==(const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept
{ return TAlign == UAlign; }

template<typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool operator!=(const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept
{ return TAlign != UAlign; }

} // end namespace Memory

#endif
