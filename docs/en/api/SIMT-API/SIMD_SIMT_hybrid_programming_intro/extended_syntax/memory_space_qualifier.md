# Address Space Qualifier

<!-- md-trans-meta sourceCommit=fcc18497af374dd54b2f833e8c2d0342c4c5505f translatedAt=2026-10-27T03:45:08.986Z pushedAt=2026-10-27T09:21:55.718Z nl=2 -->

## \_\_gm\_\_

In SIMD and SIMT hybrid programming, the address space qualifier `__gm__` is used to represent objects allocated in the device-side global memory. A global memory object can be declared as a pointer to a scalar or a user-defined structure.

```cpp
__gm__ int *var; // var points to an array of int elements

typedef struct {
    float a[3];
    int b[2];
} foo_t;

__gm__ foo_t *info; // info points to an array of foo_t elements
```

## \_\_ubuf\_\_

In mixed SIMD and SIMT programming, the address space qualifier `__ubuf__` is used to represent dynamic and static memory. The size of static memory is determined at compile time, while the size of dynamic memory is determined at kernel execution time.

-   Static memory is allocated through arrays:

    ```cpp
    __ubuf__ half staticBuf[1024];
    ```

-   Dynamic memory is requested and used in the following ways:

    ```cpp
    extern __ubuf__ half dynamicBuf[];
    ```

    The actual size of dynamic memory must be configured when the kernel is launched.
