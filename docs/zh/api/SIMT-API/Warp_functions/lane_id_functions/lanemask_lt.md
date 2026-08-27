# lanemask\_lt

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/lane_id_functions/lanemask_lt_res.md#id1 -->

## 功能说明

获取当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID严格小于当前线程”的线程的对应位设为1，其余位为0。

如Lane ID为0的线程，调用本接口获取到32位掩码：0000 0000 0000 0000 0000 0000 0000 0000。

如Lane ID为31的线程，调用本接口获取到32位掩码：0111 1111 1111 1111 1111 1111 1111 1111。

**图1**  lanemask\_lt示意图

![](../../../figures/lanemask_lt.png "lanemask_lt图示")

## 函数原型

```cpp
int32_t lanemask_lt()
```

## 参数说明

无

## 返回值说明

返回一个32位整数的位掩码。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/device_functions.h`头文件。

```cpp
#include "simt_api/device_functions.h" 
```

## 调用示例

下面示例使用`lanemask_lt()`统计当前Lane之前的线程数，并据此判断当前线程位于Warp的上半区还是下半区，再通过`asc_shfl()`分别广播上下半Warp中指定Lane的数据。示例中使用[asc_shfl](../Warp_shfl_functions/asc_shfl.md)，需另外包含`simt_api/device_warp_functions.h`头文件，其中`srcLane`的取值范围为[0, 15]。

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void kernel_lanemask_lt(int32_t* src, int32_t* dst, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lanes_before = __popc(static_cast<uint32_t>(lanemask_lt()));
        int32_t group_leader_lane = (lanes_before < 16) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) void kernel_lanemask_lt(__gm__ int32_t* src, __gm__ int32_t* dst, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lanes_before = __popc(static_cast<uint32_t>(lanemask_lt()));
        int32_t group_leader_lane = (lanes_before < 16) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```
