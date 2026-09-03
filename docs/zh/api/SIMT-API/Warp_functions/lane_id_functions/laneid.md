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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/lane_id_functions/laneid_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`。

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

## 调用示例

下面示例使用`laneid()`计算全部输入数据的和，并把结果赋值到输出数据的第0位置上，示例中使用[asc_reduce_add](../Warp_reduce_functions/asc_reduce_add.md)，需另外包含`simt_api/device_warp_functions.h`头文件，使用[asc_atomic_add](../../atomic_operations/asc_atomic_add.md)，需另外包含`simt_api/device_atomic_functions.h`头文件。

- SIMT编程场景：

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"
    #include "simt_api/device_atomic_functions.h"

    __global__ __launch_bounds__(1024) void kernel_laneid(int32_t* dst, int32_t* src, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        int32_t result = asc_reduce_add(src[idx]);
        if (laneid() == 0) {
            asc_atomic_add(dst, result);
        }
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"
    #include "simt_api/device_atomic_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_laneid(__gm__ int32_t* dst, __gm__ int32_t* src, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        int32_t result = asc_reduce_add(src[idx]);
        if (laneid() == 0) {
            asc_atomic_add(dst, result);
        }
    }
    ```
