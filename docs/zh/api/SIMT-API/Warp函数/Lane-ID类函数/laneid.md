# laneid

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

## 功能说明

获取线程在其所在Warp内的索引。

## 函数原型

```cpp
int32_t laneid()
```

## 参数说明

无

## 返回值说明

线程在其Warp内的索引，当前一个Warp中的线程数量为固定值32，故Lane ID的取值范围为[0,31]。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"simt\_api/device\_functions.h"头文件。 

```cpp
#include "simt_api/device_functions.h" 
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void kernel_laneid(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id = laneid();
        dst[idx] = lane_id;
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) void kernel_laneid(__gm__ int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id = laneid();
        dst[idx] = lane_id;
    }
    ```
