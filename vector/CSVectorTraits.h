////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  CSVector.h                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-03-30 12:22:20                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef CS_VECTOR_CONTAINER_TRAITS_H
#define CS_VECTOR_CONTAINER_TRAITS_H

template <typename T>
struct root
{
    using type = T;
};

struct TypeCategory {};

struct scalar : TypeCategory {};

struct NonScalar : TypeCategory {};

template <typename Value> struct Vector : NonScalar {};

// This will be used to help the compiler figure out if an object is of vector
// or scalar type.
template <typename T>
struct AssignShapeHelper;

template <typename T>
struct AssignShape : AssignShapeHelper<typename root<T>::type> {};

// by default every Shape will be a Scalar
template <typename T>
struct AssignShapeHelper
{
    using type = scalar;
};

// Define Helper for the main vector
template <typename T>
class CSVector;

template <typename Value>
struct AssignShapeHelper<CSVector<Value> >
{
    using type = Vector<typename AssignShape<Value>::type>;
};

// Expressions forward declerations
template <typename E1, typename E2, typename Functor>
class VectorVectorBinaryExpression;

template <typename E1, typename E2, typename Functor>
class VectorScalarBinaryExpression;

// Assignment expressions
template <typename E1, typename E2, typename Functor>
class VectorVectorAssignmentOpExpression;

template <typename E1, typename E2, typename Functor>
class VectorScalarAssignmentOpExpression;

// Helpers to classify the various expressions
template <typename E1, typename E2, typename Functor>
struct AssignShapeHelper<VectorScalarBinaryExpression<E1, E2, Functor> >
{
    using type = typename AssignShape<E1>::type;
};

template <typename E1, typename E2, typename Functor>
struct AssignShapeHelper<VectorVectorAssignmentOpExpression<E1, E2, Functor> >
{
    using type = typename AssignShape<E1>::type;
};

template <typename E1, typename E2, typename Functor>
struct AssignShapeHelper<VectorVectorBinaryExpression<E1, E2, Functor> >
{
    using type = typename AssignShape<E1>::type;
};


#endif