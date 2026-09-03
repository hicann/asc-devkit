# \_\_hfma

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/asc_bf16.h"`。

对输入数据x、y、z，计算x与y相乘加上z的结果，并遵循CAST\_RINT模式对结果进行舍入处理。

![](../../../../figures/zh-cn_formulaimage_0000002545900872.png)

## 函数原型

```cpp
inline bfloat16_t __hfma(bfloat16_t x, bfloat16_t y, bfloat16_t z)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |
| z | 输入 | 源操作数。 |

## 返回值说明

x \* y+ z的值。本接口不受全局饱和模式影响，特殊值如下：

| x值 | y值 | z值 | 返回值 |
| --- | --- | --- | --- |
| ±inf | ±0 | — | nan |
| ±0 | ±inf | — | nan |
| x*y = inf | — | -inf | nan |
| x*y = -inf | — | inf | nan |
| x*y+z超出ASCRT_MAX_NORMAL_BF16 | — | — | inf |
| x*y+z小于-ASCRT_MAX_NORMAL_BF16 | — | — | -inf |
| x、y、z任意一个为nan | — | — | nan |

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel_fma(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y, bfloat16_t* z, uint32_t total_length){
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hfma(x[idx], y[idx], z[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_fma(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* z, uint32_t total_length){
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hfma(x[idx], y[idx], z[idx]);
    }
    ```
