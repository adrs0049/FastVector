# FastVector
C++ implementations of various common linear algebra.

This repository includes two different classes for the use of common linear algebra computations.
* `CSVector` for large vectors which are dynamically allocated.
* `Vector<T, N>` for small vectors of fixed size.

*Disclaimer:* This was an educational project, and I do not guarantee correct
functioning of all code paths. In fact, I know of a few code paths that have
various bugs.

# `CSVector`

All the basic math operators are parallelized using `openMP`, on top of that since storage memory is assured to be aligned, and the loops are unrolled during compile time, most compilers are much more likely to use modern SIMD instructions.

*Example:*

Suppose that `x, r, p` are vectors and `a` is a real number. Then usually to
compute the mathematical expression:
```
x = p * r + a * x;
```
we would write the code
```
for (size_t i = 0; i < x.size(); i++)
    x[i] = p[i] * r[i] + a * x[i];
```
With `CSVector` we can simply write:
```
x = p * r + a * x;
```

In addition, the resulting loop, will be in block form (compiler uses SIMD instructions)
and will be parallelized using `openMP`.

The currently hand tuned functions for the dot product and norms of vectors are
approximately 400% fast using AVX2, then naively implemented versions.



## Building

```
mkdir build
cd build
cmake ..
make
```

## Unit tests

Unit tests can easily be executed following a build using
```
make test
```

