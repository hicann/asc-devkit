# clock

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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
<!-- npu="x90" id7 -->
- Kirin X90：不支持
<!-- end id7 -->
<!-- npu="9030" id8 -->
- Kirin 9030：不支持
<!-- end id8 -->

## 功能说明

本接口在SIMD和SIMT调试场景中提供Clock时间戳功能，用于记录从程序启动到接口调用时刻所经历的时钟周期数（Cycle Count），便于精确分析执行延迟和性能瓶颈。

## 函数原型

```cpp
__aicore__ inline uint64_t clock(void)
```

<!-- npu="950" id9 -->
以下接口为SIMT中所使用的clock接口，仅支持Ascend 950PR/Ascend 950DT。

```cpp
inline uint64_t clock(void)
```
<!-- end id9 -->

## 参数说明

无

## 返回值说明

从程序开始到调用时所经历的时钟周期数。

## 约束说明

<!-- npu="950,A3,910b" id13 -->
-   SIMT场景的支持情况如下：
      <!-- npu="950" id10 -->
    - Ascend 950PR/Ascend 950DT：支持。
      <!-- end id10 -->
      <!-- npu="A3" id11 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持。
      <!-- end id11 -->
      <!-- npu="910b" id12 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持。
      <!-- end id12 -->
<!-- end id13 -->

## 需要包含的头文件

使用该接口需要包含"utils/debug/asc\_time.h"头文件。

```cpp
#include "utils/debug/asc_time.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void SimtKernel(uint64_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = clock();
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void SimtKernel(__gm__ uint64_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = clock();
    }
    ```

-   SIMD编程场景：

    ```cpp
    __global__ __aicore__ void AicoreKernel(__gm__ uint64_t* dst)
    {
        int idx = AscendC::GetBlockIdx();
        dst[idx] = clock();
    }
    ```
