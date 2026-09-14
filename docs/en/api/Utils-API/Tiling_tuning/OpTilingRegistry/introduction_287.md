# Introduction<a name="ZH-CN_TOPIC_0000002333578885"></a>

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-09-06T16:10:32.955Z pushedAt=2026-09-10T01:34:13.153Z -->

The **OpTilingRegistry** class belongs to the context\_ascendc namespace. It is mainly used to load the dynamic library that implements Tiling and obtain the operator's Tiling function pointer for debugging and verification. This class is usually used together with the [ContextBuilder](../ContextBuilder/ContextBuilder.md) class, which is used to construct the input parameters required by the Tiling function. For details, see [How to Perform Tiling Debugging](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/programming_guide/appendix/common_operations/how_to_perform_tiling_debugging.md).

## Required Header and Library Files<a name="section78885814919"></a>

-   Header file to include

    ```
    #include "tiling/context/context_builder.h"
    ```

-   Library file to link: libtiling\_api.a.

## Public Member Functions<a name="section1173524710"></a>

```
[OpTilingRegistry](constructor_and_destructor_288.md)()
[~OpTilingRegistry](constructor_and_destructor_288.md)()
TilingFunc [GetTilingFunc](GetTilingFunc.md)(const char *opType) const
bool [LoadTilingLibrary](LoadTilingLibrary.md)(const char *tilingSoPath) const
```
