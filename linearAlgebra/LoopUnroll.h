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
            std::cout << "assign_v first(" << Offset + i << "): " << first(Offset + i) << " second(" << Offset + i << "): "
                << second(Offset + i) << std::endl;
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
            std::cout << "\tassign_v first< "
                << type_name<E1>()
                << ",\n\t\t\t " << type_name<E2>()
                << ",\n\t\t\t " << type_name<Functor>()
                << "> "
                << std::endl;

            //std::cout << "assign_v first (" << Max + i << "): " << first(Max + i)
            //    << " second(" << Max + i << "): "
            //    << second(Max + i) << std::endl;

            auto vv = second(Max + i);

            std::cout << "assign_v; second(" << Max + i << "): "
                << " i:" << i << " Max:" << Max << " vv:"
                << vv
                << " inv: " << 1. / vv
                << std::endl;

            Functor::apply(first(Max + i), second(Max + i));

            auto val = second(Max + i);

            std::cout << "assign_v_after; second(" << Max + i << "): " << val
                << " inv: " << 1. / val
                << std::endl;

            std::cout << std::endl;
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
            std::cout << "assign_s first(" << Offset + i << "): " << first(Offset + i) << " second(" << Offset + i << "): "
                << second
                << std::endl;
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
