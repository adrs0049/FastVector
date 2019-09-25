// test
#define _NO_CORE_

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <random>
#include <functional>

#include "ranges.h"

#define private public
#define protected public
#include "CSVector.h"

using namespace std;

class CSVectorTest : public CxxTest::TestSuite
{
private:
    const double tol = 1.e-8;

    int repeats;
    size_t currentLength;
    size_t size_step = 256 * 1024;

    const size_t defaultSize  = 100;
    const double defaultValue = 1.;

    const size_t mbytes = 8;
    const size_t vectorLength = mbytes * 1024 * 1024 / sizeof(double);
    //const size_t vectorLength = 32; //mbytes * 1024 * 1024 / sizeof(double);

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
    CSVector<T> add(const CSVector<T>& lhs, const CSVector<T>& rhs)
    {
        assert(lhs.size() == rhs.size());
        CSVector<T> ret (lhs.size());
        std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::plus<T>());
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
    CSVector<T> product(const CSVector<T>& lhs, const T& rhs)
    {
        CSVector<T> ret (lhs.size());
        std::transform(lhs.begin(), lhs.end(), ret.begin(),
                       std::bind1st(std::multiplies<T>(), rhs));
        return ret;
    }

    template <class T>
    CSVector<T> product(const CSVector<T>& lhs, const CSVector<T>& rhs)
    {
        CSVector<T> ret (lhs.size());
        std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::multiplies<T>());
        return ret;
    }

    template <class T>
    CSVector<T> divide(const CSVector<T>& lhs, const T& rhs)
    {
        CSVector<T> ret (lhs.size());
        std::transform(lhs.begin(), lhs.end(), ret.begin(),
                       std::bind2nd(std::divides<T>(), rhs));
        return ret;
    }

    template <class T>
    CSVector<T> divide(const T& lhs, const CSVector<T>& rhs)
    {
        CSVector<T> ret (rhs.size());
        std::transform(rhs.begin(), rhs.end(), ret.begin(),
                       std::bind1st(std::divides<T>(), lhs));
        return ret;
    }

    template <class T>
    CSVector<T> divide(const CSVector<T>& lhs, const CSVector<T>& rhs)
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

    void increaseLength()
    {
        if (currentLength <= 1024)
            currentLength++;
        else
        {
            currentLength += size_step;
            currentLength = std::min(currentLength, vectorLength);
        }

        repeats = 10;
    }

    bool keepGoing()
    {
        return (currentLength < vectorLength);
    }

public:

    void setUp()
    {
        repeats = 10;
        currentLength = 1;
    }

    void tearDown()
    {}

    void testDefaultConstructor()
    {
        TS_TRACE("Starting default constructor test");
        auto vec = CSVector<double>(defaultSize);

        TS_ASSERT_EQUALS(vec.mAllocationSize, defaultSize);
        TS_ASSERT_EQUALS(vec.mDataSize,       defaultSize);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart, defaultSize);
        TS_ASSERT_EQUALS(vec.data(), vec.mpStart);
        TS_ASSERT_EQUALS(vec.alignment(), __alignment);
        TS_ASSERT_DIFFERS(vec.mpStart, vec.mpEnd);
    }

    void testValueConstructor()
    {
        TS_TRACE("Starting default constructor test");
        auto vec = CSVector<double>(defaultSize, defaultValue);

        TS_ASSERT_EQUALS(vec.mAllocationSize, defaultSize);
        TS_ASSERT_EQUALS(vec.mDataSize,       defaultSize);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart, defaultSize);

        size_t N = vec.size();
        auto * data_ptr = vec.data();
        for (size_t i = 0; i < N; i++)
        {
            TS_ASSERT_EQUALS(vec(i), defaultValue);
            TS_ASSERT_EQUALS(vec[i], defaultValue);
            TS_ASSERT_EQUALS(vec.at(i), defaultValue);
            TS_ASSERT_EQUALS(data_ptr[i], defaultValue);
        }
    }

    void testValueAssignment()
    {
        TS_TRACE("Starting default constructor test");
        auto vec = CSVector<double>(defaultSize);

        TS_ASSERT_EQUALS(vec.mAllocationSize,       defaultSize);
        TS_ASSERT_EQUALS(vec.mDataSize,             defaultSize);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart,   defaultSize);

        vec = defaultValue;

        size_t N = vec.size();
        auto * data_ptr = vec.data();
        for (size_t i = 0; i < N; i++)
        {
            TS_ASSERT_EQUALS(vec(i), defaultValue);
            TS_ASSERT_EQUALS(vec[i], defaultValue);
            TS_ASSERT_EQUALS(vec.at(i), defaultValue);
            TS_ASSERT_EQUALS(data_ptr[i], defaultValue);
        }
    }

    void testCopyConstructor()
    {
        while (keepGoing())
        {
            TS_TRACE("Starting default constructor test");
            auto vec = getVectorIncNumbers<double>(currentLength);

            TS_ASSERT_EQUALS(vec.mAllocationSize,       currentLength);
            TS_ASSERT_EQUALS(vec.mDataSize,             currentLength);
            TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart,   currentLength);

            CSVector<double> vec2 (vec);

            TS_ASSERT_EQUALS(vec2.mAllocationSize,        currentLength);
            TS_ASSERT_EQUALS(vec2.mDataSize,              currentLength);
            TS_ASSERT_EQUALS(vec2.mpEnd - vec2.mpStart,   currentLength);

            TS_ASSERT_DIFFERS(vec.mpStart, vec2.mpStart);
            TS_ASSERT_DIFFERS(vec.mpEnd,   vec2.mpEnd);
            TS_ASSERT_EQUALS(vec.mDataSize,   vec2.mDataSize);
            TS_ASSERT_EQUALS(vec.mAllocationSize,   vec2.mAllocationSize);

            size_t N = vec.size();
            auto * data_ptr = vec.data();
            auto * data_ptr2 = vec2.data();
            for (size_t i = 0; i < N; i++)
            {
                TS_ASSERT_EQUALS(vec(i),        vec2(i));
                TS_ASSERT_EQUALS(vec[i],        vec2[i]);
                TS_ASSERT_EQUALS(vec.at(i),     vec2.at(i));
                TS_ASSERT_EQUALS(data_ptr[i],   data_ptr2[i]);
            }

            increaseLength();
        }
    }

    void testCopyAssignment()
    {
        TS_TRACE("Starting default constructor test");
        auto vec = getVectorIncNumbers<double>(vectorLength);

        TS_ASSERT_EQUALS(vec.mAllocationSize,       vectorLength);
        TS_ASSERT_EQUALS(vec.mDataSize,             vectorLength);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart,   vectorLength);

        CSVector<double> vec2 = vec;

        TS_ASSERT_EQUALS(vec2.mAllocationSize,       vectorLength);
        TS_ASSERT_EQUALS(vec2.mDataSize,             vectorLength);
        TS_ASSERT_EQUALS(vec2.mpEnd - vec2.mpStart,   vectorLength);

        TS_ASSERT_DIFFERS(vec.mpStart, vec2.mpStart);
        TS_ASSERT_DIFFERS(vec.mpEnd,   vec2.mpEnd);
        TS_ASSERT_EQUALS(vec.mDataSize,   vec2.mDataSize);
        TS_ASSERT_EQUALS(vec.mAllocationSize,   vec2.mAllocationSize);

        size_t N = vec.size();
        auto * data_ptr = vec.data();
        auto * data_ptr2 = vec2.data();
        for (size_t i = 0; i < N; i++)
        {
            TS_ASSERT_EQUALS(vec(i),        vec2(i));
            TS_ASSERT_EQUALS(vec[i],        vec2[i]);
            TS_ASSERT_EQUALS(vec.at(i),     vec2.at(i));
            TS_ASSERT_EQUALS(data_ptr[i],   data_ptr2[i]);
        }
    }

    void testMoveConstructor()
    {
        TS_TRACE("Starting default constructor test");
        auto vec  = getVectorIncNumbers<double>(vectorLength);
        auto vec3 = getVectorIncNumbers<double>(vectorLength);

        TS_ASSERT_EQUALS(vec.mAllocationSize,       vectorLength);
        TS_ASSERT_EQUALS(vec.mDataSize,             vectorLength);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart,   vectorLength);

        TS_ASSERT_EQUALS(vec3.mAllocationSize,       vectorLength);
        TS_ASSERT_EQUALS(vec3.mDataSize,             vectorLength);
        TS_ASSERT_EQUALS(vec3.mpEnd - vec3.mpStart,   vectorLength);

        auto * vec3_start = vec3.mpStart;
        auto * vec3_end   = vec3.mpEnd;
        CSVector<double> vec2 (std::move(vec3));

        TS_ASSERT_EQUALS(vec2.mAllocationSize,        vectorLength);
        TS_ASSERT_EQUALS(vec2.mDataSize,              vectorLength);
        TS_ASSERT_EQUALS(vec2.mpEnd - vec2.mpStart,   vectorLength);

        TS_ASSERT_EQUALS(vec3.mpStart, nullptr);
        TS_ASSERT_EQUALS(vec3.mpEnd,   nullptr);

        TS_ASSERT_EQUALS(vec2.mpStart, vec3_start);
        TS_ASSERT_EQUALS(vec2.mpEnd,   vec3_end);

        size_t N = vec.size();
        auto * data_ptr = vec.data();
        auto * data_ptr2 = vec2.data();
        for (size_t i = 0; i < N; i++)
        {
            TS_ASSERT_EQUALS(vec(i),        vec2(i));
            TS_ASSERT_EQUALS(vec[i],        vec2[i]);
            TS_ASSERT_EQUALS(vec.at(i),     vec2.at(i));
            TS_ASSERT_EQUALS(data_ptr[i],   data_ptr2[i]);
        }
    }

    void testMoveAssignment()
    {
        TS_TRACE("Starting default constructor test");
        auto vec  = getVectorIncNumbers<double>(vectorLength);
        auto vec3 = getVectorIncNumbers<double>(vectorLength);

        TS_ASSERT_EQUALS(vec.mAllocationSize,       vectorLength);
        TS_ASSERT_EQUALS(vec.mDataSize,             vectorLength);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart,   vectorLength);

        TS_ASSERT_EQUALS(vec3.mAllocationSize,       vectorLength);
        TS_ASSERT_EQUALS(vec3.mDataSize,             vectorLength);
        TS_ASSERT_EQUALS(vec3.mpEnd - vec3.mpStart,   vectorLength);

        auto * vec3_start = vec3.mpStart;
        auto * vec3_end   = vec3.mpEnd;
        CSVector<double> vec2 = std::move(vec3);

        TS_ASSERT_EQUALS(vec2.mAllocationSize,        vectorLength);
        TS_ASSERT_EQUALS(vec2.mDataSize,              vectorLength);
        TS_ASSERT_EQUALS(vec2.mpEnd - vec2.mpStart,   vectorLength);

        TS_ASSERT_EQUALS(vec3.mpStart, nullptr);
        TS_ASSERT_EQUALS(vec3.mpEnd,   nullptr);

        TS_ASSERT_EQUALS(vec2.mpStart, vec3_start);
        TS_ASSERT_EQUALS(vec2.mpEnd,   vec3_end);

        size_t N = vec.size();
        auto * data_ptr = vec.data();
        auto * data_ptr2 = vec2.data();
        for (size_t i = 0; i < N; i++)
        {
            TS_ASSERT_EQUALS(vec(i),        vec2(i));
            TS_ASSERT_EQUALS(vec[i],        vec2[i]);
            TS_ASSERT_EQUALS(vec.at(i),     vec2.at(i));
            TS_ASSERT_EQUALS(data_ptr[i],   data_ptr2[i]);
        }
    }

    void testInitList()
    {
        CSVector<double> test = {0,1,2,3,4,5,6,7,8,9};
        TS_ASSERT_EQUALS(test.size(), 10);
        for (size_t i = 0; i < test.size(); i++)
            TS_ASSERT_EQUALS(test[i], i);
    }

    void testAccessors()
    {
        TS_TRACE("Starting default constructor test");
        auto vec = getVectorIncNumbers<double>(vectorLength);

        TS_ASSERT_EQUALS(vec.mAllocationSize,     vectorLength);
        TS_ASSERT_EQUALS(vec.mDataSize,           vectorLength);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart, vectorLength);

        size_t N = vec.size();
        auto * data_ptr = vec.data();
        for (size_t i = 0; i < N; i++)
        {
            TS_ASSERT_EQUALS(vec(i),        double(i));
            TS_ASSERT_EQUALS(vec[i],        double(i));
            TS_ASSERT_EQUALS(vec.at(i),     double(i));
            TS_ASSERT_EQUALS(data_ptr[i],   double(i));
        }

        for (size_t i = N; i < N + 100; i++)
            TS_ASSERT_THROWS(vec.at(i),     std::out_of_range);
    }

    void testReallocateExpand()
    {
        TS_TRACE("Starting default constructor test");
        auto vec = getVectorIncNumbers<double>(vectorLength);

        TS_ASSERT_EQUALS(vec.mAllocationSize,     vectorLength);
        TS_ASSERT_EQUALS(vec.mDataSize,           vectorLength);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart, vectorLength);

        vec.resize(2 * vectorLength);

        TS_ASSERT_EQUALS(vec.mDataSize,           2 * vectorLength);
        TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart, 2 * vectorLength);
    }

    void testReallocateExpandChunk()
    {
        vector<unsigned long> chunkSizes {1,2,3,4,5,6,7,8,9,10,16,32,64,128,256,512,1024,2048};
        vector<unsigned long> factors    {2,3,4,5,6};

        for (unsigned long factor : factors)
        {
            for (unsigned long chunkSize : chunkSizes)
            {
                TS_TRACE("Starting default constructor test");
                auto vec = getVectorIncNumbers<double>(vectorLength);

                TS_ASSERT_EQUALS(vec.mAllocationSize,     vectorLength);
                TS_ASSERT_EQUALS(vec.mDataSize,           vectorLength);
                TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart, vectorLength);

                vec.resize(factor * vectorLength, chunkSize);

                TS_ASSERT_EQUALS(vec.mDataSize,           factor * vectorLength);
                TS_ASSERT_EQUALS(vec.mpEnd - vec.mpStart, factor * vectorLength);
                TS_ASSERT_EQUALS((vec.mAllocationSize - vectorLength) % chunkSize, 0);
            }
        }
    }

    void testDotProduct()
    {
        TS_TRACE("Starting dot product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                double d1 = dot(vec1, vec2);
                double d2 = dotProduct(vec1, vec2);

                TS_ASSERT_DELTA(d1, d2, 1e-2);
            }

            increaseLength();
        }
    }

    void testMax()
    {
        TS_TRACE("Starting minimum test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                double maxVal = max(vec1);

                auto max_elem = std::max_element(std::begin(vec1), std::end(vec1));

                TS_ASSERT_DELTA(maxVal, *max_elem, tol);
            }

            increaseLength();
        }
    }

    void testMaxNorm()
    {
        TS_TRACE("Starting sup norm test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                double maxVal = supNorm(vec1);

                // apply abs to vector
                auto absVec = abs(vec1);

                auto max_elem = std::max_element(std::begin(absVec), std::end(absVec));

                TS_ASSERT_DELTA(maxVal, *max_elem, tol);
            }

            increaseLength();
        }
    }

    void testMin()
    {
        TS_TRACE("Starting minimum test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                double minVal = min(vec1);

                auto min_elem = std::min_element(std::begin(vec1), std::end(vec1));

                TS_ASSERT_DELTA(minVal, *min_elem, tol);
            }

            increaseLength();
        }
    }

    void testAddition()
    {
        TS_TRACE("Starting addition product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                // careful this doesn't compute the value yet. Only the expression!
                auto res1 = vec1 + vec2;
                auto res2 = add(vec1, vec2);

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(res1[i], res2[i], tol);
            }

            increaseLength();
        }
    }

    void testAdditionVectorAssign()
    {
        TS_TRACE("Starting addition product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                CSVector<double> res1(vec1.size());
                res1 = vec1 + vec2;

                auto res2 = add(vec1, vec2);

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(res1[i], res2[i], tol);

                vec1 += vec2;

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(vec1[i], res2[i], tol);
            }

            increaseLength();
        }
    }

    void testAdditionAssign()
    {
        TS_TRACE("Starting addition product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                auto res2 = add(vec1, vec2);

                // Now let's check this one
                vec1 += vec2;

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(vec1[i], res2[i], tol);
            }

            increaseLength();
        }
    }

    void testSubstraction()
    {
        TS_TRACE("Starting subtract test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                CSVector<double> res1(currentLength);
                res1 = vec1 - vec2;
                auto res2 = substract(vec1, vec2);

                // now check this one
                vec1 -= vec2;

                for (size_t i = 0; i < currentLength; i++)
                {
                    TS_ASSERT_DELTA(res1[i], res2[i], tol);
                    TS_ASSERT_DELTA(vec1[i], res2[i], tol);
                }
            }

            increaseLength();
        }
    }

    void testSubstraction2()
    {
        TS_TRACE("Starting subtract test 2");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                auto res1 = vec1 - vec2;
                auto res2 = substract(vec1, vec2);

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(res1[i], res2[i], tol);
            }

            increaseLength();
        }
    }

    void testScalarProduct()
    {
        TS_TRACE("Starting product product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                CSVector<double> res1(vec1.size());
                res1 = vec1 * rnum;

                CSVector<double> res2(vec1.size());
                res2 = rnum * vec1;

                auto res3 = product(vec1, rnum);

                // now we can test this one
                vec1 *= rnum;

                for (size_t i = 0; i < currentLength; i++)
                {
                    TS_ASSERT_DELTA(res1[i], res3[i], tol);
                    TS_ASSERT_DELTA(res2[i], res3[i], tol);
                    TS_ASSERT_DELTA(vec1[i], res3[i], tol);
                }
            }

            increaseLength();
        }
    }

    void testProduct()
    {
        TS_TRACE("Starting product product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                CSVector<double> res1(vec1.size());
                res1 = vec1 * vec2;

                auto res3 = product(vec1, vec2);

                // now we can test this one
                vec1 *= vec2;

                for (size_t i = 0; i < currentLength; i++)
                {
                    TS_ASSERT_DELTA(res1[i], res3[i], tol);
                    TS_ASSERT_DELTA(vec1[i], res3[i], tol);
                }
            }

            increaseLength();
        }
    }

    void testProduct2()
    {
        TS_TRACE("Starting product product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                CSVector<double> res1(vec1.size());
                res1 = rnum;

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(res1(i), rnum, tol);

                res1 = vec1 * vec2;

                auto res3 = product(vec1, vec2);

                // now we can test this one
                vec1 *= vec2;

                for (size_t i = 0; i < currentLength; i++)
                {
                    TS_ASSERT_DELTA(res1[i], res3[i], tol);
                    TS_ASSERT_DELTA(vec1[i], res3[i], tol);
                }
            }

            increaseLength();
        }
    }

    void testScalarDivide()
    {
        TS_TRACE("Starting scalar divide test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                CSVector<double> res1(vec1.size());
                res1 = vec1 / rnum;

                CSVector<double> res2(vec1.size());
                res2 = rnum / vec1;

                auto res3 = divide(vec1, rnum);
                auto res4 = divide(rnum, vec1);

                // now we can test this one
                vec1 /= rnum;

                for (size_t i = 0; i < currentLength; i++)
                {
                    TS_ASSERT_DELTA(res1[i], res3[i], tol);
                    TS_ASSERT_DELTA(res2[i], res4[i], tol);
                    TS_ASSERT_DELTA(vec1[i], res3[i], tol);
                }
            }

            increaseLength();
        }
    }

    void testDivide()
    {
        TS_TRACE("Starting divide test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                CSVector<double> res1(vec1.size());
                res1 = vec1 / vec2;

                auto res3 = divide(vec1, vec2);

                // now we can test this one
                vec1 /= vec2;

                for (size_t i = 0; i < currentLength; i++)
                {
                    TS_ASSERT_DELTA(res1[i], res3[i], tol);
                    TS_ASSERT_DELTA(vec1[i], res3[i], tol);
                }
            }

            increaseLength();
        }
    }

    void testDivide2()
    {
        TS_TRACE("Starting product product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                CSVector<double> res1(vec1.size());
                res1 = rnum;

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(res1(i), rnum, tol);

                res1 = vec1 / vec2;

                auto res3 = divide(vec1, vec2);

                // now we can test this one
                vec1 /= vec2;

                for (size_t i = 0; i < currentLength; i++)
                {
                    TS_ASSERT_DELTA(res1[i], res3[i], tol);
                    TS_ASSERT_DELTA(vec1[i], res3[i], tol);
                }
            }

            increaseLength();
        }
    }

    void testComplicatedExpression1()
    {
        TS_TRACE("Starting complicated expression 1.");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);
                auto vec3 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                CSVector<double> ret(currentLength);
                ret = vec1 * vec2 + rnum * vec3;

                auto ref = complicated_expr1(vec1, vec2, vec3, rnum);

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(ref(i), ret(i), tol);
            }

            increaseLength();
        }
    }

    void testComplicatedExpression2()
    {
        TS_TRACE("Starting complicated expression 1.");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                auto ref = complicated_expr2(vec1, vec2, rnum);

                vec1 += rnum * vec2;

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(ref(i), vec1(i), tol);
            }

            increaseLength();
        }
    }

    void testComplicatedExpression3()
    {
        TS_TRACE("Starting complicated expression 1.");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                auto ref = complicated_expr3(vec1, vec2, rnum);
                vec1 -= rnum * vec2;

                for (size_t i = 0; i < currentLength; i++)
                    TS_ASSERT_DELTA(ref(i), vec1(i), tol);
            }

            increaseLength();
        }
    }

    void testNorm()
    {
        TS_TRACE("Starting norm test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);

                auto norm1 = norm(vec1);
                auto norm2 = getNorm(vec1);

                TS_ASSERT_DELTA(norm1, norm2, 1e4 * tol);
            }

            increaseLength();
        }
    }

    void testTriple1()
    {
        TS_TRACE("Starting tripled product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);
                auto vec3 = getVectorRandom<double>(currentLength);

                double value1 = triple(vec1, vec2, vec3);
                double value2 = tripleProduct(vec1, vec2, vec3);

                TS_ASSERT_DELTA(value1, value2, tol);
            }

            increaseLength();
        }
    }

    void testTriple2()
    {
        TS_TRACE("Starting tripled product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);

                double value1 = triple(vec1, vec2, vec2);
                double value2 = tripleProduct(vec1, vec2, vec2);

                TS_ASSERT_DELTA(value1, value2, tol);
            }

            increaseLength();
        }
    }

};

