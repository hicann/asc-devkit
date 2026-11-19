# enable_scalar_ub_dcache

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

配置标量单元访问Unified Buffer（UB）时从DCache中读取。DCache的读取速度快于UB，因此在对UB进行大量读取，且UB中数据不被Vector单元写入时，可选择开启该模式。

## 函数原型

```cpp
__aicore__ inline void enable_scalar_ub_dcache()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口只配置标量单元访问UB的DCache路径，不影响其他控制配置。
- 若UB中数据会被Vector单元写入，不应开启该模式。
- 本接口设置后持续生效。相关读取完成后，若后续标量单元访问UB需要直接从UB中读取，应调用[disable_scalar_ub_dcache](disable_scalar_ub_dcache.md)。

## 调用示例

以下示例先在UB中准备64个int32_t数据，再开启标量单元访问UB的DCache路径，完成标量读取求和，最后关闭该路径并将结果写入GM。

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void enable_scalar_ub_dcache_kernel(__gm__ int32_t* output)
{
    using namespace asc::te;
    constexpr uint32_t element_count = 64;
    __ubuf__ int32_t input_buffer[element_count];
    for (uint32_t i = 0; i < element_count; ++i) {
        input_buffer[i] = static_cast<int32_t>(i) + 1;
    }

    int32_t sum = 0;
    enable_scalar_ub_dcache();
    for (uint32_t i = 0; i < element_count; ++i) {
        sum += input_buffer[i];
    }
    disable_scalar_ub_dcache();
    output[0] = sum;
}
```
