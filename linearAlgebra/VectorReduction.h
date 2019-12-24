////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  CSVector.h                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-03-30 12:22:20                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef CS_VECTOR_REDUCTION_H
#define CS_VECTOR_REDUCTION_H

#include <iostream>
#include <cmath>

using std::abs;
using std::max;

template <typename T>
T zero(T value)
{
    return T(0);
}

struct one_norm_functor
{
    template <typename Value>
    static inline void init(Value& value)
    {
        value = zero(value);
    }

    template <typename Value, typename Element>
    static inline void update(Value& value, const Element& x)
    {
        value += abs(x);
    }

    template <typename Value>
    static inline void finish(Value& value, const Value& value2)
    {
        value += value2;
    }

    template <typename Value>
    static inline Value post_reduction(const Value& value)
    {
        return value;
    }
};

struct sum_functor
{
    template <typename Value>
    static inline void init(Value& value)
    {
        value = zero(value);
    }

    template <typename Value, typename Element>
    static inline void update(Value& value, const Element& x)
    {
        value += x;
    }

    template <typename Value>
    static inline void finish(Value& value, const Value& value2)
    {
        value += value2;
    }

    template <typename Value>
    static inline Value post_reduction(const Value& value)
    {
        return value;
    }
};

struct product_functor
{
    template <typename Value>
    static inline void init(Value& value)
    {
        value = zero(value);
    }

    template <typename Value, typename Element>
    static inline void update(Value& value, const Element& x)
    {
        value *= x;
    }

    template <typename Value>
    static inline void finish(Value& value, const Value& value2)
    {
        value *= value2;
    }

    template <typename Value>
    static inline Value post_reduction(const Value& value)
    {
        return value;
    }
};

struct two_norm_functor
{
    template <typename Value>
    static inline void init(Value& value)
    {
        value = zero(value);
    }

    template <typename Value, typename Element>
    static inline void update(Value& value, const Element& x)
    {
        value += x * x;
    }

    template <typename Value>
    static inline void finish(Value& value, const Value& value2)
    {
        value += value2;
    }

    template <typename Value>
    static inline Value post_reduction(const Value& value)
    {
        Value (*sqrt) (const Value) = std::sqrt;
        return sqrt(value);
    }
};

struct unary_dot : two_norm_functor
{
    template <typename Value>
    static inline Value post_reduction(const Value& value)
    {
        return value;
    }
};

struct infinity_norm_functor
{
    template <typename Value>
    static inline void init(Value& value)
    {
        value = zero(value);
    }

    template <typename Value, typename Element>
    static inline void update(Value& value, const Element& x)
    {
        value = max(value, abs(x));
    }

    template <typename Value>
    static inline void finish(Value& value, const Value& value2)
    {
        value = max(value, abs(value2));
    }

    template <typename Value>
    static inline Value post_reduction(const Value& value)
    {
        return value;
    }
};

namespace impl {

    template <unsigned long Index0, unsigned long Max0, typename Functor>
    struct reduction
    {
        using next = reduction<Index0 + 1, Max0, Functor>;

        template <typename Value>
        static inline void init(Value& tmp00, Value& tmp01, Value& tmp02, Value& tmp03, Value& tmp04, Value& tmp05, Value& tmp06, Value& tmp07)
        {
            Functor::init(tmp00);
            next::init(tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, tmp00);
        }

        template <typename Value, typename Vector, typename Size>
        static inline void update(Value& tmp00, Value& tmp01, Value& tmp02, Value& tmp03, Value& tmp04, Value& tmp05, Value& tmp06, Value& tmp07, const Vector& v, Size i)
        {
            Functor::update(tmp00, v[i + Index0]);
            next::update(tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, tmp00, v, i);
        }

        template <typename Value>
        static inline void finish(Value& tmp00, Value& tmp01, Value& tmp02, Value& tmp03, Value& tmp04, Value& tmp05, Value& tmp06, Value& tmp07)
        {
            next::finish(tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, tmp00);
            Functor::finish(tmp00, tmp01);
        }
    };

    template <unsigned long Max0, typename Functor>
    struct reduction<Max0, Max0, Functor>
    {
        template <typename Value>
        static inline void init(Value& tmp00, Value&, Value&, Value&, Value&, Value&, Value&, Value&)
        {
            Functor::init(tmp00);
        }

        template <typename Value, typename Vector, typename Size>
        static inline void update(Value& tmp00, Value&, Value&, Value&, Value&, Value&, Value&, Value&, const Vector& v, Size i)
        {
            Functor::update(tmp00, v[i + Max0]);
        }

        template <typename Value>
        static inline void finish(Value&, Value&, Value&, Value&, Value&, Value&, Value&, Value&)
        {}
    };
} // end namespace


template <unsigned long Unroll, typename Functor, typename Result>
struct reduction
{
    template <typename Vector>
    static inline Result apply(const Vector& v)
    {
        using value_type  = typename Vector::value_type;
        using size_type   = typename Vector::size_type;

        Result result;
        Result tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07;

        constexpr size_type UNROLL = std::min(Unroll, size_type(8));

        const size_type s        = size(v);
        const size_type sb       = s / UNROLL * UNROLL;

        Functor::init(result);

        //#pragma omp parallel
        {
            impl::reduction<0, UNROLL-1, Functor>::init(tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07);

            //#pragma omp for
            for (size_t i = 0; i < sb; i+=UNROLL)
                impl::reduction<0, UNROLL-1, Functor>::update(tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, v, i);

            impl::reduction<0, UNROLL-1, Functor>::finish(tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07);

            //#pragma omp critical
            Functor::finish(result, tmp00);
        }

        for (size_t i = sb; i < s; i++)
            Functor::update(result, v[i]);

        return Functor::post_reduction(result);
    }
};


namespace impl {

    template <unsigned long Index0, unsigned long Max0>
    struct dot_aux
    {
        using next = dot_aux<Index0 + 1, Max0>;

        template <typename Value, typename Vector1, typename Vector2, typename Size>
        static inline void apply(Value& tmp00, Value& tmp01, Value& tmp02, Value& tmp03, Value& tmp04, Value& tmp05, Value& tmp06, Value& tmp07, const Vector1& v1, const Vector2& v2, Size i)
        {
            tmp00 = std::fma(v1[i + Index0], v2[i + Index0], tmp00);
            next::apply(tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, tmp00, v1, v2, i);
        }
    };

    template <unsigned long Max0>
    struct dot_aux<Max0, Max0>
    {
        template <typename Value, typename Vector1, typename Vector2, typename Size>
        static inline void apply(Value& tmp00, Value&, Value&, Value&, Value&, Value&, Value&, Value&, const Vector1& v1, const Vector2& v2, Size i)
        {
            tmp00 = std::fma(v1[i + Max0], v2[i + Max0], tmp00);
        }
    };

    template <unsigned long Unroll>
    struct dot
    {
        template <typename Vector1, typename Vector2>
        static inline auto apply(const Vector1& v1, const Vector2& v2)
        {
            using value_type  = typename Vector1::value_type;
            using size_type   = typename Vector1::size_type;
            value_type z      = value_type(0);
            value_type result = z;

            constexpr size_type UNROLL = std::min(Unroll, size_type(8));

            const size_type N        = size(v1);
            const size_type no_loops = N / UNROLL;

            //#pragma omp parallel
            {
                value_type tmp00 = z, tmp01 = z, tmp02 = z, tmp03 = z, tmp04 = z, tmp05 = z, tmp06 = z, tmp07 = z;

                //#pragma omp for
                for (size_type i = 0; i < no_loops; i+=UNROLL)
                    dot_aux<0, UNROLL-1>::apply(tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, v1, v2, i);

                //#pragma omp critical
                result += ((tmp00 + tmp01) + (tmp02 + tmp03)) + ((tmp04 + tmp05) + (tmp06 + tmp07));
            }

            for (size_type i = UNROLL * no_loops; i < N; i++)
                result = std::fma(v1[i], v2[i], result);

            return result;
        }
    };
} // end namespace

template <unsigned long Unroll, typename Vector1, typename Vector2>
inline auto dot(const Vector1& v1, const Vector2& v2)
{
    return impl::dot<Unroll>::apply(v1, v2);
}

#endif
