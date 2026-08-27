# asc\_reduce\_min

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_min_res.md#id1 -->

## 功能说明

对Warp内所有活跃线程输入val求最小值。Warp内所有活跃线程返回相同的结果。

## 函数原型

```cpp
inline int32_t asc_reduce_min(int32_t val)
```

```cpp
inline uint32_t asc_reduce_min(uint32_t val)
```

```cpp
inline float asc_reduce_min(float val)
```

```cpp
inline half asc_reduce_min(half val)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| val | 输入 | 源操作数。 |

## 返回值说明

Warp内所有活跃线程输入val的最小值。

## 约束说明

无

## 需要包含的头文件

使用除half类型之外的接口需要包含`simt_api/device_warp_functions.h`头文件，使用half类型接口需要包含`simt_api/asc_fp16.h`头文件。

```cpp
#include "simt_api/device_warp_functions.h"
```

```cpp
#include "simt_api/asc_fp16.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void KernelReduceMin(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t result = asc_reduce_min(laneId); // 返回值为0
        dst[idx] = result;
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void KernelReduceMin(__gm__ int32_t* dst)
    {
        // asc_vf_call参数：dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t result = asc_reduce_min(laneId); // 返回值为0
        dst[idx] = result;
    }
    ```
