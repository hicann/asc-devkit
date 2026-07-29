# \_\_float22half2\_rz\_sat

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

饱和模式下，将float2类型数据的两个分量遵循CAST\_TRUNC模式转换为半精度浮点数，返回转换后的half2类型数据。

## 函数原型

```
inline half2 __float22half2_rz_sat(const float2 x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

饱和模式下将输入的两个分量遵循CAST\_TRUNC模式转换成的half2类型数据。

## 约束说明

使用此接口前需将CTRL\[60\]寄存器设置为0，否则饱和模式不生效。设置方式请参见[控制饱和行为的方式](../../数据类型转换/概述-258.md#控制饱和行为的方式)。

SIMT编程场景由于无法设置CTRL寄存器，本接口的饱和模式不生效。

## 需要包含的头文件

使用该接口需要包含"simt\_api/asc\_fp16.h"头文件。

```
#include "simt_api/asc_fp16.h"
```

## 调用示例

-   SIMD与SIMT混合编程场景：

    ```
    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_float22half2_rz_sat(__gm__ float2* input, __gm__ half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个float2类型的数据，即2个float类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __float22half2_rz_sat(input[idx]);
    }

    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ half* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_float22half2_rz_sat>(dim3(1024), (__gm__ float2*)input, (__gm__ half2*)output, input_total_length);
    }
    ```

