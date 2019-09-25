////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  ranges.h                                                      //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-07-01                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef RANGES_H
#define RANGES_H

#include <vector>

using std::vector;

vector<double> generateRange(double a, double b, double c);
vector<double> linspaced(double a, double b, int n);

#endif
