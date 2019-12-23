// test
#define _NO_CORE_

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "ranges.h"
#include "DynamicVectorCommonTest.h"

#define private public
#define protected public
#include "DynamicVector.h"

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

    void testScalarAdditionQuick()
    {
        TS_TRACE("Starting quick scalar addition test");
        CSVector<double> vec1 {0, 1, 2};

        double rnum = 2;

        CSVector<double> res1 (vec1.size());
        CSVector<double> res2 (vec1.size());

        res1 = rnum * vec1;
        res2 = vec1 * rnum;

        for (size_t i = 0; i < vec1.size(); ++i)
        {
            TS_ASSERT_DELTA(res1[i], vec1[i] * rnum, tol);
            TS_ASSERT_DELTA(res2[i], vec1[i] * rnum, tol);
        }
    }

    void testScalarAdditionQuick2()
    {
        TS_TRACE("Starting quick scalar addition test");
        CSVector<double> vec1 {0, 1, 2};

        double rnum = 2;

        CSVector<double> res1 = rnum * vec1;
        CSVector<double> res2 = vec1 * rnum;

        res1 = rnum * vec1;
        res2 = vec1 * rnum;

        for (size_t i = 0; i < vec1.size(); ++i)
        {
            TS_ASSERT_DELTA(res1[i], vec1[i] * rnum, tol);
            TS_ASSERT_DELTA(res2[i], vec1[i] * rnum, tol);
        }
    }

    void testScalarAddition()
    {
        TS_TRACE("Starting addition product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                CSVector<double> res1(vec1.size());
                res1 = vec1 + rnum;

                CSVector<double> res2(vec1.size());
                res2 = rnum + vec1;

                auto res3 = add(vec1, rnum);

                // now we can test this one
                vec1 += rnum;

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

    void testScalarSubstraction()
    {
        TS_TRACE("Starting minus product test");
        while (keepGoing())
        {
            while (repeats --> 0)
            {
                auto vec1 = getVectorRandom<double>(currentLength);
                auto vec2 = getVectorRandom<double>(currentLength);
                auto rnum = getRandomNumber<double>();

                CSVector<double> res1(vec1.size());
                res1 = vec1 - rnum;

                auto res3 = substract(vec1, rnum);

                CSVector<double> res2(vec1.size());
                res2 = rnum - vec1;

                auto res4 = substract(rnum, vec1);

                // check the other way around
                CSVector<double> vec3 (currentLength, rnum);
                vec3 -= vec1;

                // now we can test this one
                vec1 -= rnum;

                for (size_t i = 0; i < currentLength; i++)
                {
                    TS_ASSERT_DELTA(res1[i], res3[i], tol);
                    TS_ASSERT_DELTA(res2[i], res4[i], tol);
                    TS_ASSERT_DELTA(res2[i], vec3[i], tol);
                    TS_ASSERT_DELTA(vec1[i], res1[i], tol);
                }
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

                auto res3 = Product(vec1, rnum);

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

                auto res3 = Divide(vec1, rnum);
                auto res4 = Divide(rnum, vec1);

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
};
