////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  CSVector.h                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-03-30 12:22:20                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef CS_EXECUTION_POLICY_H
#define CS_EXECUTION_POLICY_H

#include <cstddef>
#include "VectorTraits.h"
#include "VectorOperations.h"
#include "LoopUnroll.h"

#include "type_info.h"


template <typename E1, typename E2, typename Functor, bool Vector = true,
         std::size_t BlockSize = 1, std::size_t Threads = 4>
class ParallelExecutionPolicy
{
private:
    // TODO!
    using size_type = typename E1::size_type;

public:
    void assign(E1& first, const E2& second)
    {
        size_type s = size(first), sb = s / BlockSize * BlockSize;

        std::cout << "Execution first< " << type_name<E1>() << "> : " << " second< " << type_name<E2>() << "> :" << std::endl;
        std::cout << "s:" << s << " sb:" << sb << " blockSize:" << BlockSize << std::endl;

        //#pragma omp parallel num_threads(Threads)
        {
            //#pragma omp for
            for (size_type i = 0; i < sb; i+=BlockSize)
            {
                impl::unroll<0, BlockSize-1, Functor, Vector>::apply(first, second, i);
            }
        }

        {
            for (size_type i = sb; i < s; i++)
            {
                impl::unroll<0, 0, Functor, Vector>::apply(first, second, i);
            }
        }
    }
};

template <typename E1, typename E2, typename Functor,
         bool Vector = true, std::size_t BlockSize = 4, std::size_t Threads = 1>
class UnrollExecutionPolicy
{
private:
    // TODO!
    using size_type = typename E1::size_type;

public:
    void assign(E1& first, const E2& second)
    {
        size_type s = size(first), sb = s / BlockSize * BlockSize;

        for (size_type i = 0; i < sb; i+=BlockSize)
            impl::unroll<0, BlockSize-1, Functor, Vector>::apply(first, second, i);

        for (size_type i = sb; i < s; i++)
            impl::unroll<0, 0, Functor, Vector>::apply(first, second, i);
    }
};

template <typename E1, typename E2, typename Functor, bool Vector,
         std::size_t BlockSize = 1, std::size_t Threads = 1>
class SerialExecutionPolicy
{
private:
    // TODO!
    using size_type = typename E1::size_type;

public:
    void assign(E1& first, const E2& second)
    {
        size_type s = size(first);
        for (size_type i = 0; i < s; i++)
            impl::unroll<0, 0, Functor, Vector>::apply(first, second, i);
    }
};

/// define the default policy
template <typename E1, typename E2, typename Functor, bool Vector>
using DefaultExecutionPolicy = ParallelExecutionPolicy<E1, E2, Functor, Vector>;

#endif
