# \_\_asc\_cvt\_float2\_to\_fp8x2

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__asc_cvt_float2_to_fp8x2_res.md#id1 -->

## 功能说明

将float2类型数据的两个分量，按照CAST\_RINT模式转换为指定类型（float8\_e4m3x2\_t和float8\_e5m2x2\_t）的8位浮点数，并根据指定的饱和模式（饱和或非饱和）进行溢出处理。转换结果以位级打包形式存储为\_\_asc\_fp8x2\_storage\_t类型，该类型为16位无符号整数unsigned short int，用于存储float8\_e4m3x2\_t或float8\_e5m2x2\_t类型的数据。

## 函数原型

```cpp
inline __asc_fp8x2_storage_t __asc_cvt_float2_to_fp8x2(const float2 x, const __asc_saturation_t saturate, const __asc_fp8_interpretation_t fp8_interpretation)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| saturate | 输入 | 控制饱和行为，支持的取值为：__ASC_NOSAT、__ASC_SATFINITE。<br><br>__ASC_NOSAT表示使用非饱和模式，__ASC_SATFINITE表示使用饱和模式。 |
| fp8_interpretation | 输入 | 指定转换类型，支持的取值为：__ASC_E4M3、__ASC_E5M2。<br><br>__ASC_E4M3表示转换为float8_e4m3x2_t格式的浮点数，__ASC_E5M2表示转换为float8_e5m2x2_t格式的浮点数。 |

## 返回值说明

输入的两个分量遵循CAST\_RINT模式，根据指定的8位浮点数类型和指定的饱和模式，转换成的\_\_asc\_fp8x2\_storage\_t类型数据。本接口受全局饱和模式影响，特殊值如下：

-   fp8_interpretation参数为__ASC_E4M3时：

    | 各分量输入 | 非饱和模式返回值 | 饱和模式返回值 |
    |---|---|---|
    | ±0 | ±0 | ±0 |
    | nan | nan | +0 |
    | ±inf | nan | ±448 |
    | 超出float8_e4m3_t范围的正值 | nan | 448 |
    | 超出float8_e4m3_t范围的负值 | nan | -448 |

-   fp8_interpretation的参数为__ASC_E5M2时：

    | 各分量输入 | 非饱和模式返回值 | 饱和模式返回值 |
    |---|---|---|
    | ±0 | ±0 | ±0 |
    | nan | nan | +0 |
    | ±inf | ±inf | ±57344 |
    | 超出float8_e5m2_t范围的正值 | inf | 57344 |
    | 超出float8_e5m2_t范围的负值 | -inf | -57344 |

## 约束说明

使用此接口前需将CTRL\[60\]寄存器设置为0，否则饱和模式不生效。设置方式请参见[控制饱和行为的方式](../../data_type_conversion/overview.md#section1194916101549)。

SIMT编程场景当前不支持使用该接口。

## 需要包含的头文件

使用该接口需要包含`simt_api/asc_fp8.h`头文件。

```cpp
#include "simt_api/asc_fp8.h"
```

## 调用示例

-   SIMD与SIMT混合编程场景：

    ```cpp
    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_asc_cvt_float2_to_fp8x2(__gm__ float2* input, __gm__ uint16_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个float2类型的数据，即2个float类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __asc_cvt_float2_to_fp8x2(input[idx], __ASC_NOSAT, __ASC_E4M3);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ uint16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_asc_cvt_float2_to_fp8x2>(dim3(1024), (__gm__ float2*)input, output, input_total_length);
    }
    ```
