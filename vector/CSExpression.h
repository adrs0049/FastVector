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

#include "CSOperations.h"
#include "CSVectorTraits.h"
#include "type_info.h"

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

    // load from somewhere!
    using size_type = typename E1::size_type;

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

    // load from somewhere!
    using size_type = std::size_t;

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
//
// First we define the helpers that unroll the loop.
//
namespace v_impl {

    template <unsigned long Offset, unsigned long Max, typename Functor>
    struct assign
    {
        using next = assign<Offset + 1, Max, Functor>;

        template <typename E1, typename E2, typename Size>
        static inline void apply(E1& first, const E2& second, Size i)
        {
            Functor::apply(first(Offset + i), second(Offset + i));
            next::apply(first, second, i);
        }
    };

    template <unsigned long Max, typename Functor>
    struct assign<Max, Max, Functor>
    {
        template <typename E1, typename E2, typename Size>
        static inline void apply(E1& first, const E2& second, Size i)
        {
            Functor::apply(first(Max + i), second(Max + i));
        }
    };

} // end namespace

// Now we define the actual expression
template <typename E1, typename E2, typename Functor>
struct VectorVectorAssignmentOpExpression : VectorExpression<VectorVectorBinaryExpression<E1, E2, Functor> >
{
    using base = VectorExpression< VectorVectorAssignmentOpExpression<E1, E2, Functor> >;
    using self = VectorVectorAssignmentOpExpression<E1, E2, Functor>;

    using value_type  = typename E1::value_type;
    using result_type = typename Functor::result_type;

    // load from somewhere!
    using size_type = std::size_t;

    using first_argument_type   = E1;
    using second_argument_type  = E2;

    VectorVectorAssignmentOpExpression(first_argument_type& v1, second_argument_type const& v2)
        : first(v1), second(v2)
    {}

    ~VectorVectorAssignmentOpExpression()
    {
        assign();
    }

    void assign()
    {
        const size_type BSize = 4;
        size_type s = size(first), sb = s / BSize * BSize;

        if (size(first) != size(second))
            throw std::runtime_error("Incompatible vector lengths in vec_vec_assign!");

        #pragma omp parallel num_threads(4)
        {
            #pragma omp for
            for (size_type i = 0; i < sb; i+=BSize)
                v_impl::assign<0, BSize-1, Functor>::apply(first, second, i);

            //for (size_type i = 0; i < s; i++)
            //Functor::apply(first(i), second(i));
        }

        {
            for (size_t i = sb; i < s; i++)
                Functor::apply(first(i), second(i));
        }
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
// First we define the helpers that unroll the loop.
//
namespace s_impl {

    template <unsigned long Offset, unsigned long Max, typename Functor>
    struct assign
    {
        using next = assign<Offset + 1, Max, Functor>;

        template <typename E1, typename E2, typename Size>
        static inline void apply(E1& first, const E2& second, Size i)
        {
            Functor::apply(first(Offset + i), second);
            next::apply(first, second, i);
        }
    };

    template <unsigned long Max, typename Functor>
    struct assign<Max, Max, Functor>
    {
        template <typename E1, typename E2, typename Size>
        static inline void apply(E1& first, const E2& second, Size i)
        {
            Functor::apply(first(Max + i), second);
        }
    };

} // end namespace

template <typename E1, typename E2, typename Functor>
struct VectorScalarAssignmentOpExpression : VectorExpression<VectorVectorBinaryExpression<E1, E2, Functor> >
{
    using base = VectorExpression< VectorScalarAssignmentOpExpression<E1, E2, Functor> >;
    using self = VectorScalarAssignmentOpExpression<E1, E2, Functor>;

    using value_type  = typename E1::value_type;
    using result_type = typename Functor::result_type;

    // load from somewhere!
    using size_type = std::size_t;

    using first_argument_type   = E1;
    using second_argument_type  = E2;

    VectorScalarAssignmentOpExpression(first_argument_type& v1, second_argument_type const& v2)
        : first(v1), second(v2)
    {}

    ~VectorScalarAssignmentOpExpression()
    {
        assign();
    }

    void assign()
    {
        const size_type BSize = 4;
        size_type s = size(first), sb = s / BSize * BSize;

        size_type N = size(first);

        #pragma omp parallel num_threads(4)
        {
            #pragma omp for
            for (size_type i = 0; i < sb; i+=BSize)
                s_impl::assign<0, BSize-1, Functor>::apply(first, second, i);
        }

        {
            for (size_type i = sb; i < s; i++)
                Functor::apply(first(i), second);
        }
    }

    result_type operator()(size_type i) const
    {
        return Functor::apply(first(i), second);
    }

    result_type operator[](size_type i) const
    {
        return (*this)(i);
    }

    template <typename EE1, typename EE2, typename FFunctor>
    friend std::size_t size(const VectorScalarAssignmentOpExpression<EE1, EE2, FFunctor>&);

private:
    first_argument_type&            first;
    second_argument_type const&     second;
};

template <typename EE1, typename EE2, typename FFunctor>
inline std::size_t size(const VectorScalarAssignmentOpExpression<EE1, EE2, FFunctor>& v)
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
}

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


#endif
