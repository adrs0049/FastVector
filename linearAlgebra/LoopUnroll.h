////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  CSVector.h                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-03-30 12:22:20                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef CS_LOOP_UNROLL_H
#define CS_LOOP_UNROLL_H

#include <cstddef>
#include <iostream>

#include "type_info.h"

//
// First we define the helpers that unroll the loop.
//
namespace impl {

    // The last template parameter determines whether we have a:
    //      1) vector <- vector
    //      2) vector <- scalar
    //
    //      Todo: Is there a more elegant way to do this?
    //
    template <unsigned long Offset, unsigned long Max, typename Functor, bool>
    class unroll;

    // template specialization that implements loop unrolling for vector <- vector loops
    template <unsigned long Offset, unsigned long Max, typename Functor>
    struct unroll<Offset, Max, Functor, true>
    {
        using next = unroll<Offset + 1, Max, Functor, true>;

        template <typename E1, typename E2, typename Size>
        static inline void apply(E1& first, const E2& second, Size i)
        {
            Functor::apply(first(Offset + i), second(Offset + i));
            next::apply(first, second, i);
        }
    };

    template <unsigned long Max, typename Functor>
    struct unroll<Max, Max, Functor, true>
    {
        template <typename E1, typename E2, typename Size>
        static inline void apply(E1& first, const E2& second, Size i)
        {
            Functor::apply(first(Max + i), second(Max + i));
        }
    };

    // template specialization that implements loop unrolling for vector <- scalar loops
    template <unsigned long Offset, unsigned long Max, typename Functor>
    struct unroll<Offset, Max, Functor, false>
    {
        using next = unroll<Offset + 1, Max, Functor, false>;

        template <typename E1, typename E2, typename Size>
        static inline void apply(E1& first, const E2& second, Size i)
        {
            Functor::apply(first(Offset + i), second);
            next::apply(first, second, i);
        }
    };

    template <unsigned long Max, typename Functor>
    struct unroll<Max, Max, Functor, false>
    {
        template <typename E1, typename E2, typename Size>
        static inline void apply(E1& first, const E2& second, Size i)
        {
            Functor::apply(first(Max + i), second);
        }
    };

} // end namespace


#endif
