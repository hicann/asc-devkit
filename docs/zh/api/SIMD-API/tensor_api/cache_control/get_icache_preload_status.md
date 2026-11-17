# get_icache_preload_status

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

该接口为调试接口，在[icache_preload](icache_preload.md)后调用，用于获取ICache的Preload状态：

- 返回`0`：ICache预取空闲，上一次预取已完成。
- 返回`1`：ICache预取正在进行中，尚未完成。

## 函数原型

```cpp
__aicore__ inline int64_t get_icache_preload_status()
```

## 参数说明

无

## 返回值说明

返回`int64_t`类型的ICache预取状态值，仅有效位取值如下：

- 返回`0`：ICache预取空闲，上一次预取已完成。
- 返回`1`：ICache预取正在进行中，尚未完成。

## 流水类型

PIPE_S

## 约束说明

本接口仅用于调试与性能分析场景下的状态查询，不影响预取本身的执行流程。

## 调用示例

以下示例获取当前程序计数器，预取指令后查询预取状态。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void preload_status_kernel(__gm__ int64_t* status)
{
    using namespace asc::te;
    constexpr int64_t pc_mask = 0xFFFFFFFFFFFF;
    const int64_t pc = get_program_counter() & pc_mask;
    icache_preload(reinterpret_cast<const void*>(pc), 1);
    status[0] = get_icache_preload_status();
}
```

