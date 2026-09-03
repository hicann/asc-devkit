# asc_leakyrelu

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/compute/reg_fused.h"`。

根据掩码对源矢量数据寄存器中的元素执行Leaky ReLU操作，并将结果通过函数返回值返回或写入目的矢量数据寄存器。源操作数中大于0的元素直接写入输出结果，小于或等于0的元素乘以标量立即数后写入输出结果。掩码对应位置为1的元素参与计算，为0的元素在输出结果中置零。计算公式如下：

$$
dst_i = \begin{cases} src_i & src_i > 0 \\ src_i \times \alpha & src_i \leq 0 \end{cases}
$$

本接口仅在AIV上生效。

## 函数原型

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_leakyrelu(vector_<dtype> src,
                                                    <dtype> alpha,
                                                    vector_bool mask)

// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_leakyrelu(vector_<dtype>& dst,
                                          vector_<dtype> src,
                                          <dtype> alpha,
                                          vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`half`、`float`。

### 函数原型典型示例

```c
// 通过函数返回值返回结果
__simd_callee__ inline vector_half asc_leakyrelu(vector_half src,
                                                 half alpha,
                                                 vector_bool mask)

// 通过引用参数输出结果
__simd_callee__ inline void asc_leakyrelu(vector_half& dst,
                                          vector_half src,
                                          half alpha,
                                          vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量数据寄存器）。仅无返回值类型接口包含该参数，数据类型须与`src`一致。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| alpha | 输入 | 负半轴斜率（标量）。数据类型须与`src`一致。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。`mask`未筛选的元素在输出中置零。需通过掩码设置接口预先赋值后再传入。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

对于返回值类型接口，返回保存Leaky ReLU计算结果的矢量数据寄存器，数据类型与`src`保持一致。

## 约束说明

### 通用约束

- 非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- `mask`需通过掩码设置接口预先赋值后再传入；未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- 掩码位为0的元素位置不参与运算，输出结果对应位置写0。

### 计算约束

- 源矢量数据寄存器中数据为负零（-0）时按负数处理，乘法满足IEEE 754浮点乘法规则。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENT_COUNT = 64;
constexpr uint32_t BUFFER_BYTES = ELEMENT_COUNT * sizeof(float);
constexpr float ALPHA = 0.1f;

__simd_vf__ inline void leakyrelu_vf(__ubuf__ float* output, __ubuf__ float* input)
{
    vector_float src;
    vector_bool mask = asc_create_mask_b32(PAT_ALL);
    asc_loadalign(src, input);
    vector_float dst = asc_leakyrelu(src, ALPHA, mask);
    asc_storealign(output, dst, mask);
}

__global__ __vector__ void asc_leakyrelu_kernel(__gm__ float* output, __gm__ float* input)
{
    asc_init();
    __ubuf__ float output_local[ELEMENT_COUNT];
    __ubuf__ float input_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    leakyrelu_vf(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(ELEMENT_COUNT);
    std::vector<float> output(ELEMENT_COUNT, 0.0f);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        input[i] = static_cast<float>(static_cast<int32_t>(i) - 64);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* input_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_leakyrelu_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        const float expected = input[i] > 0.0f ? input[i] : input[i] * ALPHA;
        if (std::fabs(output[i] - expected) > 1e-6f) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_leakyrelu completed."
                         : "[Failed] asc_leakyrelu output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
