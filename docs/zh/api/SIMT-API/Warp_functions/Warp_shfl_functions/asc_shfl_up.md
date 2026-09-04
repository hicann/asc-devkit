# asc\_shfl\_up

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_up_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_warp_functions.h"`（除half、half2、bfloat16\_t、bfloat16x2\_t类型之外的接口）、`"simt_api/asc_fp16.h"`（half和half2类型接口）、`"simt_api/asc_bf16.h"`（bfloat16\_t和bfloat16x2\_t类型接口）。

获取Warp内当前线程向前偏移delta（当前线程Lane ID - delta）的线程输入的用于交换的var值；如果目标线程是非活跃状态，获取到寄存器中未初始化的值。其中，参数width用于划分Warp内线程的分组。参数width设置参与交换的32个线程的分组宽度，默认值为32，即所有线程分为1组。

在多个分组场景（width小于32）下，每个分组交换操作是独立的，每个线程获取本组内当前线程向前偏移delta的线程的var值。如果当前线程向前偏移delta的线程编号，即Lane ID - delta，小于所在分组的起始Lane ID，则返回当前线程的var值。

例如，Warp内32个活跃线程调用asc\_shfl\_up\(LaneId, 2, 16\)接口，每个线程的返回值为当前线程Lane ID - 2对应线程的var值，或者当前线程的var值。

**图1**  asc\_shfl\_up结果示意图

![](../../../figures/asc_shfl_up_result.png "asc_shfl_up结果示意图")

## 函数原型

```cpp
inline int32_t asc_shfl_up(int32_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline uint32_t asc_shfl_up(uint32_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline float asc_shfl_up(float var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline int64_t asc_shfl_up(int64_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline uint64_t asc_shfl_up(uint64_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline half asc_shfl_up(half var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline half2 asc_shfl_up(half2 var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline bfloat16_t asc_shfl_up(bfloat16_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline bfloat16x2_t asc_shfl_up(bfloat16x2_t var, uint32_t delta, int32_t width = warpSize)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| var | 输入 | 线程用于交换的输入操作数。 |
| delta | 输入 | 期望获取的var值所在线程相对当前线程的向前偏移值。 |
| width | 输入 | Warp内参与交换的线程的分组宽度，默认值为32。width的取值范围为(0, 32]，width必须是2的倍数。 |

## 返回值说明

Warp内指定线程的var值。

## 约束说明

-   如果目标线程是非活跃状态，获取到寄存器中未初始化的值。
-   若delta \>=width，所有线程都返回当前线程的var值。
-   width不是2的倍数或者超出32，返回值异常。

## 调用示例

完整样例请参考[Sobel边缘检测样例](../../../../../../examples/03_simt_api/02_features/01_api_features/03_warp_instruction/sobel_warp_shfl/README.md)。

以下示例包含两类用法：示例1使用asc\_shfl\_up获取Warp分组内相对当前线程向前偏移delta的线程的输入值；示例2使用asc\_shfl\_up在每个Warp内进行归约求和。

-   示例1：

    SIMT编程场景：

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelShflUp(int32_t* dst)
    {
         int idx = threadIdx.x + blockIdx.x * blockDim.x;
         int32_t laneId = idx % 32;
         // 0-15线程返回值分别为{0,1,0,1,2,3,4,5,6,7,8,9,10,11,12,13}
         // 16-31线程返回值为{16,17,16,17,18,19,20,21,22,23,24,25,26,27,28,29}

         int32_t result = asc_shfl_up(laneId, 2, 16);
         dst[idx] = result;
    }
    ```

    SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelShflUp(__gm__ int32_t* dst)
    {
         // asc_vf_call参数：dim3{1024, 1, 1}
         int idx = threadIdx.x + blockIdx.x * blockDim.x;
         int32_t laneId = idx % 32;
         // 0-15线程返回值分别为{0,1,0,1,2,3,4,5,6,7,8,9,10,11,12,13}
         // 16-31线程返回值为{16,17,16,17,18,19,20,21,22,23,24,25,26,27,28,29}

         int32_t result = asc_shfl_up(laneId, 2, 16);
         dst[idx] = result;
    }
    ```

-   示例2：

    SIMT编程场景：

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelShflUpReduceSum(int32_t* dst)
    {
         int idx = threadIdx.x + blockIdx.x * blockDim.x;
         int32_t laneId = idx % 32;
         int32_t value = laneId;

         value += asc_shfl_up(value, 16, 32);
         value += asc_shfl_up(value, 8, 32);
         value += asc_shfl_up(value, 4, 32);
         value += asc_shfl_up(value, 2, 32);
         value += asc_shfl_up(value, 1, 32);

         dst[idx] = value; // 归约求和结果位于每个Warp内laneId为31的线程
    }
    ```

    SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelShflUpReduceSum(__gm__ int32_t* dst)
    {
         // asc_vf_call参数：dim3{1024, 1, 1}
         int idx = threadIdx.x + blockIdx.x * blockDim.x;
         int32_t laneId = idx % 32;
         int32_t value = laneId;

         value += asc_shfl_up(value, 16, 32);
         value += asc_shfl_up(value, 8, 32);
         value += asc_shfl_up(value, 4, 32);
         value += asc_shfl_up(value, 2, 32);
         value += asc_shfl_up(value, 1, 32);

         dst[idx] = value; // 归约求和结果位于每个Warp内laneId为31的线程
    }
    ```
