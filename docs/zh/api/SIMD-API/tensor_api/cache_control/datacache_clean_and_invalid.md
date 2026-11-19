# datacache_clean_and_invalid

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

按目标域清理并失效Data Cache中的缓存行。Clean操作将被修改过的数据写回Global Memory，Invalid操作将缓存行标记为无效，后续访问从Global Memory重新加载数据。

> [!NOTE]说明
>
> - Scalar单元读写GM时，若调用[load_dev](../scalar_compute/load_dev.md)和[store_dev](../scalar_compute/store_dev.md)这一对接口，则读写数据不经过DCache，无需调用本接口。
> - Scalar单元读写UB时是否经过DCache，默认为不经过DCache。可通过[enable_scalar_ub_dcache](enable_scalar_ub_dcache.md)开启，并通过[disable_scalar_ub_dcache](disable_scalar_ub_dcache.md)关闭。
> - DMA搬运UB与GM不经过DCache，不产生本页所述一致性问题。
> - DCache与GM缓存一致性成因见[缓存一致性](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md)。

## 函数原型

```cpp
enum class clean_target {
    all,
    ub,
    gm,
    atomic
};

__aicore__ inline void datacache_clean_and_invalid(clean_target target)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| target | 输入 | 目标域，类型为`clean_target`。取值为： <br>• `all`：刷新整个Data Cache中所有valid的缓存行。 <br>• `ub`：刷新整个Data Cache中包含UB地址的valid缓存行。 <br>• `gm`：刷新整个Data Cache中包含GM地址的valid缓存行。 <br>• `atomic`：刷新整个Data Cache中包含与标量原子操作相关的GM地址的valid缓存行。 |

## 返回值说明

无

## 约束说明

- 本接口只覆盖整个Data Cache的目标域刷新，不提供按地址刷新单个Cache Line的能力。
- 若需要等待Data Cache清理并失效触发的写回完成，应配合[sync_data_barrier](../sync/data_barrier.md)使用。

## 调用示例

以下示例先通过Scalar写GM产生脏数据，再调用本接口将GM地址对应的DCache脏数据写回Global Memory。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr int32_t dirty_value = 101;

__global__ __vector__ void cache_clean_and_invild_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    volatile __gm__ int32_t* data_gm = data;

    data_gm[0] = dirty_value;
    data_gm[32] = dirty_value;
    datacache_clean_and_invalid(clean_target::gm);
}
```
