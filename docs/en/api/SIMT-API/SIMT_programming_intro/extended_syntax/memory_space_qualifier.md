# Address Space Qualifier

<!-- md-trans-meta sourceCommit=fcc18497af374dd54b2f833e8c2d0342c4c5505f translatedAt=2026-10-27T03:52:50.552Z pushedAt=2026-10-27T09:21:55.741Z nl=2 -->

In SIMT programming, the address space qualifier `__ubuf__` is used to represent dynamic and static memory. The size of static memory is determined at compile time, while the size of dynamic memory is determined when the kernel is launched.

-   Static memory is allocated through arrays:

    ```cpp
    __ubuf__ half staticBuf[1024];
    ```

-   Dynamic memory is requested and used in the following ways:

    ```cpp
    extern __ubuf__ half dynamicBuf[];
    ```

    The actual size of dynamic memory must be configured when the kernel is launched.
