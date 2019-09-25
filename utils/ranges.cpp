////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  ranges.cpp                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-07-01                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "ranges.h"

vector<double> generateRange(double a, double b, double c)
{
    vector<double> arr;
    while (a <= c)
    {
        arr.push_back(a);
        a += b;
    }
    return arr;
}

vector<double> linspaced(double a, double b, int n)
{
    vector<double> arr;
    double step = (b-a)/(n-1);

    while (a <= b)
    {
        arr.push_back(a);
        a += step;
    }
    return arr;
}


