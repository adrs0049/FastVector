#ifndef COMMON_TEST_H
#define COMMON_TEST_H

#include <random>
#include <algorithm>

#include "ranges.h"

#define private public
#define protected public
#include "CSVector.h"

using namespace std;


template <class T>
CSVector<T> getVectorIncNumbers(size_t length)
{
    CSVector<T> ret(length);
    auto * data_ptr = ret.data();
    for (size_t i = 0; i < length; i++)
        data_ptr[i] = T(i);

    return ret;
}

template <class T>
CSVector<T> getVectorRandom(size_t length)
{
    CSVector<T> ret(length);
    std::random_device rnd;
    std::mt19937 engine(rnd());
    std::uniform_real_distribution<T> dist(-1e3, 1e3);
    auto rngen = std::bind(dist, engine);
    std::generate(begin(ret), end(ret), rngen);
    return ret;
}

template <class T>
T getRandomNumber()
{
    std::random_device rnd;
    std::mt19937 engine(rnd());
    std::uniform_real_distribution<T> dist(-1., 1.);
    auto rngen = std::bind(dist, engine);
    return rngen();
}

template <class T>
T dotProduct(const CSVector<T>& lhs, const CSVector<T>& rhs)
{
    return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), T(0));
}

template <class T>
T tripleProduct(const CSVector<T>& a, const CSVector<T>& b, const CSVector<T>& c)
{
    T ret {0};
    size_t N = a.size();
    assert(N == b.size());
    assert(N == c.size());

    for (size_t i = 0; i < N; i++)
        ret += a(i) * b(i) * c(i);

    return ret;
}

template <class T>
T getNorm(const CSVector<T>& lhs)
{
    return sqrt(dotProduct(lhs, lhs));
}

template <class T>
CSVector<T> add(const CSVector<T>& lhs, const T& rhs)
{
    CSVector<T> ret (lhs.size());
    std::transform(lhs.begin(), lhs.end(), ret.begin(),
                   std::bind1st(std::plus<T>(), rhs));
    return ret;
}

template <class T>
CSVector<T> add(const CSVector<T>& lhs, const CSVector<T>& rhs)
{
    assert(lhs.size() == rhs.size());
    CSVector<T> ret (lhs.size());
    std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::plus<T>());
    return ret;
}

template <class T>
CSVector<T> substract(const CSVector<T>& lhs, const T& rhs)
{
    CSVector<T> ret (lhs.size());
    std::transform(lhs.begin(), lhs.end(), ret.begin(),
                   std::bind2nd(std::minus<T>(), rhs));
    return ret;
}

template <class T>
CSVector<T> substract(const T& lhs, const CSVector<T>& rhs)
{
    CSVector<T> ret (rhs.size());
    std::transform(rhs.begin(), rhs.end(), ret.begin(),
                   std::bind1st(std::minus<T>(), lhs));
    return ret;
}

template <class T>
CSVector<T> substract(const CSVector<T>& lhs, const CSVector<T>& rhs)
{
    assert(lhs.size() == rhs.size());
    CSVector<T> ret (lhs.size());
    std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::minus<T>());
    return ret;
}

template <class T>
CSVector<T> Product(const CSVector<T>& lhs, const T& rhs)
{
    CSVector<T> ret (lhs.size());
    std::transform(lhs.begin(), lhs.end(), ret.begin(),
                   std::bind1st(std::multiplies<T>(), rhs));
    return ret;
}

template <class T>
CSVector<T> Product(const CSVector<T>& lhs, const CSVector<T>& rhs)
{
    CSVector<T> ret (lhs.size());
    std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::multiplies<T>());
    return ret;
}

template <class T>
CSVector<T> Divide(const CSVector<T>& lhs, const T& rhs)
{
    CSVector<T> ret (lhs.size());
    std::transform(lhs.begin(), lhs.end(), ret.begin(),
                   std::bind2nd(std::divides<T>(), rhs));
    return ret;
}

template <class T>
CSVector<T> Divide(const T& lhs, const CSVector<T>& rhs)
{
    CSVector<T> ret (rhs.size());
    std::transform(rhs.begin(), rhs.end(), ret.begin(),
                   std::bind1st(std::divides<T>(), lhs));
    return ret;
}

template <class T>
CSVector<T> Divide(const CSVector<T>& lhs, const CSVector<T>& rhs)
{
    CSVector<T> ret (lhs.size());
    std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::divides<T>());
    return ret;
}

template <class T>
CSVector<T> complicated_expr1(const CSVector<T>& a, const CSVector<T>& b, const CSVector<T>& c, const double& d)
{
    size_t N = a.size();
    CSVector<T> ret (N);
    for (size_t i = 0; i < N; i++)
        ret(i) = a(i) * b(i) + d * c(i);

    return ret;
}

template <class T>
CSVector<T> complicated_expr2(const CSVector<T>& a, const CSVector<T>& b, const double& d)
{
    size_t N = a.size();
    CSVector<T> ret(N);
    for (size_t i = 0; i < N; i++)
        ret(i) = a(i);

    for (size_t i = 0; i < N; i++)
        ret(i) += d * b(i);

    return ret;
}

template <class T>
CSVector<T> complicated_expr3(const CSVector<T>& a, const CSVector<T>& b, const double& d)
{
    size_t N = a.size();
    CSVector<T> ret(N);
    for (size_t i = 0; i < N; i++)
        ret(i) = a(i);

    for (size_t i = 0; i < N; i++)
        ret(i) -= d * b(i);

    return ret;
}


#endif
