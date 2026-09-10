# asc_update_addr_reg

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

头文件路径为：`"c_api/reg_compute/compute/reg_addr_reg.h"`。

更新地址寄存器（[addr_reg](../../defs/type/data_type_definition.md#地址寄存器)），用于迭代寻址。支持1~4维偏移量设置，位宽支持b8、b16、b32三种模式：

- b8模式：每次迭代地址偏移量以1字节为单位，适用于int8_t、uint8_t等类型的操作。
- b16模式：每次迭代地址偏移量以2字节为单位，适用于half、int16_t等类型的操作。
- b32模式：每次迭代地址偏移量以4字节为单位，适用于float、int32_t等类型的操作。

每次迭代时的地址偏移量计算公式为：offset = index0×stride0 + index1×stride1 + index2×stride2 + index3×stride3，其中index_i为各维度的循环迭代变量，stride_i为各维度的步长。例如在4维场景（N, C, H, W）中，offset0对应W维（最内层），offset1对应H×W，offset2对应C×H×W，offset3对应N×C×H×W。addr_reg更新后配合asc_loadalign/asc_storealign等接口使用，在循环中按设定的偏移量自动递增Unified Buffer（UB）地址。

> [!NOTE]说明
>旧接口 `asc_create_iter_reg_*` 已废弃，请使用 `asc_update_addr_reg_*` 替代。旧接口返回类型 `iter_reg` 也已更名为 `addr_reg`。

## 函数原型

- 1维偏移

    ```c
    __simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset)
    ```

- 2维偏移

    ```c
    __simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset0,
                                                        uint32_t offset1)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset0,
                                                            uint32_t offset1)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset0,
                                                            uint32_t offset1)
    ```

- 3维偏移

    ```c
    __simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset0,
                                                        uint32_t offset1,
                                                        uint32_t offset2)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset0,
                                                            uint32_t offset1,
                                                            uint32_t offset2)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset0,
                                                            uint32_t offset1,
                                                            uint32_t offset2)
    ```

- 4维偏移

    ```c
    __simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset0,
                                                        uint32_t offset1,
                                                        uint32_t offset2,
                                                        uint32_t offset3)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset0,
                                                            uint32_t offset1,
                                                            uint32_t offset2,
                                                            uint32_t offset3)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset0,
                                                            uint32_t offset1,
                                                            uint32_t offset2,
                                                            uint32_t offset3)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| offset | 输入 | 第1维的偏移量（最内层），单位为元素个数。<br>数据类型为uint32_t。<br>例如b32模式下offset=64表示64个float元素（一个VL即256B）。 |
| offset0 | 输入 | 第1维的偏移量（最内层，如W维），单位为元素个数。<br>数据类型为uint32_t。 |
| offset1 | 输入 | 第2维的偏移量（如H×W维的总步长），单位为元素个数。<br>数据类型为uint32_t。 |
| offset2 | 输入 | 第3维的偏移量（如C×H×W维的总步长），单位为元素个数。<br>数据类型为uint32_t。 |
| offset3 | 输入 | 第4维的偏移量（最外层，如N×C×H×W维的总步长），单位为元素个数。<br>数据类型为uint32_t。 |

## 返回值说明

返回值为addr_reg，地址寄存器。

## 约束说明

- 请勿在循环内使用if/else。
- addr_reg必须在最内层循环体内赋值使用。
- 不同步长的操作数，建议使用多个addr_reg，避免地址被覆盖。
- addr_reg的数量上限为8，超过8个可能会导致性能劣化。编译器会自动复用生命周期已经结束的addr_reg寄存器。
- 由于硬件（HardwareLoop）限制，addr_reg最多支持4层循环。

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

constexpr uint16_t ONE_REPEAT_SIZE = 256;
constexpr uint16_t REPEAT_COUNT = 4;
constexpr uint32_t TOTAL_BYTES = ONE_REPEAT_SIZE * REPEAT_COUNT;

__simd_vf__ inline void exercise(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input,
                                uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    vector_uint8_t value;
    addr_reg offset_reg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        offset_reg = asc_update_addr_reg_b8(oneRepeatSize);
        asc_loadalign(value, input, offset_reg);
        asc_storealign(output, value, offset_reg, mask);
    }
}

__global__ __vector__ void asc_update_addr_reg_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input,
                                                      uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    asc_init();
    __ubuf__ uint8_t output_local[TOTAL_BYTES];
    __ubuf__ uint8_t input_local[TOTAL_BYTES];
    asc_copy_gm2ub_align(input_local, input, TOTAL_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    exercise(output_local, input_local, oneRepeatSize, repeatTimes);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, TOTAL_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(TOTAL_BYTES), output(TOTAL_BYTES, 0xff), golden(TOTAL_BYTES);
    for (uint32_t i = 0; i < TOTAL_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    golden = input;
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), TOTAL_BYTES * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), TOTAL_BYTES * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_update_addr_reg_kernel<<<1, 0>>>(output_device, input_device, ONE_REPEAT_SIZE, REPEAT_COUNT);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_update_addr_reg_b8 passed." : "[Failed] asc_update_addr_reg_b8 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
