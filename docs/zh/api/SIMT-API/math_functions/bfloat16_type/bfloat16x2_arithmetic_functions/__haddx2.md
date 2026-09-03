# \_\_haddx2

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__haddx2_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/asc_bf16.h"`。

计算两个bfloat16x2\_t类型数据各分量的相加结果，并遵循CAST\_RINT模式对结果进行舍入。

## 函数原型

```cpp
bfloat16x2_t __haddx2(const bfloat16x2_t x, const bfloat16x2_t y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

输入数据各分量相加的结果。相加的分量x和y满足：

<table>
  <thead>
    <tr>
      <th>x分量值</th>
      <th>y分量值</th>
      <th>返回值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>±0</td>
      <td>±0</td>
      <td>±0</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>inf</td>
      <td>inf</td>
    </tr>
    <tr>
      <td>-inf</td>
      <td>-inf</td>
      <td>-inf</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>-inf</td>
      <td>nan</td>
    </tr>
    <tr>
      <td>-inf</td>
      <td>inf</td>
      <td>nan</td>
    </tr>
    <tr>
      <td colspan="2">x值或y值任意一个为nan</td>
      <td>nan</td>
    </tr>
    <tr>
      <td colspan="2">对有限值x（包括±0），x=-y</td>
      <td>0</td>
    </tr>
    <tr>
      <td>ASCRT_MAX_NORMAL_BF16</td>
      <td>ASCRT_MAX_NORMAL_BF16</td>
      <td>inf</td>
    </tr>
  </tbody>
</table>

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/asc_bf16.h"

    // 使用短向量可提升数据搬运效率
    __global__ __launch_bounds__(1024) void simt_haddx2(bfloat16_t* x, bfloat16_t* y, bfloat16_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个bfloat16x2_t类型的数据，即2个bfloat16_t类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        bfloat16x2_t* out = (bfloat16x2_t*)dst;
        out[idx] = __haddx2(input1[idx], input2[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_bf16.h"

    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_haddx2(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ bfloat16x2_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个bfloat16x2_t类型的数据，即2个bfloat16_t类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __haddx2(x[idx], y[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_haddx2>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, (__gm__ bfloat16x2_t*)dst, input_total_length);
    }
    ```
