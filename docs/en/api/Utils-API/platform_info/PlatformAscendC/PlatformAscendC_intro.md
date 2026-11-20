# PlatformAscendC Introduction<a name="ZH-CN_TOPIC_0000002224684138"></a>

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-10T10:37:55.617Z pushedAt=2026-11-13T08:46:49.294Z nl=2 -->

When implementing the Tiling function on the host side, you may need to obtain some hardware platform information, such as the number of cores, to support Tiling computation. The PlatformAscendC class provides the functionality to obtain such platform information.

## Required Header Files<a name="section78885814919"></a>

To use this feature, include the "tiling/platform/platform\_ascendc.h" header file. The following is an example:

```
#include "tiling/platform/platform_ascendc.h"
```

## Common Usage

### Custom Operator Project

Custom operator projects typically use it in the Tiling function. The framework passes platform information through `gert::TilingContext`, based on which you can construct a `PlatformAscendC` object and then call the platform information APIs.

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context)
{
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto coreNum = ascendcPlatform.GetCoreNum();
    // ... Design the tiling strategy based on the number of cores.
    context->SetSimdNumBlocks(coreNum);
    return ret;
}
```

### Kernel Direct Invocation

Kernel direct invocation does not obtain platform information through `gert::TilingContext`. You can directly obtain the `PlatformAscendC` pointer through [PlatformAscendCManager](../PlatformAscendCManager.md) and then call the same platform information APIs.

```cpp
void GetInfoFun()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return;
    }
    auto coreNum = ascendcPlatform->GetCoreNum();
    // ... Design the splitting strategy based on the number of cores.
}
```
