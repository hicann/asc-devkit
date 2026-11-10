---
okftype: reference.api
description: "按mask把矢量寄存器元素与同类型标量逐元素相加，结果写入目的寄存器或由函数返回值返回，支持int8至float的常见dtype。"
tag:
  - roles: [compute]
  - operations: [add]
  - traits: [elementwise, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "正文据此说明矢量数据寄存器与掩码寄存器"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "正文据此引用 Reg 矢量计算通用约束"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:函数原型"
    scope_basis: resolved
    note: "新增函数返回值形式重载：探测 inline vector_* asc_add_scalar 首次出现在 9.2.0 的 reg_base_impl/reg_arith_intf_impl.h:1275（version-map 未给出该草稿，此处以声明史与 CANN 9.2.0 Release Notes 的 Reg 算术返回值重载条目为依据），对应正文 vector_<dtype> asc_add_scalar(vector_<dtype> src, <dtype> value, vector_bool mask)。"
---
# asc_add_scalar

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/compute/reg_arith.h"`。

根据`mask`将源操作数`src`按元素与标量`value`相加，得到计算结果。计算公式如下：

$$
dst_i = src_i + value
$$

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_add_scalar(vector_<dtype> src,
                                                     <dtype> value,
                                                     vector_bool mask)

// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_add_scalar(vector_<dtype>& dst,
                                           vector_<dtype> src,
                                           <dtype> value,
                                           vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器与half标量执行加法运算
__simd_callee__ inline void asc_add_scalar(vector_half& dst,
                                           vector_half src,
                                           half value,
                                           vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| value | 输入 | 源操作数（标量）。 |
| mask | 输入 | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果：计算结果写入`dst`。
- 通过函数返回值返回结果：返回计算结果，类型为矢量数据寄存器，与`src`的数据类型一致。`mask`掩码位为0的元素在返回值中置0。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- `dst`与`src`的数据类型需要保持一致。
- `mask`掩码位为0时，结果对应元素置0。

## 调用示例

将以下代码保存为`example.asc`后，可通过`bisheng`命令编译运行。其中，`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(float);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr float SCALAR_VALUE = 2.0f;

__simd_vf__ inline void compute(__ubuf__ float* dst, __ubuf__ float* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float dst_reg;
    vector_float src_reg;
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src_reg, src + offset);
        asc_add_scalar(dst_reg, src_reg, SCALAR_VALUE, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_add_scalar_kernel(__gm__ float* dst, __gm__ float* src)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, TOTAL_LENGTH * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<compute>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}

} // namespace

int main()
{
    std::vector<float> src(TOTAL_LENGTH);
    std::vector<float> output(TOTAL_LENGTH, 0.0f);
    std::vector<float> golden(TOTAL_LENGTH);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src[i] = (static_cast<int>(i) - 16) * 0.25f;
        golden[i] = src[i] + SCALAR_VALUE;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(float), src.data(), src.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_add_scalar_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = compare_data(output, golden, 1e-6);
    std::cout << (passed ? "[Success] asc_add_scalar passed." : "[Failed] asc_add_scalar failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
