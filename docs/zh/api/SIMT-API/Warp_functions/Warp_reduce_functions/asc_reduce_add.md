# asc\_reduce\_add

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add_res.md#id1 -->

## 功能说明

对Warp内所有活跃线程输入的val求和。Warp内所有活跃线程返回相同的结果。

## 函数原型

```cpp
inline int32_t asc_reduce_add(int32_t val)
```

```cpp
inline uint32_t asc_reduce_add(uint32_t val)
```

```cpp
inline float asc_reduce_add(float val)
```

```cpp
inline half asc_reduce_add(half val)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| val | 输入 | 源操作数。 |

## 返回值说明

Warp内所有线程输入val的和。

## 约束说明

-   当数据求和结果溢出时，本接口不保证计算精度。
-   本接口底层实现使用二分算法，在某些场景计算结果与顺序计算的结果不一致。简单来说：\(\(\(a + b\) + c\) + d\)与\(\(a + b\) + \(c + d\)\)计算顺序不一致，可能会导致最终计算结果不同，这是由于在浮点数计算过程中，每次加法操作都涉及到有限精度的数值表示，这一过程中的舍入操作会导致精度损失，因此，不同的加法顺序可能会导致不同的中间结果，进而影响最终计算结果的精确度。

## 需要包含的头文件

使用除half类型之外的接口需要包含`simt_api/device_warp_functions.h`头文件，使用half类型接口需要包含`simt_api/asc_fp16.h`头文件。

```cpp
#include "simt_api/device_warp_functions.h"
```

```cpp
#include "simt_api/asc_fp16.h"
```

## 调用示例

完整样例请参考[MemoryFence样例](../../../../../../examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md)。

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void KernelReduceAdd(int32_t* dst, int32_t* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t result = asc_reduce_add(src[idx]);
        if (laneid() == 0) {
            asc_atomic_add(dst, result);
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void KernelReduceAdd(__gm__ int32_t* dst, __gm__ int32_t* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t result = asc_reduce_add(src[idx]);
        if (laneid() == 0) {
            asc_atomic_add(dst, result);
        }
    }
    ```
