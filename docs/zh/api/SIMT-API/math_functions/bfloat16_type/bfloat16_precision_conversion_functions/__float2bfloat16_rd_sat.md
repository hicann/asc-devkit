# \_\_float2bfloat16\_rd\_sat

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd_sat_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/asc_bf16.h"`。

饱和模式下，将浮点数转换为bfloat16精度，并遵循CAST\_FLOOR模式，返回转换后的值。

## 函数原型

```cpp
inline bfloat16_t __float2bfloat16_rd_sat(const float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

饱和模式下将输入遵循CAST\_FLOOR模式转换成的bfloat16类型数据。特殊值如下：

| x值 | 非饱和模式返回值 | 饱和模式返回值 |
|---|---|---|
| 0 | 0 | 0 |
| -0 | -0 | -0 |
| nan | nan | 0 |
| inf | inf | ASCRT_MAX_NORMAL_BF16 |
| -inf | -inf | -ASCRT_MAX_NORMAL_BF16 |
| ＞ASCRT_MAX_NORMAL_BF16 | ASCRT_MAX_NORMAL_BF16 | ASCRT_MAX_NORMAL_BF16 |
| ＜-ASCRT_MAX_NORMAL_BF16 | -inf | -ASCRT_MAX_NORMAL_BF16 |

## 约束说明

使用此接口前需将CTRL\[60\]寄存器设置为0，否则饱和模式不生效。设置方式请参见[控制饱和行为的方式](../../data_type_conversion/overview.md#section1194916101549)。

SIMT编程场景由于无法设置CTRL寄存器，本接口的饱和模式不生效。

## 调用示例

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2bfloat16_rd_sat(__gm__ bfloat16_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2bfloat16_rd_sat(x[idx]);
    }
    ```
