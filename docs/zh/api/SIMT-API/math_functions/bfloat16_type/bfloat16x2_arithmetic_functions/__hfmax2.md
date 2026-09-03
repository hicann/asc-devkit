# \_\_hfmax2

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/asc_bf16.h"`。

计算输入bfloat16x2\_t类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST\_RINT模式对结果进行舍入。

## 函数原型

```cpp
bfloat16x2_t __hfmax2(const bfloat16x2_t x, const bfloat16x2_t y, const bfloat16x2_t z)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |
| z | 输入 | 源操作数。 |

## 返回值说明

输入数据各分量乘加的结果。计算的分量a、b、c满足：

<table>
  <thead>
    <tr>
      <th>x分量值</th>
      <th>y分量值</th>
      <th>z分量值</th>
      <th>返回值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td colspan="3">x、y、z任意一个为nan</td>
      <td>nan</td>
    </tr>
    <tr>
      <td>±inf</td>
      <td>±0</td>
      <td>任意值</td>
      <td>nan</td>
    </tr>
    <tr>
      <td>±0</td>
      <td>±inf</td>
      <td>任意值</td>
      <td>nan</td>
    </tr>
    <tr>
      <td colspan="2">x*y为inf</td>
      <td>-inf</td>
      <td>nan</td>
    </tr>
    <tr>
      <td colspan="2">x*y为-inf</td>
      <td>inf</td>
      <td>nan</td>
    </tr>
    <tr>
      <td>±inf</td>
      <td>非±0</td>
      <td>0</td>
      <td>±inf</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>inf</td>
      <td>0</td>
      <td>inf</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>-inf</td>
      <td>0</td>
      <td>-inf</td>
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
    __global__ __launch_bounds__(1024) void simt_hfmax2(bfloat16_t* x, bfloat16_t* y, bfloat16_t* z, bfloat16_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个bfloat16x2_t类型的数据，即2个bfloat16_t类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        bfloat16x2_t* input3 = (bfloat16x2_t*)z;
        bfloat16x2_t* out = (bfloat16x2_t*)dst;
        out[idx] = __hfmax2(input1[idx], input2[idx], input3[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_bf16.h"

    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_hfmax2(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ bfloat16x2_t* z, __gm__ bfloat16x2_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个bfloat16x2_t类型的数据，即2个bfloat16_t类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hfmax2(x[idx], y[idx], z[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* z, __gm__ bfloat16_t* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hfmax2>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, (__gm__ bfloat16x2_t*)z, (__gm__ bfloat16x2_t*)dst, input_total_length);
    }
    ```
