# asc\_any

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_any_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_warp_functions.h"`。

判断是否有活跃线程的输入不为0。

当Warp内所有活跃线程执行本接口后，对所有活跃线程的输入操作数`predicate`进行判断，所有活跃线程的`predicate`均为0，返回0，否则返回1。Warp内所有活跃线程返回相同的结果。

## 函数原型

```cpp
inline int32_t asc_any(int32_t predicate)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| predicate | 输入 | 操作数。 |

## 返回值说明

当Warp内所有活跃线程的输入均为0，返回0，否则返回1。

## 约束说明

无

## 调用示例

以下样例判断当前Warp内是否存在与目标值匹配的输入，并由Lane 0将判断结果写入GM。

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelAny(
        const int32_t* input, int32_t* match_flags, uint64_t total_length, int32_t target)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        int32_t has_match = asc_any(input[idx] == target);
        if (lane_id == 0) {
            match_flags[warp_id] = has_match;
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelAny(
        __gm__ const int32_t* input, __gm__ int32_t* match_flags, uint64_t total_length, int32_t target)
    {
        // asc_vf_call参数：dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        int32_t has_match = asc_any(input[idx] == target);
        if (lane_id == 0) {
            match_flags[warp_id] = has_match;
        }
    }
    ```
