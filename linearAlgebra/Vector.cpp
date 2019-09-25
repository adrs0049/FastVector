////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  Vector.cpp                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "Vector.h"
#include <cmath>

std::ostream& operator<<(std::ostream& stream, const Points& points)
{
    stream << "(" << points.first << ", " << points.second << ")";
    return stream;
}

double getAngle_xy(const Vector3d& vec)
{
    double angle = atan2(vec.y, vec.x);
    return (angle >= 0) ? angle : angle + 2. * M_PI;
}

double getAngle_xz(const Vector3d& vec)
{
    return acos(vec.z / Norm(vec));
}

double getAngle_xz_unit(const Vector3d& vec)
{
    return acos(vec.z);
}
