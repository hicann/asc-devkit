# asc_frequency_histogram

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

对输入矢量数据寄存器中的元素进行频率统计，生成直方图。统计结果在目的矢量数据寄存器原有数据基础上累加。支持配置掩码用于指示参与统计的元素，掩码为1的元素参与统计，掩码为0的元素不统计，掩码不影响目的矢量寄存器的写入行为。本接口在Vector Function（`__simd_vf__`标记的函数）内使用。

由于源矢量数据寄存器`src`数据类型为`uint8_t`（取值范围0~255），而目的矢量数据寄存器`dst`每个元素为`uint16_t`，且一个Vector Length可存储128个`uint16_t`数据，因此本接口支持两种模式：

- **低位模式（`BIN0`）**：统计`src`中数值在[0, 127]范围内的出现次数，并累加进`dst`。`dst[0]`表示数值0的出现次数，`dst[127]`表示数值127的出现次数。
- **高位模式（`BIN1`）**：统计`src`中数值在[128, 255]范围内的出现次数，并累加进`dst`。`dst[0]`表示数值128的出现次数，`dst[127]`表示数值255的出现次数。

示例如下图所示：

![频率统计](../../figures/asc_frequency_histogram.png)

本接口仅在AIV上生效。

## 函数原型

### 低位模式

```cpp
__simd_callee__ inline void asc_frequency_histogram_bin0(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
```

### 高位模式

```cpp
__simd_callee__ inline void asc_frequency_histogram_bin1(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 目的操作数（矢量数据寄存器）。|
| src | 输入 | 源操作数（矢量数据寄存器）。|
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- `mask`需通过掩码设置接口预先赋值后再传入；未赋值的掩码寄存器内容不确定，会导致参与统计的元素位置错误。

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
constexpr uint32_t INPUT_COUNT = 256;
constexpr uint32_t BIN_COUNT = 256;
constexpr uint32_t INPUT_BYTES = INPUT_COUNT * sizeof(uint8_t);
constexpr uint32_t OUTPUT_BYTES = BIN_COUNT * sizeof(uint16_t);

__simd_vf__ inline void histogram_vf(__ubuf__ uint16_t* output, __ubuf__ uint8_t* input)
{
    vector_uint8_t src;
    vector_uint16_t dst0;
    vector_uint16_t dst1;
    vector_bool src_mask = asc_create_mask_b8(PAT_ALL);
    vector_bool dst_mask = asc_create_mask_b16(PAT_ALL);

    asc_duplicate_scalar(dst0, static_cast<uint16_t>(0));
    asc_duplicate_scalar(dst1, static_cast<uint16_t>(0));
    asc_loadalign(src, input);
    asc_frequency_histogram_bin0(dst0, src, src_mask);
    asc_frequency_histogram_bin1(dst1, src, src_mask);
    asc_storealign(output, dst0, dst_mask);
    asc_storealign(output + 128, dst1, dst_mask);
}

__global__ __vector__ void asc_frequency_histogram_kernel(__gm__ uint16_t* output,
                                                           __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[INPUT_COUNT];
    __ubuf__ uint16_t output_local[BIN_COUNT];
    asc_copy_gm2ub_align(input_local, input, INPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    histogram_vf(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_COUNT);
    std::vector<uint16_t> output(BIN_COUNT, 0);
    std::vector<uint16_t> expected(BIN_COUNT, 0);
    for (uint32_t i = 0; i < INPUT_COUNT; ++i) {
        input[i] = static_cast<uint8_t>((i * 37U + 11U) & 0xffU);
        ++expected[input[i]];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_frequency_histogram_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < BIN_COUNT; ++i) {
        if (output[i] != expected[i]) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_frequency_histogram completed."
                         : "[Failed] asc_frequency_histogram output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
