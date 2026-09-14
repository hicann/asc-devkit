# Memory Space Qualifiers

<!-- md-trans-meta sourceCommit=5cd1ca67d9a3fb2031f31c875e7c2cd5c29578f1 translatedAt=2026-09-09T02:27:52.217Z pushedAt=2026-09-09T10:00:56.664Z -->

In SIMT programming, the memory space qualifier \_\_ubuf\_\_ is used to represent dynamic and static memory. The size of static memory is determined at compile time, while the size of dynamic memory is determined at kernel launch.

-   Static memory is allocated through arrays:

    ```
    __ubuf__ half staticBuf[1024];
    ```

-   Dynamic memory is requested and used in the following ways:

    ```
    extern __ubuf__ half dynamicBuf[];
    ```

    The actual memory size of dynamic memory must be configured at kernel launch.

