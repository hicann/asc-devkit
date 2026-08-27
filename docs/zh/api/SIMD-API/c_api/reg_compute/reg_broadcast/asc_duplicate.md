# asc_duplicate

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

将源操作数src的最低位元素（即下标为0的元素）广播到目的操作数dst中被mask筛选的位置。mask仅用于筛选dst中的元素位置，不影响从src中读取的元素；dst中未被mask筛选的位置置零。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
__simd_callee__ inline void asc_duplicate(vector_<dtype>& dst,
                                          vector_<dtype> src,
                                          vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

**典型示例：**

```c
__simd_callee__ inline void asc_duplicate(vector_int8_t& dst,
                                          vector_int8_t src,
                                          vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数（矢量数据寄存器），保存广播结果。数据类型需要与src保持一致。 |
| src | 输入 | 源操作数（矢量数据寄存器），其最低位元素作为待广播的数据。数据类型需要与dst保持一致。 |
| mask | 输入 | 源操作数元素操作的有效指示（掩码寄存器）。mask筛选的元素在dst中填充为src的最低位元素，未筛选的元素在dst中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 本接口为Reg矢量计算接口，只能在使用`__simd_vf__`标记的VF函数内调用，不支持在`__aicore__`函数中直接调用，仅在AIV上生效，在AIC上调用将直接返回。
- 同一寄存器的数据依赖由硬件保序，无需额外插入同步指令。本接口与前后Reg数据搬运接口之间，如果不同寄存器访问同一UB地址且存在写后读或写后写依赖，需要调用[asc_mem_bar](../reg_sync/asc_mem_bar.md)进行同步。
- 使用mask前，需要通过掩码设置或搬入接口完成初始化；未初始化的掩码寄存器内容不确定。
- mask仅筛选dst中写入广播值的位置，不筛选src中的元素。无论mask的最低位是否有效，待广播的数据均为src的最低位元素。

<!-- npu="950" id8 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

以下调用示例代码仅Ascend 950PR/Ascend 950DT产品支持。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values, uint32_t offset = 0)
{
    std::cout << label << ":";
    const size_t remaining = values.size() - offset;
    const size_t count = remaining < 8 ? remaining : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[offset + i];
    if (remaining > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t ELEMENT_COUNT = 64;
constexpr uint32_t ACTIVE_COUNT = 4;
constexpr uint32_t RESULT_COUNT = 3;
constexpr uint32_t LOWEST_ZERO_OFFSET = 0;
constexpr uint32_t HIGHEST_ZERO_OFFSET = ELEMENT_COUNT;
constexpr uint32_t HIGHEST_MERGE_OFFSET = ELEMENT_COUNT * 2;
constexpr float INITIAL_VALUE = -7.0f;

__simd_vf__ inline void duplicate_modes(__ubuf__ float* dst, __ubuf__ float* src)
{
    vector_float src_reg;
    vector_float lowest_zero_reg;
    vector_float highest_zero_reg;
    vector_float highest_merge_reg;
    uint32_t active_count = ACTIVE_COUNT;
    uint32_t full_count = ELEMENT_COUNT;
    vector_bool active_mask = asc_update_mask_b32(active_count);
    vector_bool full_mask = asc_update_mask_b32(full_count);

    asc_loadalign(src_reg, src);
    asc_loadalign(lowest_zero_reg, dst + LOWEST_ZERO_OFFSET);
    asc_loadalign(highest_zero_reg, dst + HIGHEST_ZERO_OFFSET);
    asc_loadalign(highest_merge_reg, dst + HIGHEST_MERGE_OFFSET);

    asc_duplicate(lowest_zero_reg, src_reg, active_mask);
    vdup(highest_zero_reg, src_reg, active_mask, POS_HIGHEST, MODE_ZEROING);
    vdup(highest_merge_reg, src_reg, active_mask, POS_HIGHEST, MODE_MERGING);

    asc_storealign(dst + LOWEST_ZERO_OFFSET, lowest_zero_reg, full_mask);
    asc_storealign(dst + HIGHEST_ZERO_OFFSET, highest_zero_reg, full_mask);
    asc_storealign(dst + HIGHEST_MERGE_OFFSET, highest_merge_reg, full_mask);
}

__global__ __vector__ void asc_duplicate_kernel(__gm__ float* dst, __gm__ float* src)
{
    asc_init();
    __ubuf__ float dst_local[ELEMENT_COUNT * RESULT_COUNT];
    __ubuf__ float src_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(dst_local, dst, sizeof(dst_local));
    asc_copy_gm2ub_align(src_local, src, sizeof(src_local));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    duplicate_modes(dst_local, src_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, sizeof(dst_local));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src(ELEMENT_COUNT);
    std::vector<float> output(ELEMENT_COUNT * RESULT_COUNT, INITIAL_VALUE);
    std::vector<float> golden(output.size());
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) src[i] = static_cast<float>(i + 1);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        const bool active = i < ACTIVE_COUNT;
        golden[LOWEST_ZERO_OFFSET + i] = active ? src.front() : 0.0f;
        golden[HIGHEST_ZERO_OFFSET + i] = active ? src.back() : 0.0f;
        golden[HIGHEST_MERGE_OFFSET + i] = active ? src.back() : INITIAL_VALUE;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src_device = nullptr, *dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), src.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(float), src.data(), src.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(dst_device, output.size() * sizeof(float), output.data(), output.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_duplicate_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Input lowest/highest: " << src.front() << ' ' << src.back() << std::endl;
    print_data("Lowest + zero", output, LOWEST_ZERO_OFFSET);
    print_data("Highest + zero", output, HIGHEST_ZERO_OFFSET);
    print_data("Highest + merge", output, HIGHEST_MERGE_OFFSET);
    print_data("Golden lowest + zero", golden, LOWEST_ZERO_OFFSET);
    print_data("Golden highest + zero", golden, HIGHEST_ZERO_OFFSET);
    print_data("Golden highest + merge", golden, HIGHEST_MERGE_OFFSET);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_duplicate passed."
                        : "[Failed] asc_duplicate failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
