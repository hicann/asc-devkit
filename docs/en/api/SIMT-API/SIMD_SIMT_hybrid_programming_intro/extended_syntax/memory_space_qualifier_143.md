# Memory Space Qualifiers

<!-- md-trans-meta sourceCommit=5cd1ca67d9a3fb2031f31c875e7c2cd5c29578f1 translatedAt=2026-09-07T10:03:57.246Z pushedAt=2026-09-09T01:49:40.938Z -->

## \_\_gm\_\_

In SIMD and SIMT hybrid programming, the memory space qualifier \_\_gm\_\_ is used to represent objects allocated in the device-side global memory. A global memory object can be declared as a scalar or a pointer to a user-defined structure.

```
__gm__ int *var; // var points to an array of int elements

typedef struct {
    float a[3];
    int b[2];
} foo_t;

__gm__ foo_t *info; // info points to an array of foo_t elements
```

## \_\_ubuf\_\_

In SIMD and SIMT hybrid programming, memory space qualifiers \_\_ubuf\_\_ are used to represent dynamic and static memory. The size of static memory is determined at compile time, while the size of dynamic memory is determined at kernel execution.

-   Static memory is allocated through arrays:

    ```
    __ubuf__ half staticBuf[1024];
    ```

-   Dynamic memory is requested and used in the following ways:

    ```
    extern __ubuf__ half dynamicBuf[];
    ```

    The actual memory size of dynamic memory must be configured at kernel launch.

