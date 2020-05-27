////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  vector_detail.h                                               //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef VECTOR_DETAIL_H
#define VECTOR_DETAIL_H

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <locale>
#include <functional>
#include <cmath>
#include <numeric>
#include <utility>
#include <initializer_list>
#include <stdexcept>

#include "../concepts/concepts.h"
#include "../utils/CSVReader.h"
#include "../utils/macros.h"

#include "VectorTraits.h"
#include "VectorOperations.h"
#include "VectorExpression.h"
#include "VectorOperations.h"
#include "VectorReduction.h"

#include "bool_vector.h"

using namespace std::placeholders;
using namespace Expression;


template <typename Derived, typename T, std::size_t N>
struct BaseConstantVector : public VectorExpression<BaseConstantVector<Derived, T, N>>
{
    static_assert(N > 1, "BaseConstantVector must have more than one element!");
    static_assert(Arithmetic<T>(), "BaseConstantVector must have an arithmetic type as base!");

    using size_type              = std::size_t;
    using value_type             = T;
    using UnderlyingType         = T;
    using reverse_iterator       = typename std::reverse_iterator<T*>;
    using const_reverse_iterator = typename std::reverse_iterator<const T*>;

    // by default we just do nothing
    BaseConstantVector()
    {}

    BaseConstantVector(T (&data)[N]) // avoid decay to pointer
    {
        std::copy(std::begin(data), std::end(data), begin());
    }

    BaseConstantVector(const BaseConstantVector& vector)
    {
        std::copy(vector.cbegin(), vector.cend(), begin());
    }

    BaseConstantVector(BaseConstantVector&& vector)
    {
        this->swap(vector);
    }

    BaseConstantVector(const T value)
    {
        set(value);
    }

    template <class InputIt>
    BaseConstantVector(InputIt first, InputIt last, std::size_t pos = 0)
        : BaseConstantVector(T(0))
    {
        auto d_first = begin() + pos;
        while (first != last && d_first != end())
            *d_first++ = *first++;

        // This checks whether more data was provided using the InputIterator if
        // so report an error!
        ASSERT(std::distance(first, last) == 0, "Input iterator with size " +
               std::to_string(std::distance(first, last) + N) +
               " is larger than the container" + " size " +
               std::to_string(N) + ".");
    }

    BaseConstantVector(std::initializer_list<T> ilist)
        : BaseConstantVector(T(0)) // this make sure that if values.size() < N we have initialized values
    {
        ASSERT(ilist.size() <= N, "Initializer list with size " +
               std::to_string(ilist.size()) + " is larger than the container" +
               " size " + std::to_string(N) + ".");
        std::copy(ilist.begin(), ilist.end(), begin());
    }

    // Assignment operator for the use with template expressions
    template <class Other, typename = Disable_if<
        Some(Same<Other, BaseConstantVector<Derived, T, N> >(),
        BaseOf<BaseConstantVector<Derived, T, N>, Other >()) > > // We need this otherwise this turns on when the derived type is assigned!
    BaseConstantVector(const Other& that)
    {
        VectorAssignment<BaseConstantVector<Derived, T, N>, Other>()(static_cast<BaseConstantVector<Derived, T, N>&>(*this), that);
    }

    template <typename U,
              typename = Enable_if<All(Convertible<U, T>())> >
    void assign(const U& value)
    {
        std::fill(begin(), end(), value);
    }

    // Note this function will only take the first N values
    // as InputIt may not be a RandomAccessIterator we cant use std::distance
    template <class InputIt>
    void assign(InputIt first, InputIt last)
    {
        auto d_first = begin();
        while (first != last && d_first != end())
            *d_first++ = *first++;

        // This checks whether more data was provided using the InputIterator if
        // so report an error!
        ASSERT(std::distance(first, last) == 0, "Input iterator with size " +
               std::to_string(std::distance(first, last) + N) +
               " is larger than the container" + " size " +
               std::to_string(N) + ".");
    }

    template <typename U, typename = Enable_if<Convertible<T, U>()> >
    void assign(std::initializer_list<U> ilist)
    {
        ASSERT(ilist.size() <= N, "Initializer list with size " +
               std::to_string(ilist.size()) + " is larger than the container" +
               " size " + std::to_string(N) + ".");
        std::copy(ilist.begin(), ilist.end(), begin());
    }

    BaseConstantVector& operator=(const BaseConstantVector& vector)
    {
        std::copy(vector.cbegin(), vector.cend(), begin());
        return *this;
    }

    BaseConstantVector& operator=(BaseConstantVector&& vector)
    {
        swap(vector, *this);
        return *this;
    }

    BaseConstantVector& operator=(const T value)
    {
        set(value);
        return *this;
    }

    // Assignment operator for the use with template expressions
    template <class Other>
    Disable_if<Some(Same<Other, BaseConstantVector<Derived, T, N> >(),
        BaseOf<BaseConstantVector<Derived, T, N>, Other >()),
        typename VectorAssignment<BaseConstantVector<Derived, T, N>, Other>::type>
    operator=(const Other& that)
    {
        return VectorAssignment<BaseConstantVector<Derived, T, N>, Other>()(static_cast<BaseConstantVector<Derived, T, N>&>(*this), that);
    }

    T* begin() { return static_cast<Derived*>(this)->begin(); }
    const T* begin()  const { return static_cast<Derived*>(this)->begin(); }
    const T* cbegin() const { return static_cast<const Derived*>(this)->cbegin(); }

    reverse_iterator rbegin() { return static_cast<Derived*>(this)->rbegin(); }
    const_reverse_iterator crbegin() const { return static_cast<const Derived*>(this)->crbegin(); }

    T* end() { return static_cast<Derived*>(this)->end(); }
    const T* end()  const { return static_cast<Derived*>(this)->end(); }
    const T* cend() const { return static_cast<const Derived*>(this)->cend(); }

    reverse_iterator rend() { return static_cast<Derived*>(this)->rend(); }
    const_reverse_iterator crend() const { return static_cast<const Derived*>(this)->crend(); }

    T* data() { return begin(); }
    const T* data() const { return cbegin(); }

    constexpr std::size_t size() const
    {
        return static_cast<const Derived*>(this)->size();
    }

    std::size_t memory_size() const
    {
        return static_cast<const Derived*>(this)->memory_size();
    }

    // Access operator
    T& operator()(const std::size_t i) { return *(begin() + i); }
    const T& operator()(const std::size_t i) const { return *(cbegin() + i); }

    T& operator[](const std::size_t i) { return *(begin() + i); }
    const T& operator[](const std::size_t i) const { return *(cbegin() + i); }

    T& at(const std::size_t i)
    {
        if (!(i < size()))
            throw std::out_of_range("Index " + std::to_string(i) + " larger than "
                                    + std::to_string(size()));

        return operator[](i);
    }

    const T& at(const std::size_t i) const
    {
        if (!(i < size()))
            throw std::out_of_range("Index " + std::to_string(i) + " larger than "
                                    + std::to_string(size()));

        return operator[](i);
    }

    T& front() { return operator[](0); }
    const T& front() const { return operator[](0); }

    T& back()
    {
        return operator[](size() - 1);
    }

    const T& back() const
    {
        return operator[](size() - 1);
    }

    void swap(BaseConstantVector& other)
    {
        std::swap_ranges(begin(), end(), other.begin());
    }

    void set(const T value)
    {
        std::fill(begin(), end(), value);
    }

    constexpr void Reset()
    {
        set(T(0));
    }
};

template <typename Derived, typename T, std::size_t N>
inline constexpr typename BaseConstantVector<Derived, T, N>::size_type
size(const BaseConstantVector<Derived, T, N>& vector)
{
    return vector.size();
}

template <typename T, std::size_t N>
struct ConstantVector : public BaseConstantVector<ConstantVector<T, N>, T, N>
{
    static_assert(N > 1, "BaseConstantVector must have more than one element!");

    using reverse_iterator       = typename std::reverse_iterator<T*>;
    using const_reverse_iterator = typename std::reverse_iterator<const T*>;

    // constructors
    constexpr ConstantVector()
        : BaseConstantVector<ConstantVector<T, N>, T, N>(T(0))
    {}

    explicit ConstantVector(T (&data)[N])
        : BaseConstantVector<ConstantVector<T, N>, T, N>(data)
    {}

    ConstantVector(const ConstantVector& vector)
        : BaseConstantVector<ConstantVector<T, N>, T, N>(vector)
    {}

    ConstantVector(ConstantVector&& vector)
        : BaseConstantVector<ConstantVector<T, N>, T, N>(vector)
    {}

    explicit ConstantVector(const T value)
        : BaseConstantVector<ConstantVector<T, N>, T, N>(value)
    {}

    ConstantVector(std::initializer_list<T> values)
        : BaseConstantVector<ConstantVector<T, N>, T, N>(values)
    {}

    template <class InputIt>
    ConstantVector(InputIt first, InputIt last, std::size_t pos = 0)
        : BaseConstantVector<ConstantVector<T, N>, T, N>(first, last, pos)
    {}

    template <class Other, typename = Disable_if<
        Some(Same<Other, ConstantVector<T, N> >(),
             BaseOf<BaseConstantVector<ConstantVector<T, N>, T, N>, Other >()) > > // We need this otherwise this turns on when the base type is assigned -> leading to mem corruption
    ConstantVector(const Other& that)
        : BaseConstantVector<ConstantVector<T, N>, T, N>(that)
    {}

    ConstantVector& operator=(const ConstantVector& vector)
    {
        BaseConstantVector<ConstantVector<T, N>, T, N>::operator = (vector);
        return *this;
    }

    ConstantVector& operator=(ConstantVector&& vector)
    {
        BaseConstantVector<ConstantVector<T, N>, T, N>::operator = (vector);
        return *this;
    }

    ConstantVector& operator=(const T value)
    {
        BaseConstantVector<ConstantVector<T, N>, T, N>::operator = (value);
        return *this;
    }

    // Assignment operator for the use with template expressions
    template <class Other>
    Disable_if<Same<Other, ConstantVector<T, N> >(), typename VectorAssignment<ConstantVector<T, N>, Other>::type>
    operator=(const Other& that)
    {
        return VectorAssignment<ConstantVector<T, N>, Other>()(static_cast<ConstantVector<T, N>&>(*this), that);
    }

    ~ConstantVector() {}

    // Iterator access
    T* begin() { return std::begin(m_data); }
    const T* begin()  const { return std::begin(m_data); }
    const T* cbegin() const { return std::cbegin(m_data); }

    reverse_iterator rbegin() { return std::rbegin(m_data); }
    const_reverse_iterator crbegin() const { return std::crbegin(m_data); }

    T* end() { return std::end(m_data); }
    const T* end()  const { return std::end(m_data); }
    const T* cend() const { return std::cend(m_data); }

    reverse_iterator rend() { return std::rend(m_data); }
    const_reverse_iterator crend() const { return std::crend(m_data); }

    constexpr std::size_t size() const { return N; }
    std::size_t memory_size() const { return sizeof(*this); }

    T m_data[N];
};

template<typename T>
struct ConstantVector<T, 2> : public BaseConstantVector<ConstantVector<T, 2>, T, 2>
{
    using reverse_iterator       = typename std::reverse_iterator<T*>;
    using const_reverse_iterator = typename std::reverse_iterator<const T*>;

    // constructors
    ConstantVector()
        : BaseConstantVector<ConstantVector<T, 2>, T, 2>(T(0))
    {}

    ConstantVector(T (&data)[2])
        : BaseConstantVector<ConstantVector<T, 2>, T, 2>(data)
    {}

    ConstantVector(const ConstantVector& vector)
        : BaseConstantVector<ConstantVector<T, 2>, T, 2>(vector)
    {}

    ConstantVector(ConstantVector&& vector)
        : BaseConstantVector<ConstantVector<T, 2>, T, 2>(vector)
    {}

    explicit ConstantVector(const T value)
        : BaseConstantVector<ConstantVector<T, 2>, T, 2>(value)
    {}

    ConstantVector(std::initializer_list<T> values)
        : BaseConstantVector<ConstantVector<T, 2>, T, 2>(values)
    {}

    template <class Other, typename = Disable_if<
        Some(Same<Other, ConstantVector<T, 2> >(),
             BaseOf<BaseConstantVector<ConstantVector<T, 2>, T, 2>, Other >()) > > // We need this otherwise this turns on when the base type is assigned -> leading to mem corruption
    ConstantVector(const Other& that)
        : BaseConstantVector<ConstantVector<T, 2>, T, 2>(that)
    {}

    ConstantVector& operator=(const ConstantVector& vector)
    {
        BaseConstantVector<ConstantVector<T, 2>, T, 2>::operator = (vector);
        return *this;
    }

    ConstantVector& operator=(ConstantVector&& vector)
    {
        BaseConstantVector<ConstantVector<T, 2>, T, 2>::operator = (vector);
        return *this;
    }

    ConstantVector& operator=(const T value)
    {
        BaseConstantVector<ConstantVector<T, 2>, T, 2>::operator = (value);
        return *this;
    }

    // Assignment operator for the use with template expressions
    template <class Other>
    Disable_if<Same<Other, ConstantVector<T, 2> >(), typename VectorAssignment<ConstantVector<T, 2>, Other>::type>
    operator=(const Other& that)
    {
        return VectorAssignment<ConstantVector<T, 2>, Other>()(static_cast<ConstantVector<T, 2>&>(*this), that);
    }

    ConstantVector(T _x, T _y)
        : x(_x), y(_y)
    {}

    ~ConstantVector() {}

    union {
        T m_data[2];
        struct { T x, y; };
    };

    // Iterator access
    T* begin() { return std::begin(m_data); }
    const T* begin()  const { return std::begin(m_data); }
    const T* cbegin() const { return std::cbegin(m_data); }

    reverse_iterator rbegin() { return std::rbegin(m_data); }
    const_reverse_iterator crbegin() const { return std::crbegin(m_data); }

    T* end() { return std::end(m_data); }
    const T* end()  const { return std::end(m_data); }
    const T* cend() const { return std::cend(m_data); }

    reverse_iterator rend() { return std::rend(m_data); }
    const_reverse_iterator crend() const { return std::crend(m_data); }

    constexpr std::size_t size() const { return 2; }
    std::size_t memory_size() const { return sizeof(*this); }
};

template<typename T>
struct ConstantVector<T, 3> : public BaseConstantVector<ConstantVector<T, 3>, T, 3>
{
    using reverse_iterator       = typename std::reverse_iterator<T*>;
    using const_reverse_iterator = typename std::reverse_iterator<const T*>;

    // constructors
    constexpr ConstantVector()
        : BaseConstantVector<ConstantVector<T, 3>, T, 3>(T(0))
    {}

    explicit ConstantVector(T (&data)[3])
        : BaseConstantVector<ConstantVector<T, 3>, T, 3>(data)
    {}

    ConstantVector(const ConstantVector& vector)
        : BaseConstantVector<ConstantVector<T, 3>, T, 3>(vector)
    {}

    ConstantVector(ConstantVector&& vector)
        : BaseConstantVector<ConstantVector<T, 3>, T, 3>(vector)
    {}

    explicit ConstantVector(const T value)
        : BaseConstantVector<ConstantVector<T, 3>, T, 3>(value)
    {}

    ConstantVector(std::initializer_list<T> values)
        : BaseConstantVector<ConstantVector<T, 3>, T, 3>(values)
    {}

    template <class InputIt>
    ConstantVector(InputIt first, InputIt last)
        : BaseConstantVector<ConstantVector<T, 3>, T, 3>(first, last)
    {}

    template <class Other, typename = Disable_if<
        Some(Same<Other, ConstantVector<T, 3> >(),
             BaseOf<BaseConstantVector<ConstantVector<T, 3>, T, 3>, Other >()) > > // We need this otherwise this turns on when the base type is assigned -> leading to mem corruption
    ConstantVector(const Other& that)
        : BaseConstantVector<ConstantVector<T, 3>, T, 3>(that)
    {}

    ConstantVector& operator=(const ConstantVector& vector)
    {
        BaseConstantVector<ConstantVector<T, 3>, T, 3>::operator = (vector);
        return *this;
    }

    ConstantVector& operator=(ConstantVector&& vector)
    {
        BaseConstantVector<ConstantVector<T, 3>, T, 3>::operator = (vector);
        return *this;
    }

    ConstantVector& operator=(const T value)
    {
        BaseConstantVector<ConstantVector<T, 3>, T, 3>::operator = (value);
        return *this;
    }

    ConstantVector(T _x, T _y, T _z)
        : x(_x), y(_y), z(_z)
    {}

    ~ConstantVector() {}

    union {
        T m_data[3];
        struct { T x, y, z; };
        struct { T r, g, b; };
        struct { T red, green, blue; };
        ConstantVector<T, 2> xy;
    };

    // Iterator access
    T* begin() { return std::begin(m_data); }
    const T* begin()  const { return std::begin(m_data); }
    const T* cbegin() const { return std::cbegin(m_data); }

    reverse_iterator rbegin() { return std::rbegin(m_data); }
    const_reverse_iterator crbegin() const { return std::crbegin(m_data); }

    T* end() { return std::end(m_data); }
    const T* end()  const { return std::end(m_data); }
    const T* cend() const { return std::cend(m_data); }

    reverse_iterator rend() { return std::rend(m_data); }
    const_reverse_iterator crend() const { return std::crend(m_data); }

    constexpr std::size_t size() const { return 3; }
    std::size_t memory_size() const { return sizeof(*this); }
};

template<typename T>
struct ConstantVector<T, 4> : public BaseConstantVector<ConstantVector<T, 4>, T, 4>
{
    using reverse_iterator       = typename std::reverse_iterator<T*>;
    using const_reverse_iterator = typename std::reverse_iterator<const T*>;

    // constructors
    constexpr ConstantVector()
        : BaseConstantVector<ConstantVector<T, 4>, T, 4>(T(0))
    {}

    explicit ConstantVector(T (&data)[4])
        : BaseConstantVector<ConstantVector<T, 4>, T, 4>(data)
    {}

    ConstantVector(const ConstantVector& vector)
        : BaseConstantVector<ConstantVector<T, 4>, T, 4>(vector)
    {}

    ConstantVector(ConstantVector&& vector)
        : BaseConstantVector<ConstantVector<T, 4>, T, 4>(vector)
    {}

    explicit ConstantVector(const T value)
        : BaseConstantVector<ConstantVector<T, 4>, T, 4>(value)
    {}

    ConstantVector(std::initializer_list<T> values)
        : BaseConstantVector<ConstantVector<T, 4>, T, 4>(values)
    {}

    template <class InputIt>
    ConstantVector(InputIt first, InputIt last, std::size_t pos = 0)
        : BaseConstantVector<ConstantVector<T, 4>, T, 4>(first, last, pos)
    {}

    template <class Other, typename = Disable_if<
        Some(Same<Other, ConstantVector<T, 4> >(),
             BaseOf<BaseConstantVector<ConstantVector<T, 4>, T, 4>, Other >()) > > // We need this otherwise this turns on when the base type is assigned -> leading to mem corruption
    ConstantVector(const Other& that)
        : BaseConstantVector<ConstantVector<T, 4>, T, 4>(that)
    {}

    ConstantVector& operator=(const ConstantVector& vector)
    {
        BaseConstantVector<ConstantVector<T, 4>, T, 4>::operator = (vector);
        return *this;
    }

    ConstantVector& operator=(ConstantVector&& vector)
    {
        BaseConstantVector<ConstantVector<T, 4>, T, 4>::operator = (vector);
        return *this;
    }

    ConstantVector& operator=(const T value)
    {
        BaseConstantVector<ConstantVector<T, 4>, T, 4>::operator = (value);
        return *this;
    }

    ConstantVector(T _x, T _y, T _z, T _w)
        : x(_x), y(_y), z(_z), w(_w)
    {}

    ~ConstantVector() {}

    union {
        T m_data[4];
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        struct { T red, green, blue, alpha; };
        ConstantVector<T, 2> xy;
        ConstantVector<T, 3> xyz;
        ConstantVector<T, 3> rgb;
    };

    // Iterator access
    T* begin() { return std::begin(m_data); }
    const T* begin()  const { return std::begin(m_data); }
    const T* cbegin() const { return std::cbegin(m_data); }

    reverse_iterator rbegin() { return std::rbegin(m_data); }
    const_reverse_iterator crbegin() const { return std::crbegin(m_data); }

    T* end() { return std::end(m_data); }
    const T* end()  const { return std::end(m_data); }
    const T* cend() const { return std::cend(m_data); }

    reverse_iterator rend() { return std::rend(m_data); }
    const_reverse_iterator crend() const { return std::crend(m_data); }

    constexpr std::size_t size() const { return 4; }
    std::size_t memory_size() const { return sizeof(*this); }
};

template <typename T, std::size_t N>
struct ConstantVectorView : public BaseConstantVector<ConstantVectorView<T, N>, T, N>
{
    static_assert(N > 1, "BaseConstantVector must have more than one element!");

    using reverse_iterator       = typename std::reverse_iterator<T*>;
    using const_reverse_iterator = typename std::reverse_iterator<const T*>;

    // constructors
    explicit ConstantVectorView(T * data)
        : m_data(data)
    {}

    ConstantVectorView(const ConstantVectorView& vector)
        : m_data(vector.m_data)
    {}

    ConstantVectorView(ConstantVectorView&& vector)
        : m_data(vector.m_data)
    {
        vector.data = nullptr;
    }

    ConstantVectorView(ConstantVector<T, N>& vector)
        : m_data(vector.data())
    {}

    ConstantVectorView(ConstantVector<T, N>&& vector)
        : m_data(vector.data())
    {}

    ConstantVectorView& operator=(const ConstantVector<T, N>& vector)
    {
        BaseConstantVector<ConstantVectorView<T, N>, T, N>::operator = (vector);
        return *this;
    }

    ConstantVectorView& operator=(ConstantVector<T, N>&& vector)
    {
        BaseConstantVector<ConstantVectorView<T, N>, T, N>::operator = (vector);
        return *this;
    }

    ConstantVectorView& operator=(const T value)
    {
        BaseConstantVector<ConstantVectorView<T, N>, T, N>::operator = (value);
        return *this;
    }

    ConstantVectorView& operator=(const ConstantVectorView& vector)
    {
        m_data = vector.m_data;
        return *this;
    }

    ConstantVectorView& operator=(ConstantVectorView&& vector)
    {
        m_data = vector.m_data;
        return *this;
    }

    ~ConstantVectorView() {}

    // Iterator access
    T* begin() { return m_data; }
    const T* begin()  const { return m_data; }
    const T* cbegin() const { return m_data; }

    // reverse_iterator rbegin() { return std::rbegin(data); }
    // const_reverse_iterator crbegin() const { return std::crbegin(data); }

    T* end() { return m_data + N; }
    const T* end()  const { return m_data + N; }
    const T* cend() const { return m_data + N; }

    // reverse_iterator rend() { return std::rend(data); }
    // const_reverse_iterator crend() const { return std::crend(data); }

    constexpr std::size_t size() const { return N; }
    std::size_t memory_size() const { return sizeof(*this); }

    // Since this is a view it only has a pointer to the data!
    // And the pointer should never have to change!
    T * m_data;
};

// swap
template <typename Derived, typename T, std::size_t N>
void swap(BaseConstantVector<Derived, T, N>& lhs, BaseConstantVector<Derived, T, N>& rhs)
{
    lhs.swap(rhs);
}

// Wraps an angle around a given maximum.
template <typename T,
          typename = Enable_if<Floating_Point<T>()> >
inline T wrap_angle(const T angle, const T angleMaximum)
{
    double ret = fmod(angle, angleMaximum);
    if (ret < 0)
        ret += angleMaximum;
    return ret;
}

// Find whether the clock-wise or counter-clock wise direction is the smaller
// angle between two vectors.
template <typename T,
          typename = Enable_if<Floating_Point<T>()> >
inline T rotationDirection(const T angle, const T targetAngle)
{
    assert((targetAngle>=0.)&&(targetAngle<2.*M_PI));
    assert((angle>=0.)&&(angle<2.*M_PI));
    const T diffAngle = abs(angle - targetAngle);
    if (angle < targetAngle)
        return (diffAngle < M_PI) ? 1. : -1;
    else
        return (diffAngle < M_PI) ? -1. : 1;
}

/// These are implemented to avoid the abstraction penalty of the auto unrolling functions
template <typename T>
auto inline Norm(const ConstantVector<T, 2>& vector)
{
    return sqrt(Norm2Squared(vector));
}

template <typename T>
auto inline Norm(const ConstantVector<T, 3>& vector)
{
    return sqrt(Norm2Squared(vector));
}

template <typename T>
auto inline Norm(const ConstantVector<T, 4>& vector)
{
    return sqrt(Norm2Squared(vector));
}

template <typename T>
auto inline Norm2Squared(const ConstantVector<T, 2>& vector)
{
    return vector.x * vector.x + vector.y * vector.y;
}

template <typename T>
auto inline Norm2Squared(const ConstantVector<T, 3>& vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

template <typename T>
auto inline Norm2Squared(const ConstantVector<T, 4>& vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
}

// Need to do this here since we need to construct the correct Derived class
template <typename Derived, typename T, std::size_t N>
Derived inline Normalize(const BaseConstantVector<Derived, T, N>& vector)
{
    return {vector / Norm(vector)};
}

// TODO turn on only for signed types!
template <typename T, std::size_t N>
auto Invert(const ConstantVector<T, N>& vector)
{
    return -vector;
}

template <typename T, std::size_t N>
auto UnitVector(const ConstantVector<T, N>& vector)
{
    auto norm = Norm(vector);
    if (norm==0.)
        return vector;

    return vector / norm;
}

// If condition true select lhs, otherwise rhs
template <typename T, std::size_t N>
ConstantVector<T, N> select(const BoolVector<N>& condition,
                         const ConstantVector<T, N>& lhs,
                         const ConstantVector<T, N>& rhs)
{
    ConstantVector<T, N> ret;
    for (std::size_t i = 0; i < N; ++i)
        ret[i] = (condition[i]) ? lhs[i] : rhs[i];
    return ret;
}

template <typename T,
          typename = Enable_if<Floating_Point<T>()> >
ConstantVector<T, 2> UnitVector(const T theta)
{
    return {cos(theta), sin(theta)};
}

template <typename T, std::size_t N>
T sum(const ConstantVector<T, N>& vector)
{
    return std::accumulate(vector.begin(), vector.end(), T(0));
}

// Theta is the inclination, while phi is the azimuth
template <typename T,
          typename = Enable_if<Floating_Point<T>()> >
ConstantVector<T, 3> UnitVector(const T theta, const T phi)
{
    double t = wrap_angle(theta, M_PI);
    double p = wrap_angle(phi,   2. * M_PI);

    assert((0. <= t) && (t <= M_PI));
    assert((0. <= p) && (p <= 2. * M_PI));

    double sinTheta = sin(t);
    return {sinTheta * cos(p), sinTheta * sin(p), cos(t)};
}

// Theta is the inclination, while phi is the azimuth
template <typename T,
          typename = Enable_if<Floating_Point<T>()> >
std::tuple<ConstantVector<T, 3>, ConstantVector<T, 3>,  ConstantVector<T, 3>>
BasisVectors(const T theta, const T phi)
{
    double t = wrap_angle(theta, M_PI);
    double p = wrap_angle(phi,   2. * M_PI);

    assert((0. <= t) && (t <= M_PI));
    assert((0. <= p) && (p <= 2. * M_PI));

    double cosPhi   = cos(p);
    double sinPhi   = sin(p);
    double sinTheta = sin(t);
    double cosTheta = cos(t);

    return {{cosPhi * cosTheta, sinPhi * cosTheta, - sinTheta},
            {-sinPhi, cosPhi, 0},
            {sinTheta * cosPhi, sinTheta * sinPhi, cosTheta}};
}

template <typename T, std::size_t N>
auto Dot(const ConstantVector<T, N>& lhs, const ConstantVector<T, N>& rhs)
{
    return dot<N>(lhs, rhs);
}

template <typename T>
auto Dot(const ConstantVector<T, 2>& lhs, const ConstantVector<T, 2>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T>
auto Dot(const ConstantVector<T, 3>& lhs, const ConstantVector<T, 3>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename T>
auto Dot(const ConstantVector<T, 4>& lhs, const ConstantVector<T, 4>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

template <typename T, typename Transform, std::size_t N>
ConstantVector<T, N> Apply(const Transform& transform,
                        const ConstantVector<T, N>& vector)
{
    return transform(vector);
}

template <typename T>
T Cross(const ConstantVector<T, 2>& lhs, const ConstantVector<T, 2>& rhs)
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

template <typename T>
ConstantVector<T, 3> Cross(const ConstantVector<T, 3>& lhs,
                        const ConstantVector<T, 3>& rhs)
{
    return {lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x};
}

template <typename T, std::size_t N>
auto minComponent(const ConstantVector<T, N>& vector)
{
    auto res = std::min_element(vector.cbegin(), vector.cend());
    return std::distance(std::cbegin(vector), res);
}

template <typename T, std::size_t N>
auto min(const ConstantVector<T, N>& vector)
{
    return *std::min_element(vector.cbegin(), vector.cend());
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
auto min(const ConstantVector<T, N>& vector, const U value)
{
    ConstantVector<T, N> ret;
    // min has too many overloads the compiler struggles
    T const& (*min) (T const&, T const&) = std::min<T>;
    std::transform(vector.cbegin(), vector.cend(), ret.begin(),
                   std::bind(min, _1, T(value)));
    return ret;
}

template <typename T, std::size_t N>
auto maxComponent(const ConstantVector<T, N>& vector)
{
    auto res = std::max_element(vector.cbegin(), vector.cend());
    return std::distance(std::cbegin(vector), res);
}

template <typename T, std::size_t N>
auto max(const ConstantVector<T, N>& vector)
{
    return *std::max_element(vector.cbegin(), vector.cend());
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
auto max(const ConstantVector<T, N>& vector, const U value)
{
    ConstantVector<T, N> ret;
    // min has too many overloads the compiler struggles
    T const& (*max) (T const&, T const&) = std::max<T>;
    std::transform(vector.cbegin(), vector.cend(), ret.begin(),
                   std::bind(max, _1, T(value)));
    return ret;
}

// Computes (x * y) + z
template <typename T, std::size_t N,
          typename = Enable_if<Floating_Point<T>()> >
auto fma(const ConstantVector<T, N>& x, const ConstantVector<T, N>& y,
         const ConstantVector<T, N>& z)
{
    // can we make this a AVX?
    ConstantVector<T, N> ret;
    for (std::size_t i = 0; i < N; i++)
        ret[i] = std::fma(x[i], y[i], z[i]);
    return ret;
}

template <typename T, std::size_t N,
          typename = Enable_if<Floating_Point<T>()> >
bool isfinite(const ConstantVector<T, N>& vector)
{
    bool (*isfinite) (const T) = std::isfinite;
    return std::all_of(vector.cbegin(), vector.cend(), std::bind(isfinite, _1));
}

template <typename T, std::size_t N,
          typename = Enable_if<Floating_Point<T>()> >
bool isnan(const ConstantVector<T, N>& vector)
{
    bool (*isnan) (const T) = std::isnan;
    return std::all_of(vector.cbegin(), vector.cend(), std::bind(isnan, _1));
}

template <typename T, std::size_t N,
          typename = Enable_if<Floating_Point<T>()> >
bool isinf(const ConstantVector<T, N>& vector)
{
    bool (*isinf) (const T) = std::isinf;
    return std::all_of(vector.cbegin(), vector.cend(), std::bind(isinf, _1));
}

template <typename T, std::size_t N,
          typename = Enable_if<Floating_Point<T>()> >
bool isnormal(const ConstantVector<T, N>& vector)
{
    bool (*isnormal) (const T) = std::isnormal;
    return std::all_of(vector.cbegin(), vector.cend(), std::bind(isnormal, _1));
}

template <typename T>
T clip(const T& n, const T& lower_bound, const T& upper_bound)
{
    return std::max(lower_bound, std::min(n, upper_bound));
}

template <typename T, std::size_t N>
ConstantVector<T, N> lerp(const ConstantVector<T, N>& start,
                       const ConstantVector<T, N> end, double t)
{
    return start + (end - start) * clip(t, 0., 1.);
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
auto clamp(const ConstantVector<T, N>& vector,
           const U lower_bound, const U upper_bound)
{
    ConstantVector<T, N> ret;
    std::transform(vector.cbegin(), vector.cend(), ret.begin(),
                   std::bind(clip<T>, _1, T(lower_bound), T(upper_bound)));
    return ret;
}

template <typename T, std::size_t N>
auto saturate(const ConstantVector<T, N>& vector)
{
    return clamp(vector, T(0), T(1));
}

// mathematical comparisions
template <typename T, std::size_t N>
BoolVector<N> operator==(const ConstantVector<T, N>& vec1,
                         const ConstantVector<T, N>& vec2)
{
    BoolVector<N> ret;
    for (std::size_t i = 0; i < N; ++i)
        ret[i] = (vec1[i] == vec2[i]);

    return ret;
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
bool operator==(const ConstantVector<T, N>& vector, const U value)
{
    return all_of(vector.cbegin(), vector.cend(),
                  std::bind1st(std::equal_to<T>(), value));
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
bool operator==(const U value, const ConstantVector<T, N>& vector)
{
    return (vector == value);
}

template <typename T, std::size_t N>
BoolVector<N> operator!=(const ConstantVector<T, N>& vec1,
                         const ConstantVector<T, N>& vec2)
{
    return !(vec1 == vec2);
}

// should return false if a single value does not equal value
template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
bool operator!=(const ConstantVector<T, N>& vector, const U value)
{
    return any_of(vector.cbegin(), vector.cend(),
                  std::bind1st(std::not_equal_to<T>(), value));
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
bool operator!=(const U value, const ConstantVector<T, N>& vector)
{
    return (vector != value);
}

template <typename T, std::size_t N>
BoolVector<N> operator<(const ConstantVector<T, N>& vec1,
                        const ConstantVector<T, N>& vec2)
{
    BoolVector<N> ret;
    for (std::size_t i = 0; i < N; ++i)
        ret[i] = (vec1[i] < vec2[i]);

    return ret;
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
bool operator<(const ConstantVector<T, N>& vector, const U value)
{
    return all_of(vector.cbegin(), vector.cend(),
                  std::bind2nd(std::less<T>(), T(value)));
}

template <typename T, std::size_t N>
BoolVector<N> operator<=(const ConstantVector<T, N>& vec1,
                         const ConstantVector<T, N>& vec2)
{
    return !(vec1 > vec2);
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
bool operator<=(const ConstantVector<T, N>& vector, const U value)
{
    return !(vector > T(value));
}

template <typename T, std::size_t N>
BoolVector<N> operator>(const ConstantVector<T, N>& vec1,
                        const ConstantVector<T, N>& vec2)
{
    return vec2 < vec1;
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
bool operator>(const ConstantVector<T, N>& vector, const U value)
{
    return all_of(vector.cbegin(), vector.cend(),
                  std::bind2nd(std::greater<T>(), T(value)));
}

template <typename T, std::size_t N>
BoolVector<N> operator>=(const ConstantVector<T, N>& vec1,
                         const ConstantVector<T, N>& vec2)
{
    return !(vec1 < vec2);
}

template <typename T, typename U, std::size_t N,
          typename = Enable_if<All(Convertible<U, T>())> >
bool operator>=(const ConstantVector<T, N>& vector, const U value)
{
    return !(vector < T(value));
}

// computes the determinant of matrix constructed of vectors a, b, c as rows
template <typename T>
auto determinant(const ConstantVector<T, 3>& a, const ConstantVector<T, 3>& b,
                 const ConstantVector<T, 3>& c)
{
    return a.x * (b.y * c.z - c.y * b.z) -
           a.y * (b.x * c.z - c.x * b.z) +
           a.z * (b.x * c.y - b.y * c.x);
}

template <typename T>
auto Angle(const ConstantVector<T, 2>& a, const ConstantVector<T, 2>& b)
{
    auto dot = Dot(a, b);
    auto crs = Cross(a, b);
    return atan2(crs, dot);
}

template <typename T>
auto AcuteAngle(const ConstantVector<T, 2>& a, const ConstantVector<T, 2>& b)
{
    auto dot = Dot(a, b);
    auto crs = Cross(a, b);
    return atan2(crs, std::abs(dot));
}

template <typename T>
auto Angle(const ConstantVector<T, 3>& a, const ConstantVector<T, 3>& b)
{
    auto dot = Dot(a, b);
    auto crs = Norm(Cross(a, b));
    return atan2(crs, dot);
}

template <typename T>
auto AcuteAngle(const ConstantVector<T, 3>& a, const ConstantVector<T, 3>& b)
{
    auto dot = std::abs(Dot(a, b));
    auto crs = Norm(Cross(a, b));
    return atan2(crs, dot);
}

template <typename T>
auto Angle2(const ConstantVector<T, 3>& a, const ConstantVector<T, 3>& b)
{
    auto n1 = Norm(a);
    auto n2 = Norm(b);
    auto vec1 = n2 * a - n1 * b;
    auto vec2 = n2 * a + n1 * b;
    return 2. * atan2(Norm(vec1), Norm(vec2));
}

template <typename T, std::size_t N>
auto EuclideanDistance(const ConstantVector<T, N>& a, const ConstantVector<T, N>& b)
{
    return Norm(a - b);
}

// input and output
template<typename T, typename Derived, std::size_t N, std::size_t Precision = 12>
std::ostream& operator<<(std::ostream& stream,  const BaseConstantVector<Derived, T, N>& vec)
{
    std::ostringstream ss;

    ss << "(";
    for (auto p = vec.cbegin(); p!=vec.cend() - 1; ++p)
        ss << std::setprecision(Precision) << *p << ", ";
    ss << std::setprecision(Precision) << *(vec.cend() - 1) << ")";

    stream << ss.str();
    return stream;
}

template<typename T, std::size_t N>
std::istream& operator>>(std::istream& stream,
                         BaseConstantVector<ConstantVector<T, N>, T, N>& vec)
{
    stream.imbue(std::locale(std::locale(), new csv_reader()));
    vec.assign(std::istream_iterator<T>(stream), std::istream_iterator<T>());
    return stream;
}

template <typename T, std::size_t N>
ConstantVector<T, N> zero()
{
    ConstantVector<T, N> zero;
    return zero;
}

template <typename T, std::size_t N>
ConstantVector<T, N> ones()
{
    ConstantVector<T, N> ones(T(1));
    return ones;
}

template <typename T, std::size_t N>
ConstantVector<T, N> linspace(T start, T end)
{
    ConstantVector<T, N> array;

    if (N == 0)
        return array;

    if (N == 1)
    {
        array[0] = start;
        return array;
    }

    // let's try this
    T delta = (end - start) / (N - 1);

    for (size_t i = 0; i < N - 1; i++)
        array[i] = start + delta * i;

    array[N-1] = end;

    return array;
}


#endif
