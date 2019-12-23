// test
#define _NO_CORE_

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <string>
#include <memory>
#include <random>
#include <vector>

#include "ranges.h"

#define private public
#define protected public
#include "../Vector.h"

#define REPEATS 10

using namespace std;

void vector_equals(Vector3d& vector, double value)
{
    TS_ASSERT_EQUALS((vector).x, (value));
    TS_ASSERT_EQUALS((vector).y, (value));
    TS_ASSERT_EQUALS((vector).z, (value));
}

class VectorTest : public CxxTest::TestSuite
{
private:
    double tol = 1.e-8;
    int repeats = 1;

    // some special vectors
    Vector3d ex {1,0,0};
    Vector3d ey {0,1,0};
    Vector3d ez {0,0,1};

    auto getVector2d()
    {
        return Vector2d(1., 2.);
    }

    auto getVector3d()
    {
        return Vector3d(1., 2., 3.);
    }

    auto getVector4d()
    {
        return Vector4d(1., 2., 3., 4.);
    }

    double norm2d()
    {
        return std::sqrt(1. + 2.*2.);
    }

    double norm3d()
    {
        return std::sqrt(1. + 2.*2. + 3.*3.);
    }

    double norm4d()
    {
        return std::sqrt(1. + 2.*2. + 3.*3. + 4.*4.);
    }

    auto getZero2d()
    {
        return Vector2d(0, 0);
    }

    auto getZero3d()
    {
        return Vector3d(0, 0, 0);
    }

    auto getZero4d()
    {
        return Vector4d(0, 0, 0, 0);
    }

    auto getUnit2d()
    {
        return Vector2d(1, 1);
    }

    auto getUnit3d()
    {
        return Vector3d(1, 1, 1);
    }

    auto getUnit4d()
    {
        return Vector4d(1, 1, 1, 1);
    }

    // some templated helpers
    template <class T, size_t N>
    VectorClass<T, N> getRandomVector()
    {
        VectorClass<T, N> ret;
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

    template <class T, size_t N>
    T dotProduct(const VectorClass<T, N>& lhs, const VectorClass<T, N>& rhs)
    {
        return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), T(0));
    }

    template <class T, size_t N>
    T getNorm(const VectorClass<T, N>& lhs)
    {
        return sqrt(dotProduct(lhs, lhs));
    }

    template <class T, size_t N>
    VectorClass<T, N> addition(const VectorClass<T, N>& lhs, const VectorClass<T, N>& rhs)
    {
        VectorClass<T, N> ret;
        std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::plus<T>());
        return ret;
    }

    template <class T, size_t N>
    VectorClass<T, N> substract(const VectorClass<T, N>& lhs, const VectorClass<T, N>& rhs)
    {
        VectorClass<T, N> ret;
        std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::minus<T>());
        return ret;
    }

    template <class T, size_t N>
    VectorClass<T, N> product(const VectorClass<T, N>& lhs, const T& rhs)
    {
        VectorClass<T, N> ret;
        std::transform(lhs.begin(), lhs.end(), ret.begin(),
                       std::bind1st(std::multiplies<T>(), rhs));
        return ret;
    }

    template <class T, size_t N>
    VectorClass<T, N> product(const VectorClass<T, N>& lhs, const VectorClass<T, N>& rhs)
    {
        VectorClass<T, N> ret;
        std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::multiplies<T>());
        return ret;
    }

    template <class T, size_t N>
    VectorClass<T, N> divide(const VectorClass<T, N>& lhs, const T& rhs)
    {
        VectorClass<T, N> ret;
        std::transform(lhs.begin(), lhs.end(), ret.begin(),
                       std::bind2nd(std::divides<T>(), rhs));
        return ret;
    }

    template <class T, size_t N>
    VectorClass<T, N> divide(const T& lhs, const VectorClass<T, N>& rhs)
    {
        VectorClass<T, N> ret;
        std::transform(rhs.begin(), rhs.end(), ret.begin(),
                       std::bind1st(std::divides<T>(), lhs));
        return ret;
    }

    template <class T, size_t N>
    VectorClass<T, N> divide(const VectorClass<T, N>& lhs, const VectorClass<T, N>& rhs)
    {
        VectorClass<T, N> ret;
        std::transform(lhs.begin(), lhs.end(), rhs.begin(), ret.begin(), std::divides<T>());
        return ret;
    }

    template <class T, size_t N>
    void additionTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1 = getRandomVector<T, N>();
            auto vec2 = getRandomVector<T, N>();

            // compute the to be compared results
            VectorClass<T, N> res1 = vec1 + vec2;
            auto res2 = addition(vec1, vec2);

            for (size_t i = 0; i < N; i++)
                TS_ASSERT_DELTA(res1[i], res2[i], tol);
        }

        repeats = REPEATS;
    }

    template <class T, size_t N>
    void additionScalarTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1   = getRandomVector<T, N>();
            auto scalar = getRandomNumber<T>();

            // compute the to be compared results
            VectorClass<T, N> res1 = vec1 + scalar;

            // and the other way around
            VectorClass<T, N> res3 = scalar + vec1;

            for (size_t i = 0; i < N; i++)
            {
                TS_ASSERT_DELTA(res1[i], vec1[i] + scalar, tol);
                TS_ASSERT_DELTA(res3[i], scalar + vec1[i], tol);
            }
        }

        repeats = REPEATS;
    }

    template <class T, size_t N>
    void substractionTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1 = getRandomVector<T, N>();
            auto vec2 = getRandomVector<T, N>();

            // compute the to be compared results
            VectorClass<T, N> res1 = vec1 - vec2;
            auto res2 = substract(vec1, vec2);

            for (size_t i = 0; i < N; i++)
                TS_ASSERT_DELTA(res1[i], res2[i], tol);
        }

        repeats = REPEATS;
    }

    template <class T, size_t N>
    void substractionScalarTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1   = getRandomVector<T, N>();
            auto scalar = getRandomNumber<T>();

            // compute the to be compared results
            VectorClass<T, N> res1 = vec1 - scalar;

            // and the other way around
            VectorClass<T, N> res3 = scalar - vec1;

            for (size_t i = 0; i < N; i++)
            {
                TS_ASSERT_DELTA(res1[i], vec1[i] - scalar, tol);
                TS_ASSERT_DELTA(res3[i], scalar - vec1[i], tol);
            }
        }

        repeats = REPEATS;
    }

    template <class T, size_t N>
    void productTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1 = getRandomVector<T, N>();
            auto vec2 = getRandomVector<T, N>();

            // compute the to be compared results
            VectorClass<T, N> res1 = vec1 * vec2;
            auto res2 = product(vec1, vec2);

            for (size_t i = 0; i < N; i++)
                TS_ASSERT_DELTA(res1[i], res2[i], tol);
        }

        repeats = REPEATS;
    }

    template <class T, size_t N>
    void productScalarTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1   = getRandomVector<T, N>();
            auto scalar = getRandomNumber<T>();

            // compute the to be compared results
            VectorClass<T, N> res1 = vec1 * scalar;
            VectorClass<T, N> res2 = scalar * vec1;
            auto res3 = product(vec1, scalar);

            for (size_t i = 0; i < N; i++)
            {
                TS_ASSERT_DELTA(res1[i], res3[i], tol);
                TS_ASSERT_DELTA(res2[i], res3[i], tol);
            }
        }

        repeats = REPEATS;
    }

    template <class T, size_t N>
    void divisionTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1 = getRandomVector<T, N>();
            auto vec2 = getRandomVector<T, N>();

            // compute the to be compared results
            VectorClass<T, N> res1 = vec1 / vec2;
            auto res2 = divide(vec1, vec2);

            for (size_t i = 0; i < N; i++)
                TS_ASSERT_DELTA(res1[i], res2[i], tol);
        }

        repeats = REPEATS;
    }

    template <class T, size_t N>
    void divisionScalarTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1   = getRandomVector<T, N>();
            auto scalar = getRandomNumber<T>();

            // compute the to be compared results
            VectorClass<T, N> res1 = vec1 / scalar;
            auto res2 = divide(vec1, scalar);

            // and the other way around
            VectorClass<T, N> res3 = scalar / vec1;
            auto res4 = divide(scalar, vec1);

            for (size_t i = 0; i < N; i++)
            {
                TS_ASSERT_DELTA(res1[i], res2[i], tol);
                TS_ASSERT_DELTA(res3[i], res4[i], tol);
            }
        }

        repeats = REPEATS;
    }

    template <class T>
    void VectorAdditionTypeTest()
    {
        additionTestGeneric<T, 2>();
        additionTestGeneric<T, 3>();
        additionTestGeneric<T, 4>();
        additionTestGeneric<T, 5>();
        additionTestGeneric<T, 6>();
        additionTestGeneric<T, 10>();
    }

    template <class T>
    void VectorAdditionScalarTypeTest()
    {
        additionScalarTestGeneric<T, 2>();
        additionScalarTestGeneric<T, 3>();
        additionScalarTestGeneric<T, 4>();
        additionScalarTestGeneric<T, 5>();
        additionScalarTestGeneric<T, 6>();
        additionScalarTestGeneric<T, 10>();
    }

    template <class T>
    void VectorSubstractionTypeTest()
    {
        substractionTestGeneric<T, 2>();
        substractionTestGeneric<T, 3>();
        substractionTestGeneric<T, 4>();
        substractionTestGeneric<T, 5>();
        substractionTestGeneric<T, 6>();
        substractionTestGeneric<T, 10>();
    }

    template <class T>
    void VectorSubstractionScalarTypeTest()
    {
        substractionScalarTestGeneric<T, 2>();
        substractionScalarTestGeneric<T, 3>();
        substractionScalarTestGeneric<T, 4>();
        substractionScalarTestGeneric<T, 5>();
        substractionScalarTestGeneric<T, 6>();
        substractionScalarTestGeneric<T, 10>();
    }

    template <class T>
    void VectorProductTypeTest()
    {
        productTestGeneric<T, 2>();
        productTestGeneric<T, 3>();
        productTestGeneric<T, 4>();
        productTestGeneric<T, 5>();
        productTestGeneric<T, 6>();
        productTestGeneric<T, 10>();
    }

    template <class T>
    void VectorProductScalarTypeTest()
    {
        productScalarTestGeneric<T, 2>();
        productScalarTestGeneric<T, 3>();
        productScalarTestGeneric<T, 4>();
        productScalarTestGeneric<T, 5>();
        productScalarTestGeneric<T, 6>();
        productScalarTestGeneric<T, 10>();
    }

    template <class T>
    void VectorDivisionTypeTest()
    {
        divisionTestGeneric<T, 2>();
        divisionTestGeneric<T, 3>();
        divisionTestGeneric<T, 4>();
        divisionTestGeneric<T, 5>();
        divisionTestGeneric<T, 6>();
        divisionTestGeneric<T, 10>();
    }

    template <class T>
    void VectorDivisionScalarTypeTest()
    {
        divisionScalarTestGeneric<T, 2>();
        divisionScalarTestGeneric<T, 3>();
        divisionScalarTestGeneric<T, 4>();
        divisionScalarTestGeneric<T, 5>();
        divisionScalarTestGeneric<T, 6>();
        divisionScalarTestGeneric<T, 10>();
    }

public:

    void setUp()
    {
        repeats = REPEATS;
    }

    void tearDown()
    {}

    void testConstruction()
    {
        VectorClass<double, 2> vec = getVector2d();
    }

    void testAdditionGeneric()
    {
        VectorAdditionTypeTest<double>();
        VectorAdditionTypeTest<float>();
    }

    void testSubstractGeneric()
    {
        VectorSubstractionTypeTest<double>();
        VectorSubstractionTypeTest<float>();
    }

    void testProductGeneric()
    {
        VectorProductTypeTest<double>();
        VectorProductTypeTest<float>();
    }

    void testProductScalarGeneric()
    {
        VectorProductScalarTypeTest<double>();
        VectorProductScalarTypeTest<float>();
    }

    void testDivisionGeneric()
    {
        VectorDivisionTypeTest<double>();
        VectorDivisionTypeTest<float>();
    }

    void testDivisionScalarGeneric()
    {
        VectorDivisionScalarTypeTest<double>();
        VectorDivisionScalarTypeTest<float>();
    }

    void testDefaultConstructor()
    {
        TS_TRACE("Starting default constructor test");

        Vector2d vec2;
        Vector3d vec3;
        Vector4d vec4;

        TS_ASSERT_EQUALS(vec2.x, 0)
        TS_ASSERT_EQUALS(vec2.y, 0)

        TS_ASSERT_EQUALS(vec3.x, 0)
        TS_ASSERT_EQUALS(vec3.y, 0)
        TS_ASSERT_EQUALS(vec3.z, 0)

        TS_ASSERT_EQUALS(vec4.x, 0)
        TS_ASSERT_EQUALS(vec4.y, 0)
        TS_ASSERT_EQUALS(vec4.z, 0)
        TS_ASSERT_EQUALS(vec4.w, 0)

        TS_TRACE("Finished default constructor test");
    }

    void testConstruct2()
    {
        TS_TRACE("Starting single value constructor test");

        Vector2d vec2 (1);
        Vector3d vec3 (1);
        Vector4d vec4 (1);

        TS_ASSERT_EQUALS(vec2.x, 1)
        TS_ASSERT_EQUALS(vec2.y, 1)

        TS_ASSERT_EQUALS(vec3.x, 1)
        TS_ASSERT_EQUALS(vec3.y, 1)
        TS_ASSERT_EQUALS(vec3.z, 1)

        TS_ASSERT_EQUALS(vec4.x, 1)
        TS_ASSERT_EQUALS(vec4.y, 1)
        TS_ASSERT_EQUALS(vec4.z, 1)
        TS_ASSERT_EQUALS(vec4.w, 1)

        TS_TRACE("Finished single value constructor test");
    }

    void testConstructor3()
    {
        double data2[] {1, 2};
        double data3[] {1, 2, 3};
        double data4[] {1, 2, 3, 4};

        Vector2d vec2 (data2);
        Vector3d vec3 (data3);
        Vector4d vec4 (data4);

        TS_ASSERT_EQUALS(vec2.x, 1)
        TS_ASSERT_EQUALS(vec2.y, 2)

        TS_ASSERT_EQUALS(vec3.x, 1)
        TS_ASSERT_EQUALS(vec3.y, 2)
        TS_ASSERT_EQUALS(vec3.z, 3)

        TS_ASSERT_EQUALS(vec4.x, 1)
        TS_ASSERT_EQUALS(vec4.y, 2)
        TS_ASSERT_EQUALS(vec4.z, 3)
        TS_ASSERT_EQUALS(vec4.w, 4)
    }

    void testConstructor4()
    {
        Vector2d vec2 {1, 2};
        Vector3d vec3 {1, 2, 3};
        Vector4d vec4 {1, 2, 3, 4};

        TS_ASSERT_EQUALS(vec2.x, 1)
        TS_ASSERT_EQUALS(vec2.y, 2)

        TS_ASSERT_EQUALS(vec3.x, 1)
        TS_ASSERT_EQUALS(vec3.y, 2)
        TS_ASSERT_EQUALS(vec3.z, 3)

        TS_ASSERT_EQUALS(vec4.x, 1)
        TS_ASSERT_EQUALS(vec4.y, 2)
        TS_ASSERT_EQUALS(vec4.z, 3)
        TS_ASSERT_EQUALS(vec4.w, 4)
    }

    void testConstructor5()
    {
        Vector2d vec2 {1};

        Vector3d vec3 {1, 2};
        Vector3d vec32 {1};

        Vector4d vec42 {1, 2, 3};
        Vector4d vec43 {1, 2};
        Vector4d vec44 {1};

        TS_ASSERT_EQUALS(vec2.x, 1)
        TS_ASSERT_EQUALS(vec2.y, 0)

        TS_ASSERT_EQUALS(vec3.x, 1)
        TS_ASSERT_EQUALS(vec3.y, 2)
        TS_ASSERT_EQUALS(vec3.z, 0)

        TS_ASSERT_EQUALS(vec32.x, 1)
        TS_ASSERT_EQUALS(vec32.y, 0)
        TS_ASSERT_EQUALS(vec32.z, 0)

        TS_ASSERT_EQUALS(vec42.x, 1)
        TS_ASSERT_EQUALS(vec42.y, 2)
        TS_ASSERT_EQUALS(vec42.z, 3)
        TS_ASSERT_EQUALS(vec42.w, 0)

        TS_ASSERT_EQUALS(vec43.x, 1)
        TS_ASSERT_EQUALS(vec43.y, 2)
        TS_ASSERT_EQUALS(vec43.z, 0)
        TS_ASSERT_EQUALS(vec43.w, 0)

        TS_ASSERT_EQUALS(vec44.x, 1)
        TS_ASSERT_EQUALS(vec44.y, 0)
        TS_ASSERT_EQUALS(vec44.z, 0)
        TS_ASSERT_EQUALS(vec44.w, 0)
    }

    void testReset()
    {
        Vector2d vec2 (1);
        Vector3d vec3 (1);
        Vector4d vec4 (1);

        vec2.Reset();
        vec3.Reset();
        vec4.Reset();

        TS_ASSERT_EQUALS(vec2.x, 0)
        TS_ASSERT_EQUALS(vec2.y, 0)

        TS_ASSERT_EQUALS(vec3.x, 0)
        TS_ASSERT_EQUALS(vec3.y, 0)
        TS_ASSERT_EQUALS(vec3.z, 0)

        TS_ASSERT_EQUALS(vec4.x, 0)
        TS_ASSERT_EQUALS(vec4.y, 0)
        TS_ASSERT_EQUALS(vec4.z, 0)
        TS_ASSERT_EQUALS(vec4.w, 0)
    }

    void testSet1()
    {
        const double set = 2;

        Vector2d vec2 (1);
        Vector3d vec3 (1);
        Vector4d vec4 (1);

        vec2.set(set);
        vec3.set(set);
        vec4.set(set);

        TS_ASSERT_EQUALS(vec2.x, set)
        TS_ASSERT_EQUALS(vec2.y, set)

        TS_ASSERT_EQUALS(vec3.x, set)
        TS_ASSERT_EQUALS(vec3.y, set)
        TS_ASSERT_EQUALS(vec3.z, set)

        TS_ASSERT_EQUALS(vec4.x, set)
        TS_ASSERT_EQUALS(vec4.y, set)
        TS_ASSERT_EQUALS(vec4.z, set)
        TS_ASSERT_EQUALS(vec4.w, set)
    }

    void testCopyMove1()
    {
        Vector2d m2 {getVector2d()};
        Vector3d m3 {getVector3d()};
        Vector4d m4 {getVector4d()};

        TS_ASSERT_EQUALS(m2.x, 1)
        TS_ASSERT_EQUALS(m2.y, 2)

        TS_ASSERT_EQUALS(m3.x, 1)
        TS_ASSERT_EQUALS(m3.y, 2)
        TS_ASSERT_EQUALS(m3.z, 3)

        TS_ASSERT_EQUALS(m4.x, 1)
        TS_ASSERT_EQUALS(m4.y, 2)
        TS_ASSERT_EQUALS(m4.z, 3)
        TS_ASSERT_EQUALS(m4.w, 4)
    }

    void testAssignmentMove1()
    {
        Vector2d m2 = getVector2d();
        Vector3d m3 = getVector3d();
        Vector4d m4 = getVector4d();

        TS_ASSERT_EQUALS(m2.x, 1)
        TS_ASSERT_EQUALS(m2.y, 2)

        TS_ASSERT_EQUALS(m3.x, 1)
        TS_ASSERT_EQUALS(m3.y, 2)
        TS_ASSERT_EQUALS(m3.z, 3)

        TS_ASSERT_EQUALS(m4.x, 1)
        TS_ASSERT_EQUALS(m4.y, 2)
        TS_ASSERT_EQUALS(m4.z, 3)
        TS_ASSERT_EQUALS(m4.w, 4)
    }

    void testCopyRef1()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        Vector2d m2 {c2};
        Vector3d m3 {c3};
        Vector4d m4 {c4};

        TS_ASSERT_EQUALS(m2.x, 1)
        TS_ASSERT_EQUALS(m2.y, 2)

        TS_ASSERT_EQUALS(m3.x, 1)
        TS_ASSERT_EQUALS(m3.y, 2)
        TS_ASSERT_EQUALS(m3.z, 3)

        TS_ASSERT_EQUALS(m4.x, 1)
        TS_ASSERT_EQUALS(m4.y, 2)
        TS_ASSERT_EQUALS(m4.z, 3)
        TS_ASSERT_EQUALS(m4.w, 4)
    }

    void testAssignmentCopy1()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        Vector2d m2 = c2;
        Vector3d m3 = c3;
        Vector4d m4 = c4;

        TS_ASSERT_EQUALS(m2.x, 1)
        TS_ASSERT_EQUALS(m2.y, 2)

        TS_ASSERT_EQUALS(m3.x, 1)
        TS_ASSERT_EQUALS(m3.y, 2)
        TS_ASSERT_EQUALS(m3.z, 3)

        TS_ASSERT_EQUALS(m4.x, 1)
        TS_ASSERT_EQUALS(m4.y, 2)
        TS_ASSERT_EQUALS(m4.z, 3)
        TS_ASSERT_EQUALS(m4.w, 4)
    }

    void testNorm1()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        TS_ASSERT_DELTA(Norm2(c2), norm2d(), tol);
        TS_ASSERT_DELTA(Norm2(c3), norm3d(), tol);
        TS_ASSERT_DELTA(Norm2(c4), norm4d(), tol);

        TS_ASSERT_DELTA(Norm(c2), norm2d(), tol);
        TS_ASSERT_DELTA(Norm(c3), norm3d(), tol);
        TS_ASSERT_DELTA(Norm(c4), norm4d(), tol);
    }

    void testNorm2()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        auto n2 = Normalize(c2);
        auto n3 = Normalize(c3);
        auto n4 = Normalize(c4);

        TS_ASSERT_DELTA(Norm(n2), 1., tol);
        TS_ASSERT_DELTA(Norm(n3), 1., tol);
        TS_ASSERT_DELTA(Norm(n4), 1., tol);
    }

    void testNorm3()
    {
        Vector2d c2 {0, 0};
        Vector3d c3 {0, 0, 0};
        Vector4d c4 {0, 0, 0, 0};

        TS_ASSERT_THROWS(Normalize(c2), std::runtime_error);
        TS_ASSERT_THROWS(Normalize(c3), std::runtime_error);
        TS_ASSERT_THROWS(Normalize(c4), std::runtime_error);

        TS_ASSERT_DELTA(Norm(c2), 0., tol);
        TS_ASSERT_DELTA(Norm(c3), 0., tol);
        TS_ASSERT_DELTA(Norm(c4), 0., tol);
    }

    void testAddition1()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        c2 += c2;
        c3 += c3;
        c4 += c4;

        TS_ASSERT_EQUALS(c2.x, 2)
        TS_ASSERT_EQUALS(c2.y, 4)

        TS_ASSERT_EQUALS(c3.x, 2)
        TS_ASSERT_EQUALS(c3.y, 4)
        TS_ASSERT_EQUALS(c3.z, 6)

        TS_ASSERT_EQUALS(c4.x, 2)
        TS_ASSERT_EQUALS(c4.y, 4)
        TS_ASSERT_EQUALS(c4.z, 6)
        TS_ASSERT_EQUALS(c4.w, 8)
    }

    void testSubstraction1()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        c2 -= c2;
        c3 -= c3;
        c4 -= c4;

        TS_ASSERT_EQUALS(c2.x, 0)
        TS_ASSERT_EQUALS(c2.y, 0)

        TS_ASSERT_EQUALS(c3.x, 0)
        TS_ASSERT_EQUALS(c3.y, 0)
        TS_ASSERT_EQUALS(c3.z, 0)

        TS_ASSERT_EQUALS(c4.x, 0)
        TS_ASSERT_EQUALS(c4.y, 0)
        TS_ASSERT_EQUALS(c4.z, 0)
        TS_ASSERT_EQUALS(c4.w, 0)
    }

    void testSubstraction2()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        Vector2d m2 {1, 1};
        Vector3d m3 {1, 1, 1};
        Vector4d m4 {1, 1, 1, 1};

        c2 -= m2;
        c3 -= m3;
        c4 -= m4;

        TS_ASSERT_EQUALS(c2.x, 0)
        TS_ASSERT_EQUALS(c2.y, 1)

        TS_ASSERT_EQUALS(c3.x, 0)
        TS_ASSERT_EQUALS(c3.y, 1)
        TS_ASSERT_EQUALS(c3.z, 2)

        TS_ASSERT_EQUALS(c4.x, 0)
        TS_ASSERT_EQUALS(c4.y, 1)
        TS_ASSERT_EQUALS(c4.z, 2)
        TS_ASSERT_EQUALS(c4.w, 3)
    }

    void testSubstraction3()
    {
        Vector2d c2 {0,0};
        Vector2d m2 {0,0};

        Vector3d c3 {0,0,0};
        Vector3d m3 {0,0,0};

        Vector4d c4 {0,0,0,0};
        Vector4d m4 {0,0,0,0};

        Vector2d r2 = c2 - m2;
        Vector3d r3 = c3 - m3;
        Vector4d r4 = c4 - m4;

        TS_ASSERT_EQUALS(r2.x, 0);
        TS_ASSERT_EQUALS(r2.y, 0);

        TS_ASSERT_EQUALS(r3.x, 0);
        TS_ASSERT_EQUALS(r3.y, 0);
        TS_ASSERT_EQUALS(r3.z, 0);

        TS_ASSERT_EQUALS(r4.x, 0);
        TS_ASSERT_EQUALS(r4.y, 0);
        TS_ASSERT_EQUALS(r4.z, 0);
        TS_ASSERT_EQUALS(r4.w, 0);
    }

    void testMultiplication1()
    {
        const double m = 2.;

        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        c2 *= m;
        c3 *= m;
        c4 *= m;

        TS_ASSERT_EQUALS(c2.x, 2)
        TS_ASSERT_EQUALS(c2.y, 4)

        TS_ASSERT_EQUALS(c3.x, 2)
        TS_ASSERT_EQUALS(c3.y, 4)
        TS_ASSERT_EQUALS(c3.z, 6)

        TS_ASSERT_EQUALS(c4.x, 2)
        TS_ASSERT_EQUALS(c4.y, 4)
        TS_ASSERT_EQUALS(c4.z, 6)
        TS_ASSERT_EQUALS(c4.w, 8)
    }

    void testMultiplication2()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        Vector2d m2 {1, 2};
        Vector3d m3 {1, 2, 3};
        Vector4d m4 {1, 2, 3, 4};

        c2 *= m2;
        c3 *= m3;
        c4 *= m4;

        TS_ASSERT_EQUALS(c2.x, 1)
        TS_ASSERT_EQUALS(c2.y, 4)

        TS_ASSERT_EQUALS(c3.x, 1)
        TS_ASSERT_EQUALS(c3.y, 4)
        TS_ASSERT_EQUALS(c3.z, 9)

        TS_ASSERT_EQUALS(c4.x, 1)
        TS_ASSERT_EQUALS(c4.y, 4)
        TS_ASSERT_EQUALS(c4.z, 9)
        TS_ASSERT_EQUALS(c4.w, 16)
    }

    void testDivision1()
    {
        const double m = 2.;

        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        c2 /= m;
        c3 /= m;
        c4 /= m;

        TS_ASSERT_EQUALS(c2.x, 0.5)
        TS_ASSERT_EQUALS(c2.y, 1.)

        TS_ASSERT_EQUALS(c3.x, 0.5)
        TS_ASSERT_EQUALS(c3.y, 1.0)
        TS_ASSERT_EQUALS(c3.z, 1.5)

        TS_ASSERT_EQUALS(c4.x, 0.5)
        TS_ASSERT_EQUALS(c4.y, 1.0)
        TS_ASSERT_EQUALS(c4.z, 1.5)
        TS_ASSERT_EQUALS(c4.w, 2.0)
    }

    void testDivision2()
    {
        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        Vector2d m2 {2., 2.};
        Vector3d m3 {2., 2., 2.};
        Vector4d m4 {2., 2., 2., 2.};

        c2 /= m2;
        c3 /= m3;
        c4 /= m4;

        TS_ASSERT_EQUALS(c2.x, 0.5)
        TS_ASSERT_EQUALS(c2.y, 1.0)

        TS_ASSERT_EQUALS(c3.x, 0.5)
        TS_ASSERT_EQUALS(c3.y, 1.0)
        TS_ASSERT_EQUALS(c3.z, 1.5)

        TS_ASSERT_EQUALS(c4.x, 0.5)
        TS_ASSERT_EQUALS(c4.y, 1.0)
        TS_ASSERT_EQUALS(c4.z, 1.5)
        TS_ASSERT_EQUALS(c4.w, 2.0)
    }

    void testUnitVector()
    {
        auto phis   = linspaced(0., 2. * M_PI, 360);
        auto thetas = linspaced(0., M_PI, 180);

        for (auto phi : phis)
        {
            for (auto theta : thetas)
            {
                auto vec = UnitVector(theta, phi);

                // Now compute spherical coordinates and see if they match
                double r = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
                double t = std::acos(vec.z);
                double p = std::atan2(vec.y, vec.x);
                if (p < 0.)
                    p = 2. * M_PI + p;

                TS_ASSERT_DELTA(r, 1., tol);
                TS_ASSERT_DELTA(t, theta, tol);
                if (t > 0.)
                    TS_ASSERT_DELTA(p, phi,   tol);
            }
        }
    }

    void testUnitVector2()
    {
        auto phis   = linspaced(0., 2. * M_PI, 360);
        auto thetas = linspaced(0., M_PI, 180);

        for (auto phi : phis)
        {
            for (auto theta : thetas)
            {
                auto vec = UnitVector(theta, phi + 2. * M_PI);

                // Now compute spherical coordinates and see if they match
                double r = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
                double t = std::acos(vec.z);
                double p = std::atan2(vec.y, vec.x);
                if (p < 0.)
                    p = 2. * M_PI + p;

                TS_ASSERT_DELTA(r, 1., tol);
                TS_ASSERT_DELTA(t, theta, tol);
                if (t > 0.)
                    TS_ASSERT_DELTA(p, phi,   tol);
            }
        }
    }

    void testUnitVector3()
    {
        auto phis   = linspaced(0., 2. * M_PI, 360);
        auto thetas = linspaced(0., M_PI, 180);

        for (auto phi : phis)
        {
            for (auto theta : thetas)
            {
                auto vec = UnitVector(theta + M_PI, phi);

                // Now compute spherical coordinates and see if they match
                double r = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
                double t = std::acos(vec.z);
                double p = std::atan2(vec.y, vec.x);
                if (p < 0.)
                    p = 2. * M_PI + p;

                TS_ASSERT_DELTA(r, 1., tol);
                TS_ASSERT_DELTA(t, theta, tol);
                if (t > 0.)
                    TS_ASSERT_DELTA(p, phi,   tol);
            }
        }
    }

    void testUnitVector4()
    {
        auto phis   = linspaced(0., 2. * M_PI, 360);
        auto thetas = linspaced(0., M_PI, 180);

        for (auto phi : phis)
        {
            for (auto theta : thetas)
            {
                auto vec = UnitVector(theta, phi - 2. * M_PI);

                // Now compute spherical coordinates and see if they match
                double r = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
                double t = std::acos(vec.z);
                double p = std::atan2(vec.y, vec.x);
                if (p < 0.)
                    p = 2. * M_PI + p;

                TS_ASSERT_DELTA(r, 1., tol);
                TS_ASSERT_DELTA(t, theta, tol);
                if (t > 0.)
                    TS_ASSERT_DELTA(p, phi,   tol);
            }
        }
    }

    void testUnitVector5()
    {
        auto phis   = linspaced(0., 2. * M_PI, 360);
        auto thetas = linspaced(0., M_PI, 180);

        for (auto phi : phis)
        {
            for (auto theta : thetas)
            {
                auto vec = UnitVector(theta - M_PI, phi);

                // Now compute spherical coordinates and see if they match
                double r = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
                double t = std::acos(vec.z);
                double p = std::atan2(vec.y, vec.x);
                if (p < 0.)
                    p = 2. * M_PI + p;

                TS_ASSERT_DELTA(r, 1., tol);
                TS_ASSERT_DELTA(t, theta, tol);
                if (t > 0.)
                    TS_ASSERT_DELTA(p, phi,   tol);
            }
        }
    }

    void testAngle()
    {
        auto ax = getAngle_xy(ex);
        TS_ASSERT_DELTA(ax, 0., tol);

        auto ay = getAngle_xy(ey);
        TS_ASSERT_DELTA(ay, M_PI * 0.5, tol);

        auto az = getAngle_xy(ez);
        TS_ASSERT_DELTA(az, 0., tol);

        vector<Vector3d> vecs {{1.,1.,0.}, {-1.,1.,0}, {-1.,-1.,0.}, {1.,-1.,0}};
        vector<double> angles {0.25 * M_PI, 0.75 * M_PI, 1.25 * M_PI, 1.75 * M_PI};

        for (size_t i = 0; i < vecs.size(); i++)
        {
            auto vec = vecs[i];
            Normalize(vec);

            auto a1 = getAngle_xy(vec);
            TS_ASSERT_DELTA(a1, angles[i], tol);
        }
    }

    void testWrapAngle()
    {
        auto angles = linspaced(0., 2. * M_PI - 0.01, 100);
        vector<double> multipliers {-5, -4, -3, -2, -1, 1, 2, 3, 4, 5};
        for (auto angle : angles)
            for (auto multiplier : multipliers)
            {
                double angleToWrap = angle + 2. * multiplier * M_PI;
                TS_ASSERT_DELTA(angle, wrap_angle(angleToWrap, 2. * M_PI), tol);
            }
    }

    void testRotationDirection()
    {


    }

};
