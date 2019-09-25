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
auto Rotate(const VectorClass<T, 3>& vector, VectorClass<T, 3> axis, const T angle)
{
    // TODO in this case we dont have to assemble the inverse
    auto rotate = BasicDatatypes::Transform<T>::rotate(axis, angle);
    return Apply(rotate, vector);
}

template <typename T, typename = Enable_if<Floating_Point<T>()> >
auto Rotate_x(const VectorClass<T, 3>& vector, const T angle)
{
    auto rotate = BasicDatatypes::Transform<T>::rotate_x(angle);
    return Apply(rotate, vector);
}

template <typename T, typename = Enable_if<Floating_Point<T>()> >
auto Rotate_y(const VectorClass<T, 3>& vector, const T angle)
{
    auto rotate = BasicDatatypes::Transform<T>::rotate_y(angle);
    return Apply(rotate, vector);
}

template <typename T, typename = Enable_if<Floating_Point<T>()> >
auto Rotate_z(const VectorClass<T, 3>& vector, const T angle)
{
    auto rotate = BasicDatatypes::Transform<T>::rotate_z(angle);
    return Apply(rotate, vector);
}

// useful shortcuts
using Vector2 = VectorClass<double, 2>;
using Vector3 = VectorClass<double, 3>;
using Vector4 = VectorClass<double, 4>;

using Vector2d = VectorClass<double, 2>;
using Vector3d = VectorClass<double, 3>;
using Vector4d = VectorClass<double, 4>;

// uncomment when old vector class can be removed
// this should be float
using Vector2f = VectorClass<double, 2>;
using Vector3f = VectorClass<double, 3>;
using Vector4f = VectorClass<double, 4>;

using Vector2i = VectorClass<int, 2>;
using Vector3i = VectorClass<int, 3>;
using Vector4i = VectorClass<int, 4>;

using Vector2l = VectorClass<long, 2>;
using Vector3l = VectorClass<long, 3>;
using Vector4l = VectorClass<long, 4>;

using Vector2u = VectorClass<unsigned int, 2>;
using Vector3u = VectorClass<unsigned int, 3>;
using Vector4u = VectorClass<unsigned int, 4>;

using Vector2ul = VectorClass<unsigned long, 2>;
using Vector3ul = VectorClass<unsigned long, 3>;
using Vector4ul = VectorClass<unsigned long, 4>;

using Points = std::pair<Vector3d, Vector3d>;

std::ostream& operator<<(std::ostream& stream, const Points& points);

// compute the angle between (1,0,0) and a vector in the xy-plane
double getAngle_xy(const Vector3d& vec);
double getAngle_xz(const Vector3d& vec);
double getAngle_xz_unit(const Vector3d& vec);

#endif
