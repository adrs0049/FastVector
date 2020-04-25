// test
#define _NO_CORE_

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <random>
#include <vector>

#include "ranges.h"

#define private public
#define protected public
#include "../Vector.h"

#define REPEATS 50

using namespace std;

void vector_equals(Vector3d& vector, double value)
{
    TS_ASSERT_EQUALS((vector).x, (value));
    TS_ASSERT_EQUALS((vector).y, (value));
    TS_ASSERT_EQUALS((vector).z, (value));
}

class VectorReductionTest : public CxxTest::TestSuite
{
private:
    double tol = 1.e-8;
    double ftol = 1e-5;
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

    auto getVector2f()
    {
        return Vector2f(1., 2.);
    }

    auto getVector3f()
    {
        return Vector3f(1., 2., 3.);
    }

    auto getVector4f()
    {
        return Vector4f(1., 2., 3., 4.);
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
    ConstantVector<T, N> getRandomVector()
    {
        ConstantVector<T, N> ret;
        std::random_device rnd;
        std::mt19937 engine(rnd());
        std::uniform_real_distribution<T> dist(-1e1, 1e1);
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
    T dotProduct(const ConstantVector<T, N>& lhs, const ConstantVector<T, N>& rhs)
    {
        return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), T(0));
    }

    template <class T, size_t N>
    T getNorm(const ConstantVector<T, N>& lhs)
    {
        return sqrt(dotProduct(lhs, lhs));
    }

    template <class T, size_t N>
    void dotTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1 = getRandomVector<T, N>();
            auto vec2 = getRandomVector<T, N>();

            auto cdot = Dot(vec1, vec2);
            auto vdot = dotProduct(vec1, vec2);

            double diff = abs(cdot-vdot);

            if (diff > 1e-2)
            {
                std::cout << "\t" << std::setprecision(12) << "vec1:" << vec1 << " vec2:" << vec2 << std::endl;
                std::cout << "\t" << std::setprecision(12) << "dot:" << cdot << " vdot:" << vdot << std::endl;
                std::cout << "\t" << std::setprecision(12) << "diff:" << abs(cdot - vdot) << std::endl;
            }

            TS_ASSERT_DELTA(cdot, vdot, 1e-2);
        }

        repeats = REPEATS;
    }

    template <class T, size_t N>
    void normTestGeneric()
    {
        while (repeats --> 0)
        {
            auto vec1 = getRandomVector<T, N>();

            auto vdot  = dotProduct(vec1, vec1);
            auto vnorm = getNorm(vec1);

            auto cnorm = Norm(vec1);
            TS_ASSERT_DELTA(cnorm, vnorm, 1e4 * tol);

            auto dnorm = Norm2(vec1);
            TS_ASSERT_DELTA(dnorm, vnorm, 1e4 * tol);

            auto enorm = Norm2Squared(vec1);
            TS_ASSERT_DELTA(enorm, vdot,  1e4 * tol);
        }

        repeats = REPEATS;
    }

    template <class T>
    void VectorDotTest()
    {
        dotTestGeneric<T, 2>();
        dotTestGeneric<T, 3>();
        dotTestGeneric<T, 4>();
        dotTestGeneric<T, 5>();
        dotTestGeneric<T, 6>();
        dotTestGeneric<T, 10>();
    }

    template <class T>
    void VectorNormTest()
    {
        normTestGeneric<T, 2>();
        normTestGeneric<T, 3>();
        normTestGeneric<T, 4>();
        normTestGeneric<T, 5>();
        normTestGeneric<T, 6>();
        normTestGeneric<T, 10>();
    }

public:

    void setUp()
    {
        repeats = REPEATS;
    }

    void tearDown()
    {}

    void testNorm()
    {
        TS_TRACE("Test Norm");
        std::cout << "TESTING NORM!" << std::endl;

        VectorNormTest<double>();
        VectorNormTest<float>();
    }

    void testNorm1()
    {
        std::cout << "TESTING NORM1!" << std::endl;

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
        std::cout << "TESTING NORM2!" << std::endl;

        Vector2d c2 {getVector2d()};
        Vector3d c3 {getVector3d()};
        Vector4d c4 {getVector4d()};

        Vector2d d2 = Normalize(c2);
        Vector3d d3 = Normalize(c3);
        Vector4d d4 = Normalize(c4);

        TS_ASSERT_DELTA(Norm(d2), 1., tol);
        TS_ASSERT_DELTA(Norm(d3), 1., tol);
        TS_ASSERT_DELTA(Norm(d4), 1., tol);

        auto n2 = Normalize(c2);
        auto n3 = Normalize(c3);
        auto n4 = Normalize(c4);

        std::cout << "n2:" << Norm(n2) << std::endl;
        std::cout << "n3:" << Norm(n3) << std::endl;
        std::cout << "n4:" << Norm(n4) << std::endl;

        std::cout << "d2:" << d2 << std::endl;
        std::cout << "d3:" << d3 << std::endl;
        std::cout << "d4:" << d4 << std::endl;

        TS_ASSERT_DELTA(Norm(n2), 1., tol);
        TS_ASSERT_DELTA(Norm(n3), 1., tol);
        TS_ASSERT_DELTA(Norm(n4), 1., tol);

        std::cout << "\n\nNORMALIZE DONE!\n\n\n" << std::endl;
    }

    void testNorm2f()
    {
        std::cout << "TESTING NORM2 for float!" << std::endl;

        Vector2f c2 {getVector2f()};
        Vector3f c3 {getVector3f()};
        Vector4f c4 {getVector4f()};

        Vector2f d2 = Normalize(c2);
        Vector3f d3 = Normalize(c3);
        Vector4f d4 = Normalize(c4);

        TS_ASSERT_DELTA(Norm(d2), 1., ftol);
        TS_ASSERT_DELTA(Norm(d3), 1., ftol);
        TS_ASSERT_DELTA(Norm(d4), 1., ftol);

        auto n2 = Normalize(c2);
        auto n3 = Normalize(c3);
        auto n4 = Normalize(c4);

        std::cout << "n2:" << Norm(n2) << std::endl;
        std::cout << "n3:" << Norm(n3) << std::endl;
        std::cout << "n4:" << Norm(n4) << std::endl;

        std::cout << "d2:" << d2 << std::endl;
        std::cout << "d3:" << d3 << std::endl;
        std::cout << "d4:" << d4 << std::endl;

        TS_ASSERT_DELTA(Norm(n2), 1., ftol);
        TS_ASSERT_DELTA(Norm(n3), 1., ftol);
        TS_ASSERT_DELTA(Norm(n4), 1., ftol);

        std::cout << "\n\nNORMALIZE DONE!\n\n\n" << std::endl;
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

    void testDotQuick()
    {
        TS_TRACE("Test Dot Quick");

        ConstantVector<double, 10> vec1 {-4.93850946426, -8.46946620941, -7.63285923004, -2.6777780056, 6.51855230331, -5.89416980743, -5.11822414398, -1.50745928288, 6.52557373047, 2.04554438591};

        ConstantVector<double, 10> vec2 {-3.02331733704, 3.36335539818, 2.91306257248, 6.8647518158, -7.24288129807, -2.56353974342, -9.11276531219, -3.44443321228, -6.99633932114, -8.41554450989};

        double cdot = Dot(vec1, vec2);
        double vdot = dotProduct(vec1, vec2);

        TS_ASSERT_DELTA(cdot, vdot, tol);
    }

    void testDot()
    {
        TS_TRACE("Test Dot");

        VectorDotTest<double>();
        VectorDotTest<float>();
    }

    void testDotQuick2()
    {
        TS_TRACE("Test Dot second");

        ConstantVector<double, 10> vec1 {-4.93850946426, -8.46946620941, -7.63285923004, -2.6777780056, 6.51855230331, -5.89416980743, -5.11822414398, -1.50745928288, 6.52557373047, 2.04554438591};

        ConstantVector<double, 10> vec2 {-3.02331733704, 3.36335539818, 2.91306257248, 6.8647518158, -7.24288129807, -2.56353974342, -9.11276531219, -3.44443321228, -6.99633932114, -8.41554450989};

        double cdot = Dot(vec1, vec2);
        double vdot = dotProduct(vec1, vec2);

        TS_ASSERT_DELTA(cdot, vdot, tol);
    }


};
