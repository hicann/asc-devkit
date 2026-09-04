# shfl_up

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block_tile/shfl_up_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/cooperative_groups.h"`。

获取`thread_block_tile`组内当前线程向前偏移`delta`的线程的数据。

## 函数原型

```cpp
template <typename T>
T shfl_up(T var, unsigned int delta) const
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| var | 输入 | 线程用于交换的输入操作数。支持的数据类型为：half、int32_t、uint32_t、float、half2、int64_t、uint64_t、bfloat16_t、bfloat16x2_t。 |
| delta | 输入 | 期望获取的`var`值所在线程在组内相对当前线程的向前偏移值。 |

## 返回值说明

协作组内当前线程向前偏移`delta`的线程输入的`var`值。若偏移后超出组范围，返回当前线程输入的`var`值。

## 约束说明

仅支持`thread_block_tile`的模板参数`Size`小于等于32且为2的幂（1、2、4、8、16、32）的场景，即仅支持单Warp内线程子组。

## 调用示例

以4个线程为一组划分线程块，获取协作组内当前线程向前偏移`delta`的线程输入的`var`值。

**图1**  shfl_up接口返回结果示意图  
![](../../../figures/thread_block_tile_shfl_up.png "thread_block_tile_shfl_up")

- SIMT编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        uint32_t result = tile4.shfl_up(threadIdx.x + 100, 2);
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
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        uint32_t result = tile4.shfl_up(threadIdx.x + 100, 2);
        ...
    }
    ```
