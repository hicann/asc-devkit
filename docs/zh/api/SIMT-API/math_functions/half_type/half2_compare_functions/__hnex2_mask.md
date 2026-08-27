# \_\_hnex2\_mask

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_compare_functions/__hnex2_mask_res.md#id1 -->

## 功能说明

比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。

## 函数原型

```cpp
unsigned int __hnex2_mask(half2 x, half2 y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

比较输入数据各分量是否不相等的结果：满足时对应16位掩码结果为0xFFFF，不满足时对应16位掩码结果为0x0。各分量掩码结果如下：

<table>
  <tr>
    <th>x分量</th>
    <th>y分量</th>
    <th>返回值（对应分量）</th>
  </tr>
  <tr>
    <td>±0</td>
    <td>±0</td>
    <td>0x0</td>
  </tr>
  <tr>
    <td colspan="2">任一分量为nan</td>
    <td>0x0</td>
  </tr>
  <tr>
    <td>±inf</td>
    <td>±inf</td>
    <td>0x0</td>
  </tr>
</table>


## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/asc_fp16.h`头文件。

```cpp
#include "simt_api/asc_fp16.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    // 使用短向量可提升数据搬运效率
    __global__ __launch_bounds__(1024) void simt_hnex2_mask(half* x, half* y, unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个half2类型的数据，即2个half类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        dst[idx] = __hnex2_mask(input1[idx], input2[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_hnex2_mask(__gm__ half2* x, __gm__ half2* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个half2类型的数据，即2个half类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hnex2_mask(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hnex2_mask>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, dst, input_total_length);
    }
    ```
