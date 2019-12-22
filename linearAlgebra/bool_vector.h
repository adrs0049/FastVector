////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  vector_detail.h                                               //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BOOL_VECTOR_DETAIL_H
#define BOOL_VECTOR_DETAIL_H

#include <bitset>

using std::bitset;

template <std::size_t N>
class BoolVector : public bitset<N>
{
public:
    using bitset<N>::all;
    using bitset<N>::flip;

    BoolVector()
        : bitset<N>()
    {}

    template< class CharT, class Traits, class Alloc>
    BoolVector(const std::basic_string<CharT, Traits, Alloc>& str,
               typename std::basic_string<CharT, Traits, Alloc>::size_type pos = 0,
               typename std::basic_string<CharT, Traits, Alloc>::size_type n =
               std::basic_string<CharT, Traits, Alloc>::npos,
               CharT zero = CharT('0'), CharT one = CharT('1'))
        : bitset<N>(str, n)
    {}

    template< class CharT >
    BoolVector(const CharT * str,
               typename std::basic_string<CharT>::size_type n =
               std::basic_string<CharT>::npos,
               CharT zero = CharT('0'), CharT one = CharT('1'))
        : bitset<N>(str, n)
    {}

    explicit operator bool() const { return all(); }
};

template <std::size_t N>
bool All(const BoolVector<N>& vector)
{
    return vector.all();
}

template <std::size_t N>
bool None(const BoolVector<N>& vector)
{
    return vector.none();
}

template <std::size_t N>
bool Any(const BoolVector<N>& vector)
{
    return vector.any();
}

template <std::size_t N>
BoolVector<N> operator~(const BoolVector<N>& vector)
{
    BoolVector<N> ret {vector};
    ret.flip();
    return ret;
}

template <std::size_t N>
BoolVector<N> operator!(const BoolVector<N>& vector)
{
    BoolVector<N> ret {vector};
    ret.flip();
    return ret;
}

#endif
