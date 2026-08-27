# asc\_ballot

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_ballot_res.md#id1 -->

## 功能说明

判断Warp中每个活跃线程的输入是否非零。

当Warp内所有活跃线程执行本接口后，对所有活跃线程的输入操作数predicate进行判断，返回一个32bit的无符号整数，若Warp内活跃线程输入的predicate不为0，则返回值中与线程Lane ID对应的bit位为1，否则为0。Warp内所有活跃线程返回相同的结果。

## 函数原型

```cpp
inline uint32_t asc_ballot(int32_t predicate)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| predicate | 输入 | 操作数。 |

## 返回值说明

32bit的无符号整数：若Warp内活跃线程输入的predicate不为0，则返回值中与线程Lane ID对应的bit位为1，否则为0。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/device_warp_functions.h`头文件。

```cpp
#include "simt_api/device_warp_functions.h"
```

## 调用示例

以下样例通过掩码记录当前Warp内输入数据超过阈值的线程，并由Lane 0将掩码写入GM。

完整样例请参考[Sobel边缘检测样例](../../../../../../examples/03_simt_api/02_features/01_api_features/03_warp_instruction/sobel_warp_shfl/README.md)。

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void KernelBallot(
        const float* input, uint32_t* selected_mask, uint64_t total_length, float threshold)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        uint32_t mask = asc_ballot(input[idx] > threshold);
        if (lane_id == 0) {
            selected_mask[warp_id] = mask; // 每个bit表示对应Lane的数据是否超过阈值。
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void KernelBallot(
        __gm__ const float* input, __gm__ uint32_t* selected_mask, uint64_t total_length, float threshold)
    {
        // asc_vf_call参数：dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        uint32_t mask = asc_ballot(input[idx] > threshold);
        if (lane_id == 0) {
            selected_mask[warp_id] = mask; // 每个bit表示对应Lane的数据是否超过阈值。
        }
    }
    ```
