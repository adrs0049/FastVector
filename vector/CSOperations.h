////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  CSVector.h                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-03-30 12:22:20                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef CS_OPERATIONS_H
#define CS_OPERATIONS_H

#include <functional>
#include <iostream>


template<class T> struct Assign
{
    void operator()(T& a, const T& c) { a = c; }
};

template <class T, class F> struct Func_assign
{
    void operator()(T& a, const T& c) { a = f(c); }
};

template<class T> struct Add_assign
{
    void operator()(T& a, const T& c) { a += c; }
};

template<class T> struct Mul_assign
{
    void operator()(T& a, const T& c) { a *= c; }
};

template<class T> struct Minus_assign
{
    void operator()(T& a, const T& c) { a -= c; }
};

template<class T> struct Div_assign
{
    void operator()(T& a, const T& c) { a /= c; }
};

template<class T> struct Mod_assign
{
    void operator()(T& a, const T& c) { a %= c; }
};

template<class T> struct Or_assign
{
    void operator()(T& a, const T& c) { a |= c; }
};

template<class T> struct Xor_assign
{
    void operator()(T& a, const T& c) { a ^= c; }
};

template<class T> struct And_assign
{
    void operator()(T& a, const T& c) { a &= c; }
};

template<class T> struct Not_assign
{
    void operator()(T& a) { a = !a; }
};

template<class T> struct Not
{
    T operator()(T& a) { return !a; }
};

template<class T> struct Unary_minus
{
    T operator()(T& a) { return -a; }
};

template<class T> struct Complement
{
    T operator()(T& a) { return ~a; }
};

// Views
template <typename Value>
struct negate
{
    using argument_type = const Value&;
    using result_type   = Value;

    static inline result_type apply(const Value& v) { return -v; }
    result_type operator() (const Value& v)
    {
        return -v;
    }
};

template <typename Value1, typename Value2>
struct plus_test
{
    using first_argument_type   = const Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = std::common_type_t<Value1, Value2>;

    static inline result_type apply(first_argument_type v1, second_argument_type v2)
    {
        return v1 + v2;
    }

    result_type operator()(first_argument_type v1, second_argument_type v2) const
    {
        return v1 + v2;
    }
};

template <typename Value1, typename Value2>
struct minus_test
{
    using first_argument_type   = const Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = std::common_type_t<Value1, Value2>;

    static inline result_type apply(first_argument_type v1, second_argument_type v2)
    {
        return v1 - v2;
    }

    result_type operator()(first_argument_type v1, second_argument_type v2) const
    {
        return v1 - v2;
    }
};

template <typename Value1, typename Value2>
struct inverse_minus
{
    using first_argument_type   = const Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = std::common_type_t<Value1, Value2>;

    static inline result_type apply(first_argument_type v1, second_argument_type v2)
    {
        return v2 - v1;
    }

    result_type operator()(first_argument_type v1, second_argument_type v2) const
    {
        return v2 - v1;
    }
};

template <typename Value1, typename Value2>
struct product
{
    using first_argument_type   = const Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = std::common_type_t<Value1, Value2>;

    static inline result_type apply(first_argument_type v1, second_argument_type v2)
    {
        return v1 * v2;
    }

    result_type operator()(first_argument_type v1, second_argument_type v2) const
    {
        return v1 * v2;
    }
};

template <typename Value1, typename Value2>
struct divide
{
    using first_argument_type   = const Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = std::common_type_t<Value1, Value2>;

    static inline result_type apply(first_argument_type v1, second_argument_type v2)
    {
        return v1 / v2;
    }

    result_type operator()(first_argument_type v1, second_argument_type v2) const
    {
        return v1 / v2;
    }
};

template <typename Value1, typename Value2>
struct inverse_divide
{
    using first_argument_type   = const Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = std::common_type_t<Value1, Value2>;

    static inline result_type apply(first_argument_type v1, second_argument_type v2)
    {
        return v2 / v1;
    }

    result_type operator()(first_argument_type v1, second_argument_type v2) const
    {
        return v2 / v1;
    }
};

template <typename Value1, typename Value2>
struct assign
{
    using first_argument_type   = Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = Value1&;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
        return v1 = Value1(v2);
    }

    result_type operator()(Value1& v1, const Value2& v2) const
    {
        return v1 = v2;
    }
};

template <typename Value1, typename Value2>
struct plus_assign
{
    using first_argument_type   = Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = Value1&;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
        return v1 += Value1(v2);
    }

    result_type operator()(Value1& v1, const Value2& v2) const
    {
        return v1 += v2;
    }
};

template <typename Value1, typename Value2>
struct minus_assign
{
    using first_argument_type   = Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = Value1&;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
        return v1 -= Value1(v2);
    }

    result_type operator()(Value1& v1, const Value2& v2) const
    {
        return v1 -= v2;
    }
};

template <typename Value1, typename Value2>
struct product_assign
{
    using first_argument_type   = Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = Value1&;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
        return v1 *= Value1(v2);
    }

    result_type operator()(Value1& v1, const Value2& v2) const
    {
        return v1 *= v2;
    }
};

template <typename Value1, typename Value2>
struct divide_assign
{
    using first_argument_type   = Value1&;
    using second_argument_type  = const Value2&;
    using result_type           = Value1&;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
        return v1 /= Value1(v2);
    }

    result_type operator()(Value1& v1, const Value2& v2) const
    {
        return v1 /= v2;
    }
};

template <typename Value>
struct identity
{
    using argument_type = const Value&;
    using result_type   = Value;

    static inline result_type apply(const Value& v)
    {
        return v;
    }

    result_type operator() (const Value& v) const
    {
        return v;
    }
};

#endif
