////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  vector_detail.h                                               //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef VECTOR_OPERATIONS_H
#define VECTOR_OPERATIONS_H

#include "VectorTraits.h"
#include "VectorFunctors.h"
#include "VectorExpression.h"
#include "VectorReduction.h"

#include "type_info.h"


using namespace Expression;

template <typename E1, unsigned long Unroll = 4>
inline auto Norm(const VectorExpression<E1>& e1)
{
    using value_type = typename E1::value_type;
    return reduction<Unroll, two_norm_functor, value_type>::apply(static_cast<const E1&>(e1));
}

template <typename E1, unsigned long Unroll = 4>
inline auto Norm2(const VectorExpression<E1>& e1)
{
    using value_type = typename E1::value_type;
    return reduction<Unroll, two_norm_functor, value_type>::apply(static_cast<const E1&>(e1));
}

template <typename E1, unsigned long Unroll = 4>
inline auto Norm2Squared(const VectorExpression<E1>& e1)
{
    using value_type = typename E1::value_type;
    return reduction<Unroll, unary_dot, value_type>::apply(static_cast<const E1&>(e1));
}

template <typename E1>
inline auto Normalize(const VectorExpression<E1>& e1)
{
    std::cout << "Calling normalize: " << type_name<E1>() << std::endl;

    // make sure that we can still call the more specific Norm functions
    using value_type = typename E1::value_type;
    //value_type norm = Norm(static_cast<const E1&>(e1));
    value_type norm = value_type(1);
    std::cout << "norm: " << norm << std::endl;
    if (!std::isnormal(norm))
        throw std::overflow_error("Divide by zero exception!");

    std::cout << "type_norm: " << type_name<value_type>() << std::endl;

    auto ret = static_cast<const E1&>(e1) / norm;

    std::cout << "expr(";
    for (size_t i = 0; i < size(ret); i++)
        std::cout << " " << ret(i);
    std::cout << std::endl << std::endl;

    std::cout << "ret: " << demangle(typeid(ret).name()) << std::endl;

    auto vec = E1(ret);
    std::cout << "ret: " << vec << std::endl;

    std::cout << "\nnorm:" << norm << "\n";
    std::cout << "\nRETURN!\n\n";
    //return vec;
    return e1 / norm;
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarAssignmentOpExpression<E1, E2, plus_assign<typename E1::value_type, E2> >
operator+= (VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarAssignmentOpExpression<E1, E2, plus_assign<typename E1::value_type, E2> >;
    return rtype(static_cast<E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarAssignmentOpExpression<E1, E2, minus_assign<typename E1::value_type, E2> >
operator-= (VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarAssignmentOpExpression<E1, E2, minus_assign<typename E1::value_type, E2> >;
    return rtype(static_cast<E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarAssignmentOpExpression<E1, E2, product_assign<typename E1::value_type, E2> >
operator*= (VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarAssignmentOpExpression<E1, E2, product_assign<typename E1::value_type, E2> >;
    return rtype(static_cast<E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarAssignmentOpExpression<E1, E2, divide_assign<typename E1::value_type, E2> >
operator/= (VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarAssignmentOpExpression<E1, E2, divide_assign<typename E1::value_type, E2> >;
    return rtype(static_cast<E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarBinaryExpression<E1, E2, plus_test<typename E1::value_type, E2> >
operator+ (const VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarBinaryExpression<E1, E2, plus_test<typename E1::value_type, E2> >;
    return rtype(static_cast<const E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E1>()> >
inline VectorScalarBinaryExpression<E2, E1, plus_test<typename E2::value_type, E1> >
operator+ (const E1& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorScalarBinaryExpression<E2, E1, plus_test<typename E2::value_type, E1> >;
    return rtype(static_cast<const E2&>(e2), e1);
}


template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarBinaryExpression<E1, E2, minus_test<typename E1::value_type, E2> >
operator- (const VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarBinaryExpression<E1, E2, minus_test<typename E1::value_type, E2> >;
    return rtype(static_cast<const E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E1>()> >
inline VectorScalarBinaryExpression<E2, E1, inverse_minus<typename E2::value_type, E1> >
operator- (const E1& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorScalarBinaryExpression<E2, E1, inverse_minus<typename E2::value_type, E1> >;
    return rtype(static_cast<const E2&>(e2), e1);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E1>()> >
inline VectorScalarBinaryExpression<E2, E1, product<typename E2::value_type, E1> >
operator* (const E1& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorScalarBinaryExpression<E2, E1, product<typename E2::value_type, E1> >;
    return rtype(static_cast<const E2&>(e2), e1);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()>>
inline VectorScalarBinaryExpression<E1, E2, product<typename E1::value_type, E2> >
operator* (const VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarBinaryExpression<E1, E2, product<typename E1::value_type, E2> >;
    return rtype(static_cast<const E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E1>()> >
inline VectorScalarBinaryExpression<E2, E1, inverse_divide<typename E2::value_type, E1> >
operator/ (const E1& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorScalarBinaryExpression<E2, E1, inverse_divide<typename E2::value_type, E1> >;
    return rtype(static_cast<const E2&>(e2), e1);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()>>
inline VectorScalarBinaryExpression<E1, E2, divide<typename E1::value_type, E2> >
operator/ (const VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarBinaryExpression<E1, E2, divide<typename E1::value_type, E2> >;
    return rtype(static_cast<const E1&>(e1), e2);
}

template <typename E1, typename E2>
inline VectorVectorBinaryExpression<E1, E2, product<typename E1::value_type, typename E2::value_type> >
operator* (const VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorBinaryExpression<E1, E2, product<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorBinaryExpression<E1, E2, divide<typename E1::value_type, typename E2::value_type> >
operator/ (const VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorBinaryExpression<E1, E2, divide<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorBinaryExpression<E1, E2, plus_test<typename E1::value_type, typename E2::value_type> >
operator+ (const VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorBinaryExpression<E1, E2, plus_test<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorBinaryExpression<E1, E2, minus_test<typename E1::value_type, typename E2::value_type> >
operator- (const VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorBinaryExpression<E1, E2, minus_test<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1>
inline VectorUnaryExpression<E1, negate<typename E1::value_type> >
operator- (const VectorExpression<E1>& e1)
{
    using rtype = VectorUnaryExpression<E1, negate<typename E1::value_type> >;
    return rtype(static_cast<const E1&>(e1));
}

template <typename E1>
inline VectorUnaryExpression<E1, Abs<typename E1::value_type> >
abs(const VectorExpression<E1>& e1)
{
    using rtype = VectorUnaryExpression<E1, Abs<typename E1::value_type> >;
    return rtype(static_cast<const E1&>(e1));
}

template <typename E1, typename E2>
inline VectorVectorAssignmentOpExpression<E1, E2, plus_assign<typename E1::value_type, typename E2::value_type> >
operator+= (VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorAssignmentOpExpression<E1, E2, plus_assign<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorAssignmentOpExpression<E1, E2, minus_assign<typename E1::value_type, typename E2::value_type> >
operator-= (VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorAssignmentOpExpression<E1, E2, minus_assign<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorAssignmentOpExpression<E1, E2, product_assign<typename E1::value_type, typename E2::value_type> >
operator*= (VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorAssignmentOpExpression<E1, E2, product_assign<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorAssignmentOpExpression<E1, E2, divide_assign<typename E1::value_type, typename E2::value_type> >
operator/= (VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorAssignmentOpExpression<E1, E2, divide_assign<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<E1&>(e1), static_cast<const E2&>(e2));
}


#endif
