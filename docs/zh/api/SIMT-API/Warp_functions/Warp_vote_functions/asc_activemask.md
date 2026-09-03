# asc\_activemask

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_activemask_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_warp_functions.h"`。

查看Warp内所有线程是否为活跃状态。

返回一个32bit的无符号整数，若Warp内某个线程是活跃（已结束线程是非活跃状态）的，则返回值中与线程Lane ID对应的bit位为1，否则为0。Warp内所有活跃线程返回相同的结果。

## 函数原型

```cpp
inline uint32_t asc_activemask()
```

## 参数说明

无

## 返回值说明

32bit的无符号整数：若Warp内某个线程是活跃的，则返回值中与线程Lane ID对应的bit位为1，否则为0。

## 约束说明

无

## 调用示例

以下样例获取当前Warp中各个线程的活跃状态，并由Lane 0将结果写入GM。

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelActiveMask(uint32_t* active_masks, uint64_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        uint32_t active_mask = asc_activemask(); // 获取当前Warp中仍然活跃的线程掩码。
        if (lane_id == 0) {
            active_masks[warp_id] = active_mask; // 由Lane 0负责写出当前Warp的各个线程的是否活跃的状态。
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelActiveMask(
        __gm__ uint32_t* active_masks, uint64_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        // asc_vf_call参数：dim3{1024, 1, 1}
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        uint32_t active_mask = asc_activemask(); // 获取当前Warp中仍然活跃的线程掩码。
        if (lane_id == 0) {
            active_masks[warp_id] = active_mask; // 由Lane 0负责写出当前Warp的各个线程的是否活跃的状态。
        }
    }
    ```
