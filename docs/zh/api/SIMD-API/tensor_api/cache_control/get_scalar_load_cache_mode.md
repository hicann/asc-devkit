# get_scalar_load_cache_mode

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

读取当前标量读（load）路径的L2 Cache管理策略，并返回`load_l2_cache_mode`枚举值。该值可通过[set_scalar_cache_mode](set_scalar_cache_mode.md)接口设置，关于策略的详细说明请参考该接口。

常用于临时切换策略前保存原配置，或在设置后做回读校验。

## 函数原型

```cpp
__aicore__ inline load_l2_cache_mode get_scalar_load_cache_mode()
```

## 参数说明

无

## 返回值说明

当前标量load路径的L2 Cache管理策略，类型为`load_l2_cache_mode`。

## 流水类型

PIPE_S

## 约束说明

本接口仅反映标量load侧策略，与标量store侧以及向量搬运接口参数中的缓存策略无关。

## 调用示例

以下示例保存标量load路径当前策略，切换策略后恢复原策略。

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void load_cache_mode_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    const load_l2_cache_mode old_mode = get_scalar_load_cache_mode();
    set_scalar_cache_mode(load_l2_cache_mode::normal_first_victim);
    const int32_t value = load_dev<int32_t>(data);
    set_scalar_cache_mode(old_mode);
    store_dev<int32_t>(data, value);
}
```

