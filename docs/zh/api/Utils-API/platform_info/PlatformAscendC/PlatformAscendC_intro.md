# PlatformAscendC简介<a name="ZH-CN_TOPIC_0000002224684138"></a>

在实现Host侧的Tiling函数时，可能需要获取一些硬件平台的信息，来支撑Tiling的计算，比如获取硬件平台的核数等信息。PlatformAscendC类提供获取这些平台信息的功能。

## 需要包含的头文件<a name="section78885814919"></a>

使用该功能需要包含"tiling/platform/platform\_ascendc.h"头文件。样例如下：

```
#include "tiling/platform/platform_ascendc.h"
```

## 常见使用方式

### 自定义算子工程

自定义算子工程通常在Tiling函数中使用。框架通过`gert::TilingContext`传入平台信息，可据此构造`PlatformAscendC`对象后调用平台信息接口。

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context)
{
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto coreNum = ascendcPlatform.GetCoreNum();
    // ... 根据核数自行设计Tiling策略
    context->SetSimdNumBlocks(coreNum);
    return ret;
}
```

### Kernel直调

Kernel直调不通过`gert::TilingContext`获取平台信息，可通过[PlatformAscendCManager](../PlatformAscendCManager.md)直接获取`PlatformAscendC`指针后调用相同的平台信息接口。

```cpp
void GetInfoFun()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return;
    }
    auto coreNum = ascendcPlatform->GetCoreNum();
    // ... 根据核数自行设计切分策略
}
```
