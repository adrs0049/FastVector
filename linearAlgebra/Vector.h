////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  Vector.h                                                      //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef CS_VECTOR_H
#define CS_VECTOR_H

#include <utility>

// Import Basic Vector library
#include "vector_detail.h"
#include "Matrix.h"
#include "Transform.h"

// Aligned Data types required for the solvers
#include "AlignedVector.h"

// Converts degrees to radians.
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

// Converts radians to degrees.
#define radiansToDegrees(angleRadians) (angleRadians * 180.0 / M_PI)

// Define some commonly used transformations of vectors
//
// We cant define them earlier as they require Transformations to be defined
//
template <typename T, typename = Enable_if<Floating_Point<T>()> >
auto Rotate(const ConstantVector<T, 3>& vector, ConstantVector<T, 3> axis, const T angle)
{
    // TODO in this case we dont have to assemble the inverse
    auto rotate = BasicDatatypes::Transform<T>::rotate(axis, angle);
    return Apply(rotate, vector);
}

template <typename T, typename = Enable_if<Floating_Point<T>()> >
auto Rotate_x(const ConstantVector<T, 3>& vector, const T angle)
{
    auto rotate = BasicDatatypes::Transform<T>::rotate_x(angle);
    return Apply(rotate, vector);
}

template <typename T, typename = Enable_if<Floating_Point<T>()> >
auto Rotate_y(const ConstantVector<T, 3>& vector, const T angle)
{
    auto rotate = BasicDatatypes::Transform<T>::rotate_y(angle);
    return Apply(rotate, vector);
}

template <typename T, typename = Enable_if<Floating_Point<T>()> >
auto Rotate_z(const ConstantVector<T, 3>& vector, const T angle)
{
    auto rotate = BasicDatatypes::Transform<T>::rotate_z(angle);
    return Apply(rotate, vector);
}

// useful shortcuts
using Vector2 = ConstantVector<double, 2>;
using Vector3 = ConstantVector<double, 3>;
using Vector4 = ConstantVector<double, 4>;

using Vector2d = ConstantVector<double, 2>;
using Vector3d = ConstantVector<double, 3>;
using Vector4d = ConstantVector<double, 4>;

// uncomment when old vector class can be removed
// this should be float
using Vector2f = ConstantVector<float, 2>;
using Vector3f = ConstantVector<float, 3>;
using Vector4f = ConstantVector<float, 4>;

using Vector2i = ConstantVector<int, 2>;
using Vector3i = ConstantVector<int, 3>;
using Vector4i = ConstantVector<int, 4>;

using Vector2l = ConstantVector<long, 2>;
using Vector3l = ConstantVector<long, 3>;
using Vector4l = ConstantVector<long, 4>;

using Vector2u = ConstantVector<unsigned int, 2>;
using Vector3u = ConstantVector<unsigned int, 3>;
using Vector4u = ConstantVector<unsigned int, 4>;

using Vector2ul = ConstantVector<unsigned long, 2>;
using Vector3ul = ConstantVector<unsigned long, 3>;
using Vector4ul = ConstantVector<unsigned long, 4>;

using Points = std::pair<Vector3d, Vector3d>;

std::ostream& operator<<(std::ostream& stream, const Points& points);

// compute the angle between (1,0,0) and a vector in the xy-plane
double getAngle_xy(const Vector3d& vec);
double getAngle_xz(const Vector3d& vec);
double getAngle_xz_unit(const Vector3d& vec);

#endif
