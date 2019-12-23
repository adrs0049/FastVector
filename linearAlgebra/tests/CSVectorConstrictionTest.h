// test
#define _NO_CORE_

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "CSVectorCommonTest.h"

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
