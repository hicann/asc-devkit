# shfl

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/coalesced_group/shfl_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/cooperative_groups.h"`。

`coalesced_group`组内线程的数据交换接口，不通过共享内存实现直接读取组内指定线程的寄存器中的值。

## 函数原型

```cpp
template <typename T>
T shfl(T var, int src_rank) const
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| var | 输入 | 线程用于交换的数据。支持的数据类型为：half、int32_t、uint32_t、float、half2、int64_t、uint64_t、bfloat16_t、bfloat16x2_t。 |
| src_rank | 输入 | 期望获取的`var`值所在的线程在组内的排名。当`src_rank`大于等于组内线程数时，获取`src_rank % num_threads()`对应rank线程的`var`值。 |

## 返回值说明

`coalesced_group`组内指定线程输入的`var`值。

## 约束说明

无

## 调用示例

示例代码中的条件分支将一个warp中所有线程id是偶数的线程组成`coalesced_group`协作组，组内各线程`shfl()`接口返回结果如下图所示。

**图1**  shfl结果示意图  
![](../../../figures/coalesced_group_shfl.png)

- SIMT编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            uint32_t result = active.shfl(threadIdx.x + 100, 3);
        }
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            uint32_t result = active.shfl(threadIdx.x + 100, 3);
        }
        ...
    }
    ```
