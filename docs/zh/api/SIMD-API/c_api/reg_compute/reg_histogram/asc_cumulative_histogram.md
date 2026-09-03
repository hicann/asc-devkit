# asc_cumulative_histogram

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

头文件路径为：`"c_api/reg_compute/compute/reg_histogram.h"`。

对输入矢量数据寄存器中的元素进行累计频率统计，生成累计直方图。统计结果在目的矢量数据寄存器原有数据基础上累加。支持配置掩码用于指示参与统计的元素，掩码位为1时，对应元素参与统计；为0时不统计。掩码不影响目的矢量数据寄存器的写入行为。本接口在Vector Function（`__simd_vf__`标记的函数）内使用。

由于源矢量数据寄存器`src`的数据类型为`uint8_t`，取值范围为[0, 255]，而目的矢量数据寄存器`dst`的元素类型为`uint16_t`，且一个Vector Length可存储128个`uint16_t`数据，因此本接口支持以下两种模式，其中n为`dst`的元素索引，n ∈ {0, 1, ..., 127}：

- **低位模式（`BIN0`）**：`dst[n]`累加`src`中取值不大于`n`的元素个数，累计阈值范围为[0, 127]。
- **高位模式（`BIN1`）**：`dst[n]`累加`src`中取值不大于`128 + n`的元素个数，累计阈值范围为[128, 255]。

示例如下图所示：

![累计统计](../../../../figures/asc_cumulative_histogram.png)

本接口仅在AIV上生效。

## 函数原型

### 低位模式

```c
__simd_callee__ inline void asc_cumulative_histogram_bin0(vector_uint16_t& dst,
                                                          vector_uint8_t src,
                                                          vector_bool mask)
```

### 高位模式

```c
__simd_callee__ inline void asc_cumulative_histogram_bin1(vector_uint16_t& dst,
                                                          vector_uint8_t src,
                                                          vector_bool mask)
```

`src`支持的数据类型为`uint8_t`，`dst`支持的数据类型为`uint16_t`。

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输入/输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。mask中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 本接口在非AIV上调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- `mask`需通过[掩码设置接口](../../defs/type/data_type_definition.md#掩码寄存器)预先赋值后再传入；未赋值的掩码寄存器内容不确定，会导致参与统计的元素位置错误。

### 计算约束

- `mask`用于筛选源操作数，不影响`dst`中未被累加位置的原有数据和写入行为。掩码位为0时，源操作数`src`对应位置的数值将被忽略，`dst`对应位置数值为忽略该位置`src`后统计得到的值。
- `dst`中的统计结果在原有数据基础上累加。首次统计前需初始化`dst`；多次调用本接口时，后一次统计结果累加到前一次统计结果中。
- `dst`的数据类型为`uint16_t`，单个统计结果的最大值为65535。用户需保证累加后的统计结果不超过该范围，否则会发生溢出。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

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

constexpr uint32_t SRC_COUNT = 256;
constexpr uint32_t DST_COUNT = 128;

__simd_vf__ inline void histogram(__ubuf__ uint16_t* low, __ubuf__ uint16_t* high, __ubuf__ uint8_t* src)
{
    vector_uint16_t low_reg, high_reg;
    vector_uint8_t src_reg;
    uint32_t src_count = SRC_COUNT;
    vector_bool src_mask = asc_update_mask_b8(src_count);
    uint32_t dst_count = DST_COUNT;
    vector_bool dst_mask = asc_update_mask_b16(dst_count);
    asc_duplicate_scalar(low_reg, static_cast<uint16_t>(0), dst_mask);
    asc_duplicate_scalar(high_reg, static_cast<uint16_t>(0), dst_mask);
    asc_loadalign(src_reg, src);
    asc_cumulative_histogram_bin0(low_reg, src_reg, src_mask);
    asc_cumulative_histogram_bin1(high_reg, src_reg, src_mask);
    asc_storealign(low, low_reg, dst_mask);
    asc_storealign(high, high_reg, dst_mask);
}

__global__ __vector__ void asc_cumulative_histogram_kernel(__gm__ uint16_t* low, __gm__ uint16_t* high, __gm__ uint8_t* src)
{
    asc_init();
    __ubuf__ uint16_t low_local[DST_COUNT], high_local[DST_COUNT];
    __ubuf__ uint8_t src_local[SRC_COUNT];
    asc_copy_gm2ub_align(src_local, src, SRC_COUNT);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    histogram(low_local, high_local, src_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(low, low_local, DST_COUNT * sizeof(uint16_t));
    asc_copy_ub2gm_align(high, high_local, DST_COUNT * sizeof(uint16_t));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> src(SRC_COUNT);
    std::vector<uint16_t> low(DST_COUNT, 0), high(DST_COUNT, 0);
    std::vector<uint16_t> golden_low(DST_COUNT), golden_high(DST_COUNT);
    for (uint32_t i = 0; i < SRC_COUNT; ++i) src[i] = static_cast<uint8_t>(i);
    for (uint32_t i = 0; i < DST_COUNT; ++i) {
        golden_low[i] = i + 1;
        golden_high[i] = DST_COUNT + i + 1;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (SRC_COUNT) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* low_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&low_device), (DST_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* high_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&high_device), (DST_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(uint8_t), src.data(), src.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_cumulative_histogram_kernel<<<1, 0>>>(low_device, high_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(low.data(), low.size() * sizeof(uint16_t), low_device, low.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(high.data(), high.size() * sizeof(uint16_t), high_device, high.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", src);
    print_data("BIN0 output", low);
    print_data("BIN1 output", high);
    print_data("BIN0 Golden", golden_low);
    print_data("BIN1 Golden", golden_high);
    const bool passed = compare_data(low, golden_low) && compare_data(high, golden_high);
    std::cout << (passed ? "[Success] asc_cumulative_histogram_bin0/bin1 passed." : "[Failed] asc_cumulative_histogram_bin0/bin1 failed.") << std::endl;
    aclrtFree(high_device);
    aclrtFree(low_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
