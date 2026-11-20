# Overview<a name="ZH-CN_TOPIC_0000002333578885"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-11-13T09:46:26.749Z pushedAt=2026-11-16T07:07:41.631Z nl=2 -->

The OpTilingRegistry class belongs to the context\_ascendc namespace. It is mainly used to load the dynamic library that implements Tiling and obtain the operator's Tiling function pointer for debugging and verification. This class is typically used together with the [ContextBuilder](../ContextBuilder/ContextBuilder.md) class, which constructs the input parameters required by the Tiling function. For details, see [Tiling Debugging](../../../../guide/programming_guide/appendix/common_operations/tiling_debug.md).

## Required Header Files and Library Files<a name="section78885814919"></a>

-   Required Header File

    ```
    #include "tiling/context/context_builder.h"
    ```

-   Library files to be linked: libtiling\_api.a.

## Public Member Functions<a name="section1173524710"></a>

```
[OpTilingRegistry](constructor_and_destructor.md)()
[~OpTilingRegistry](constructor_and_destructor.md)()
TilingFunc [GetTilingFunc](GetTilingFunc.md)(const char *opType) const
bool [LoadTilingLibrary](LoadTilingLibrary.md)(const char *tilingSoPath) const
```

