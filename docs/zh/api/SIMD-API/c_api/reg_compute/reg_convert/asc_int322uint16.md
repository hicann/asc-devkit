# asc_int322uint16

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

头文件路径为：`"c_api/reg_compute/reg_convert.h"`。

根据`mask`将`src`中的每个`int32_t`类型元素转换为`uint16_t`类型，并支持不同的饱和模式，结果写入`dst`。由于源操作数与目的操作数类型位宽比为2:1，写入数据时需要将一个`VL`大小的数据分为两部分，根据位置参数选择数据写入索引为奇数的位置或偶数的位置。参考伪代码：

```python
# 位置判断仅用于说明接口语义，实际位置参数为编译期常量。
# VL表示矢量数据寄存器位宽，取值256字节。
def asc_int322uint16(dst, src, mask, dst_pos):
    offset = 0 if dst_pos == ASC_POSITION_EVEN else 1
    for i in range(VL // 4):
        dst[2 * i] = 0
        dst[2 * i + 1] = 0
        if mask[i]:
            dst[2 * i + offset] = uint16(src[i])  # int32 -> uint16
```

关于舍入模式和饱和/非饱和模式的详细说明，请参见[舍入模式与饱和模式](rounding_mode.md)。

## 函数原型

```c
// 占位符形式
__simd_callee__ inline void asc_int322uint16<sat_mode>(vector_uint16_t& dst,
                                                       vector_int32_t src,
                                                       vector_bool mask,
                                                       std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)

// 占位符形式
__simd_callee__ inline void asc_int322uint16<sat_mode>(vector_uint16_t& dst,
                                                       vector_int32_t src,
                                                       vector_bool mask,
                                                       std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
```

`<sat_mode>`表示饱和非饱和模式，为空时表示非饱和模式，取值为`_sat`时表示饱和模式。

### 函数原型典型示例

```c
// 非饱和模式，数据写入索引为偶数的位置
__simd_callee__ inline void asc_int322uint16(vector_uint16_t& dst,
                                             vector_int32_t src,
                                             vector_bool mask,
                                             std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述                                                                                                               |
| ------ | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst    | 输出      | 目的操作数（矢量数据寄存器）。                                                                                     |
| src    | 输入      | 源操作数（矢量数据寄存器）。                                                                                       |
| mask   | 输入      | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| dst_pos | 输入 | 目的操作数位置。`ASC_POSITION_EVEN`表示偶数索引，`ASC_POSITION_ODD`表示奇数索引。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- `src`与`dst`的数据类型需要与函数原型匹配。
- `mask`掩码位为0时，`dst`对应元素置0。

## 调用示例


<!-- npu="950" id8 -->

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```c
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

constexpr uint32_t BUFFER_BYTES = 256;
__simd_vf__ inline void convert(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input)
{
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    vector_uint16_t dst_0;
    vector_int32_t src_0;
    asc_loadalign(dst_0, reinterpret_cast<__ubuf__ uint16_t*>(output));
    asc_loadalign(src_0, reinterpret_cast<__ubuf__ int32_t*>(input));
    asc_int322uint16_sat(dst_0, src_0, mask, ASC_POSITION_EVEN);
    asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output), dst_0, mask);
    vector_uint16_t dst_1;
    vector_int32_t src_1;
    asc_loadalign(dst_1, reinterpret_cast<__ubuf__ uint16_t*>(output));
    asc_loadalign(src_1, reinterpret_cast<__ubuf__ int32_t*>(input));
    asc_int322uint16(dst_1, src_1, mask, ASC_POSITION_ODD);
    asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output), dst_1, mask);
    vector_uint16_t dst_2;
    vector_int32_t src_2;
    asc_loadalign(dst_2, reinterpret_cast<__ubuf__ uint16_t*>(output));
    asc_loadalign(src_2, reinterpret_cast<__ubuf__ int32_t*>(input));
    asc_int322uint16_sat(dst_2, src_2, mask, ASC_POSITION_ODD);
    asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output), dst_2, mask);
    vector_uint16_t dst_3;
    vector_int32_t src_3;
    asc_loadalign(dst_3, reinterpret_cast<__ubuf__ uint16_t*>(output));
    asc_loadalign(src_3, reinterpret_cast<__ubuf__ int32_t*>(input));
    asc_int322uint16(dst_3, src_3, mask, ASC_POSITION_EVEN);
    asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output), dst_3, mask);
}
__global__ __vector__ void asc_int322uint16_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES];
    __ubuf__ uint8_t input_local[BUFFER_BYTES];
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    convert(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace
int main()
{
    constexpr uint8_t input_pattern[] = {0x01, 0x00, 0x00, 0x00};
    constexpr uint8_t golden_pattern[] = {0x01, 0x00, 0x00, 0x00};
    std::vector<uint8_t> input(BUFFER_BYTES);
    std::vector<uint8_t> output(BUFFER_BYTES, 0xff);
    std::vector<uint8_t> golden(BUFFER_BYTES);
    for (size_t i = 0; i < BUFFER_BYTES; ++i) {
        input[i] = input_pattern[i % sizeof(input_pattern)];
        golden[i] = golden_pattern[i % sizeof(golden_pattern)];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_int322uint16_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    std::cout << "Conversion: vector_int32_t -> vector_uint16_t" << std::endl;
    print_data("Input 1.0 (raw bytes)", input);
    print_data("Output (raw bytes)", output);
    print_data("Golden 1.0 (raw bytes)", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_int322uint16/asc_int322uint16_sat/"
                           "ASC_POSITION_ODD/ASC_POSITION_EVEN passed."
                         : "[Failed] asc_int322uint16/asc_int322uint16_sat/"
                           "ASC_POSITION_ODD/ASC_POSITION_EVEN failed.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```

<!-- end id8 -->
