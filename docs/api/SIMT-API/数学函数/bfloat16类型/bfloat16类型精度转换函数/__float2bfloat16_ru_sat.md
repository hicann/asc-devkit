# \_\_float2bfloat16\_ru\_sat

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

饱和模式下，将浮点数转换为bfloat16精度，并遵循CAST\_CEIL模式，返回转换后的值。

## 函数原型

```
inline bfloat16_t __float2bfloat16_ru_sat(const float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

饱和模式下将输入遵循CAST\_CEIL模式转换成的bfloat16类型数据。

## 约束说明

使用此接口前需将CTRL\[60\]寄存器设置为0，否则饱和模式不生效。设置方式请参见[控制饱和行为的方式](../../数据类型转换/概述-258.md#控制饱和行为的方式)。

SIMT编程场景由于无法设置CTRL寄存器，本接口的饱和模式不生效。

## 需要包含的头文件

使用该接口需要包含"simt\_api/asc\_bf16.h"头文件。

```
#include "simt_api/asc_bf16.h"
```

## 调用示例

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2bfloat16_ru_sat(__gm__ bfloat16_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2bfloat16_ru_sat(x[idx]);
    }
    ```

