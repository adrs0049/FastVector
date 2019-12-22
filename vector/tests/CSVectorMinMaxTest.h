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

#define REPEATS 100

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

        repeats = REPEATS;
    }

    bool keepGoing()
    {
        return (currentLength < vectorLength);
    }

public:

    void setUp()
    {
        repeats = REPEATS;
        currentLength = 1;
    }

    void tearDown()
    {}

    void testMaxQuick()
    {
        CSVector<double> vec {-128.427, -310.224, -693.769, -891.324, -105.034, -430.547, -826.372, -781.211};
        double maxVal = max(vec);
        auto max_elem = std::max_element(std::begin(vec), std::end(vec));
        TS_ASSERT_DELTA(maxVal, *max_elem, tol);
    }

    void testMax()
    {
        TS_TRACE("Starting maximum test");
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
};
