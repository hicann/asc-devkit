# \_\_hgeu

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_compare_functions/__hgeu_res.md#id1 -->

## 功能说明

比较两个half类型数据，当第一个数大于或等于第二个数时，返回true。若任一输入为nan，返回true。

## 函数原型

```cpp
bool __hgeu(half x, half y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

比较输入的第一个数是否大于或等于第二个数的结果。特殊值如下：

<table>
  <tr>
    <th>x值</th>
    <th>y值</th>
    <th>返回值</th>
  </tr>
  <tr>
    <td colspan="2">x&lt;y</td>
    <td>false</td>
  </tr>
  <tr>
    <td colspan="2">x&gt;y</td>
    <td>true</td>
  </tr>
  <tr>
    <td colspan="2">x=y</td>
    <td>true</td>
  </tr>
  <tr>
    <td colspan="2">任一值为nan</td>
    <td>true</td>
  </tr>
  <tr>
    <td>±0</td>
    <td>±0</td>
    <td>true</td>
  </tr>
  <tr>
    <td>inf</td>
    <td>有限值</td>
    <td>true</td>
  </tr>
  <tr>
    <td>-inf</td>
    <td>有限值</td>
    <td>false</td>
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
    __global__ __launch_bounds__(1024) void KernelHgeu(bool* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hgeu(x[idx], y[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void KernelHgeu(__gm__ bool* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hgeu(x[idx], y[idx]);
    }
    ```
