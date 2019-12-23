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
};
