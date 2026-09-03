# asc_exp_sub

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

根据`mask`将`src0`与`src1`按元素相减，并计算以差值为指数的自然指数函数。

- `asc_exp_sub`用于`float`类型输入。计算公式如下：

  $$
  result_i = e^{src0_i - src1_i}
  $$

- `asc_exp_sub_half2float`用于`half`类型输入。该接口支持读取源操作数中偶数索引或奇数索引的元素，将其转换为`float`类型后进行计算。计算公式如下：

  $$
  \begin{cases}
  result_i = e^{float(src0_{2i}) - float(src1_{2i})} & \text{提取偶数索引元素} \\
  result_i = e^{float(src0_{2i+1}) - float(src1_{2i+1})} & \text{提取奇数索引元素}
  \end{cases}
  $$

## 函数原型

- 用于处理`float`数据类型的源操作数。

  ```c
  __simd_callee__ inline void asc_exp_sub(vector_float& dst,
                                          vector_float src0,
                                          vector_float src1,
                                          vector_bool mask)
  ```

- 处理`half`数据类型的源操作数，支持读取偶数索引或奇数索引的源数据。

  ```c
  __simd_callee__ inline void asc_exp_sub_half2float(
      vector_float& dst,
      vector_half src0,
      vector_half src1,
      vector_bool mask,
      std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)

  __simd_callee__ inline void asc_exp_sub_half2float(
      vector_float& dst,
      vector_half src0,
      vector_half src1,
      vector_bool mask,
      std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
  ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ---- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数0（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数1（矢量数据寄存器）。 |
| mask | 输入 | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与输出元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| src_pos | 输入 | 指示源操作数读取位置。取值为`ASC_POSITION_EVEN`时读取偶数索引元素，取值为`ASC_POSITION_ODD`时读取奇数索引元素。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 只有当输出数据类型位宽大于输入时，计算时才会有精度提升。

<!-- npu="950" id8 -->

## 调用示例

将以下代码保存为`example.asc`后，可通过`bisheng`命令编译运行。其中，`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
bool compare_data(const std::vector<float>& actual, const std::vector<float>& expected, double tolerance)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t FLOAT_COUNT = 64;
constexpr uint32_t HALF_COUNT = 128;

__simd_vf__ inline void compute(__ubuf__ float* float_dst, __ubuf__ float* half_even_dst,
    __ubuf__ float* half_odd_dst, __ubuf__ float* float_src0, __ubuf__ float* float_src1,
    __ubuf__ half* half_src0, __ubuf__ half* half_src1)
{
    vector_float float_src0_reg;
    vector_float float_src1_reg;
    vector_half half_src0_reg;
    vector_half half_src1_reg;
    asc_loadalign(float_src0_reg, float_src0);
    asc_loadalign(float_src1_reg, float_src1);
    asc_loadalign(half_src0_reg, half_src0);
    asc_loadalign(half_src1_reg, half_src1);

    uint32_t count = FLOAT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    vector_bool mask_b16 = asc_create_mask_b16(PAT_ALL);
    vector_float float_result;
    asc_exp_sub(float_result, float_src0_reg, float_src1_reg, mask);
    vector_float half_even_result;
    asc_exp_sub_half2float(half_even_result, half_src0_reg, half_src1_reg, mask_b16,
        ASC_POSITION_EVEN);
    vector_float half_odd_result;
    asc_exp_sub_half2float(half_odd_result, half_src0_reg, half_src1_reg, mask_b16,
        ASC_POSITION_ODD);

    asc_storealign(float_dst, float_result, mask);
    asc_storealign(half_even_dst, half_even_result, mask);
    asc_storealign(half_odd_dst, half_odd_result, mask);
}

__global__ __vector__ void asc_exp_sub_kernel(__gm__ float* float_dst, __gm__ float* half_even_dst,
    __gm__ float* half_odd_dst, __gm__ float* float_src0, __gm__ float* float_src1,
    __gm__ half* half_src0, __gm__ half* half_src1)
{
    asc_init();
    __ubuf__ float float_dst_local[FLOAT_COUNT];
    __ubuf__ float half_even_dst_local[FLOAT_COUNT];
    __ubuf__ float half_odd_dst_local[FLOAT_COUNT];
    __ubuf__ float float_src0_local[FLOAT_COUNT];
    __ubuf__ float float_src1_local[FLOAT_COUNT];
    __ubuf__ half half_src0_local[HALF_COUNT];
    __ubuf__ half half_src1_local[HALF_COUNT];
    asc_copy_gm2ub_align(float_src0_local, float_src0, FLOAT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(float_src1_local, float_src1, FLOAT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(half_src0_local, half_src0, HALF_COUNT * sizeof(half));
    asc_copy_gm2ub_align(half_src1_local, half_src1, HALF_COUNT * sizeof(half));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    compute(float_dst_local, half_even_dst_local, half_odd_dst_local, float_src0_local,
        float_src1_local, half_src0_local, half_src1_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(float_dst, float_dst_local, FLOAT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(half_even_dst, half_even_dst_local, FLOAT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(half_odd_dst, half_odd_dst_local, FLOAT_COUNT * sizeof(float));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> float_src0(FLOAT_COUNT);
    std::vector<float> float_src1(FLOAT_COUNT);
    std::vector<half> half_src0(HALF_COUNT);
    std::vector<half> half_src1(HALF_COUNT);
    std::vector<float> float_output(FLOAT_COUNT);
    std::vector<float> half_even_output(FLOAT_COUNT);
    std::vector<float> half_odd_output(FLOAT_COUNT);
    std::vector<float> float_golden(FLOAT_COUNT);
    std::vector<float> half_even_golden(FLOAT_COUNT);
    std::vector<float> half_odd_golden(FLOAT_COUNT);
    for (uint32_t i = 0; i < FLOAT_COUNT; ++i) {
        float_src0[i] = static_cast<float>(i % 8) * 0.125f;
        float_src1[i] = static_cast<float>((i + 1) % 8) * 0.125f;
        float_golden[i] = std::exp(float_src0[i] - float_src1[i]);
    }
    for (uint32_t i = 0; i < HALF_COUNT; ++i) {
        half_src0[i] = static_cast<half>(static_cast<float>(i % 8) * 0.125f);
        half_src1[i] = static_cast<half>(static_cast<float>((i + 2) % 8) * 0.125f);
    }
    for (uint32_t i = 0; i < FLOAT_COUNT; ++i) {
        half_even_golden[i] = std::exp(static_cast<float>(half_src0[2 * i]) -
                                       static_cast<float>(half_src1[2 * i]));
        half_odd_golden[i] = std::exp(static_cast<float>(half_src0[2 * i + 1]) -
                                      static_cast<float>(half_src1[2 * i + 1]));
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* float_dst_device = nullptr;
    float* half_even_dst_device = nullptr;
    float* half_odd_dst_device = nullptr;
    float* float_src0_device = nullptr;
    float* float_src1_device = nullptr;
    half* half_src0_device = nullptr;
    half* half_src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&float_dst_device), FLOAT_COUNT * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&half_even_dst_device), FLOAT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&half_odd_dst_device), FLOAT_COUNT * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&float_src0_device), FLOAT_COUNT * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&float_src1_device), FLOAT_COUNT * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&half_src0_device), HALF_COUNT * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&half_src1_device), HALF_COUNT * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(float_src0_device, FLOAT_COUNT * sizeof(float), float_src0.data(), FLOAT_COUNT * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(float_src1_device, FLOAT_COUNT * sizeof(float), float_src1.data(), FLOAT_COUNT * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(half_src0_device, HALF_COUNT * sizeof(half), half_src0.data(), HALF_COUNT * sizeof(half),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(half_src1_device, HALF_COUNT * sizeof(half), half_src1.data(), HALF_COUNT * sizeof(half),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_exp_sub_kernel<<<1, 0>>>(float_dst_device, half_even_dst_device, half_odd_dst_device,
        float_src0_device, float_src1_device, half_src0_device, half_src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(float_output.data(), FLOAT_COUNT * sizeof(float), float_dst_device, FLOAT_COUNT * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(half_even_output.data(), FLOAT_COUNT * sizeof(float), half_even_dst_device,
        FLOAT_COUNT * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(half_odd_output.data(), FLOAT_COUNT * sizeof(float), half_odd_dst_device,
        FLOAT_COUNT * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = compare_data(float_output, float_golden, 1e-4) &&
                        compare_data(half_even_output, half_even_golden, 1e-4) &&
                        compare_data(half_odd_output, half_odd_golden, 1e-4);
    std::cout << (passed ? "[Success] asc_exp_sub passed." : "[Failed] asc_exp_sub failed.") << std::endl;
    aclrtFree(float_dst_device);
    aclrtFree(half_even_dst_device);
    aclrtFree(half_odd_dst_device);
    aclrtFree(float_src0_device);
    aclrtFree(float_src1_device);
    aclrtFree(half_src0_device);
    aclrtFree(half_src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```


<!-- end id8 -->
