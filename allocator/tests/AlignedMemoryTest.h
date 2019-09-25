////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  AlignedMemoryTest.h                                           //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <cstdlib>

#include <cxxtest/TestSuite.h>
#include "../AlignedMemory.h"

using namespace std;
using namespace Memory;

class AlignedMemoryTest : public CxxTest::TestSuite
{
private:


public:

    void testAlignment()
    {
        size_t alignment = 32;
        size_t size = sizeof(double) * 1021 * 1023;
        void * ptr = malloc(size + sizeof(aligned_memory_header) + alignment);

        size_t value = reinterpret_cast<std::size_t>(ptr)
            + sizeof(aligned_memory_header);
        size_t offset = (value+alignment-1) & ~(alignment-1);
        size_t offset2 = offset - reinterpret_cast<size_t>(ptr);

        aligned_memory_header* header = reinterpret_cast<aligned_memory_header*>
            (static_cast<char*>(ptr) + offset2) - 1;

        header->offset = offset2;
        header->allocated_size = size +sizeof(aligned_memory_header) + alignment;

        free(ptr);
    }

    void setUp()
    {}

    void tearDown()
    {}

};

