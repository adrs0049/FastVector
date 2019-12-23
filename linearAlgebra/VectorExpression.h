////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  CSVector.h                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-03-30 12:22:20                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef CS_EXPRESSION_H
#define CS_EXPRESSION_H

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <iostream>

#include "VectorTraits.h"
#include "ExecutionPolicy.h"
#include "concepts.h"
#include "type_info.h"


namespace Expression
{

// Template Expressions for CSVector<T>
//
// This is based on the chapter in Modern C++
//
// Essentially an expression is a replacement for a mathematical epxression such as
//
//      x = a + b
//
// Here a + b will be replaced by an expression object instead of the result.
//
// The goal is to do all memory access and store operations together, so that we
// avoid unnecessary loops over large vectors.
//
template <typename Vector>
struct VectorExpression {};

//
// VectorVectorBinaryExpression: This represents Functor(E1, E2) where E1 and E2 are
// both of vector type.
//
template <typename E1, typename E2, typename Functor>
    struct VectorVectorBinaryExpression : VectorExpression<VectorVectorBinaryExpression<E1, E2, Functor> >
{
    using base = VectorExpression< VectorVectorBinaryExpression<E1, E2, Functor> >;
    using self = VectorVectorBinaryExpression<E1, E2, Functor>;

    using value_type  = std::common_type_t<typename E1::value_type, typename E2::value_type>;
    using result_type = typename Functor::result_type;
    using size_type   = Common_type<typename E1::size_type, typename E2::size_type>;

    using first_argument_type   = E1;
    using second_argument_type  = E2;

    VectorVectorBinaryExpression(first_argument_type const& v1, second_argument_type const& v2)
        : first(v1), second(v2)
    {}

    result_type operator()(size_type i) const
    {
        return Functor::apply(first(i), second(i));
    }

    result_type operator[](size_type i) const
    {
        return (*this)(i);
    }

    template <typename EE1, typename EE2, typename FFunctor>
    friend std::size_t size(const VectorVectorBinaryExpression<EE1, EE2, FFunctor>&);

private:
    first_argument_type  const&     first;
    second_argument_type const&     second;
};

template <typename EE1, typename EE2, typename FFunctor>
inline std::size_t size(const VectorVectorBinaryExpression<EE1, EE2, FFunctor>& v)
{
    if (size(v.first) != size(v.second))
        throw std::runtime_error("Incompatible vector lengths in vector binary expression!");

    return size(v.first);
}

//
// VectorUnaryExpression: This represents Functor(E1, E2) where E1 and E2 are
// both of vector type.
//
template <typename E1, typename Functor>
struct VectorUnaryExpression : VectorExpression<VectorUnaryExpression<E1, Functor> >
{
    using base = VectorExpression< VectorUnaryExpression<E1, Functor> >;
    using self = VectorUnaryExpression<E1, Functor>;

    using value_type  = std::common_type_t<typename E1::value_type>;
    using result_type = typename Functor::result_type;
    using size_type   = Common_type<typename E1::size_type>;

    using first_argument_type   = E1;

    VectorUnaryExpression(first_argument_type const& v1)
        : first(v1)
    {}

    result_type operator()(size_type i) const
    {
        return Functor::apply(first(i));
    }

    result_type operator[](size_type i) const
    {
        return (*this)(i);
    }

    template <typename EE1, typename FFunctor>
    friend std::size_t size(const VectorUnaryExpression<EE1, FFunctor>&);

private:
    first_argument_type  const&     first;
};

template <typename EE1, typename FFunctor>
inline std::size_t size(const VectorUnaryExpression<EE1, FFunctor>& v)
{
    return size(v.first);
}

//
// VectorScalarBinaryExpression replaces expressions of the form
//
// Functor(E1, E2) where E1 is a vector and E2 is a scalar.
//
template <typename E1, typename E2, typename Functor>
struct VectorScalarBinaryExpression : VectorExpression<VectorScalarBinaryExpression<E1, E2, Functor> >
{
    using base = VectorExpression< VectorScalarBinaryExpression <E1, E2, Functor> >;
    using self = VectorScalarBinaryExpression <E1, E2, Functor>;

    using value_type  = std::common_type_t<typename E1::value_type, E2>;
    using result_type = typename Functor::result_type;
    using size_type   = typename E1::size_type;

    using first_argument_type   = E1;
    using second_argument_type  = E2;

    VectorScalarBinaryExpression (first_argument_type const& v1, second_argument_type const& v2)
        : first(v1), second(v2)
    {}

    result_type operator()(size_type i) const
    {
        return Functor::apply(first(i), second);
    }

    result_type operator[](size_type i) const
    {
        return (*this)(i);
    }

    template <typename EE1, typename EE2, typename FFunctor>
    friend std::size_t size(const VectorScalarBinaryExpression<EE1, EE2, FFunctor>&);

private:
    first_argument_type  const&     first;
    second_argument_type const&     second;
};

template <typename EE1, typename EE2, typename FFunctor>
inline std::size_t size(const VectorScalarBinaryExpression<EE1, EE2, FFunctor>& v)
{
    return size(v.first);
}

//
// VectorVectorAssignmentOpExpression replaces
//
// E1 = Function(E2)
//
// the result is written to the memory in E1 when the destructor of this
// expression is called. The for loop that does this assignment is implemented
// using openMp and the loop is unrolled using templates to encourage the
// compiler to use SSE / AVX instructions.
//
// Now we define the actual expression
template <typename E1, typename E2, typename Functor,
          typename ExecutionPolicy = DefaultExecutionPolicy<E1, E2, Functor, true> >
struct VectorVectorAssignmentOpExpression :
    VectorExpression<VectorVectorBinaryExpression<E1, E2, Functor> >,
    private ExecutionPolicy
{
    using base = VectorExpression< VectorVectorAssignmentOpExpression<E1, E2, Functor, ExecutionPolicy> >;
    using self = VectorVectorAssignmentOpExpression<E1, E2, Functor, ExecutionPolicy>;

    using value_type  = typename E1::value_type;
    using result_type = typename Functor::result_type;
    using size_type   = Common_type<typename E1::size_type, typename E2::size_type>;

    using first_argument_type   = E1;
    using second_argument_type  = E2;

    VectorVectorAssignmentOpExpression(first_argument_type& v1, second_argument_type const& v2)
        : first(v1), second(v2)
    {
        // TODO this check should come earlier! - so we don't throw in the dtor
        if (size(first) != size(second))
            throw std::runtime_error("Incompatible vector lengths in vec_vec_assign!");
    }

    ~VectorVectorAssignmentOpExpression()
    {
        ExecutionPolicy::assign(first, second);
    }

    result_type operator()(size_type i) const
    {
        return Functor::apply(first(i), second(i));
    }

    result_type operator[](size_type i) const
    {
        return (*this)(i);
    }

    template <typename EE1, typename EE2, typename FFunctor>
    friend std::size_t size(const VectorVectorAssignmentOpExpression<EE1, EE2, FFunctor>&);

private:
    first_argument_type&            first;
    second_argument_type const&     second;
};

template <typename EE1, typename EE2, typename FFunctor>
inline std::size_t size(const VectorVectorAssignmentOpExpression<EE1, EE2, FFunctor>& v)
{
    if (size(v.first) != size(v.second))
        throw std::runtime_error("Incompatible vector lengths in vector assign op!");

    return size(v.first);
}

//
// VectorScalarAssignmentOpExpression replaces expressions
//
// E1 = Functor(E2)
//
// where E1 is of vector type and E2 is a scalar. Otherwise the function is the
// same as of VectorVectorAssignmentOpExpression. Note we are not using openMP
// here at the moment.
//
template <typename E1, typename E2, typename Functor,
         typename ExecutionPolicy = DefaultExecutionPolicy<E1, E2, Functor, false> >
struct VectorScalarAssignmentOpExpression :
    VectorExpression<VectorVectorBinaryExpression<E1, E2, Functor> >,
    private ExecutionPolicy
{
    using base = VectorExpression< VectorScalarAssignmentOpExpression<E1, E2, Functor, ExecutionPolicy> >;
    using self = VectorScalarAssignmentOpExpression<E1, E2, Functor, ExecutionPolicy>;

    // do some transform checks!
    using value_type  = typename E1::value_type;
    using result_type = typename Functor::result_type;
    using size_type   = typename E1::size_type;

    using first_argument_type   = E1;
    using second_argument_type  = E2;

    VectorScalarAssignmentOpExpression(first_argument_type& v1, second_argument_type const& v2)
        : first(v1), second(v2)
    {}

    ~VectorScalarAssignmentOpExpression()
    {
        ExecutionPolicy::assign(first, second);
    }

    result_type operator()(size_type i) const
    {
        return Functor::apply(first(i), second);
    }

    result_type operator[](size_type i) const
    {
        return (*this)(i);
    }

    template <typename EE1, typename EE2, typename FFunctor, typename Policy>
    friend std::size_t size(const VectorScalarAssignmentOpExpression<EE1, EE2, FFunctor, Policy>&);

private:
    first_argument_type&            first;
    second_argument_type const&     second;
};


template <typename EE1, typename EE2, typename FFunctor, typename Policy>
inline std::size_t size(const VectorScalarAssignmentOpExpression<EE1, EE2, FFunctor, Policy>& v)
{
    return size(v.first);
}

//
// AssignmentExpressions i.e. the implementation of
// VectorVectorAssignmentOpExpression where the functor is f(a,b) {a = b};
//
template <typename E1, typename E2>
struct VectorVectorAssignExpression
: public VectorVectorAssignmentOpExpression<E1, E2, assign<typename E1::value_type, typename E2::value_type> >
{
    using base = VectorVectorAssignmentOpExpression<E1, E2, assign<typename E1::value_type, typename E2::value_type> >;
    VectorVectorAssignExpression(E1& v1, E2 const& v2)
        : base(v1, v2)
    {}
};

//
// AssignmentExpressions i.e. the implementation of
// VectorScalarAssignmentOpExpression where the functor is f(a,b) {a = b};
//
template <typename E1, typename E2>
struct VectorScalarAssignmentExpression
: public VectorScalarAssignmentOpExpression<E1, E2, assign<typename E1::value_type, E2> >
{
    using base = VectorScalarAssignmentOpExpression<E1, E2, assign<typename E1::value_type, E2> >;
    VectorScalarAssignmentExpression(E1& v1, E2 const& v2)
        : base(v1, v2)
    {}
};

//
// The last thing that remains to be done is to help the compiler determine
// which AssignmentExpression it needs.
//
namespace detail {

template <typename Vector, typename Source, typename SourceCategory, typename VCat>
struct VectorAssignment {};

template <typename Vector, typename Source, typename Cat>
struct VectorAssignment<Vector, Source, Cat, Cat>
{
    using type = VectorVectorAssignExpression<Vector, Source>;
    type operator()(Vector& vector, const Source& src)
    {
        return type(vector, src);
    }
};

template <typename Vector, typename Source, typename VectorCategory>
struct VectorAssignment<Vector, Source, VectorCategory, scalar>
{
    using type = VectorScalarAssignmentExpression<Vector, Source>;
    type operator()(Vector& vector, const Source& src)
    {
        return type(vector, src);
    }
};

} // end namespace

//
// Switch to select the correct AssignmentExpressionOperator
//
// Vector: is the vector type to which we want to assign the result of some op.
// Source: is the expression whose result we want to assign to Vector
//
// We use the traits from CSVectorTraits to switch between the Vector and Scalar
// assignment expression.
//
template <typename Vector, typename Source>
struct VectorAssignment :
    detail::VectorAssignment<Vector, Source, typename AssignShape<Vector>::type, typename AssignShape<Source>::type>
{};

} // end namespace

#endif
