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

                auto res3 = Product(vec1, vec2);

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

                auto res3 = Product(vec1, vec2);

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

                auto res3 = Divide(vec1, vec2);

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

                auto res3 = Divide(vec1, vec2);

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
};
