# asc_ndim_copy_dci

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

头文件路径为：`"c_api/vector_datamove/vector_datamove.h"`。

刷新NDDMA DataCache。在多核场景下，当多个AI Core读取同一块GM（Global Memory）地址时，NDDMA DataCache中可能缓存旧数据，导致部分核读取到错误的数据。因此，在使用[asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md)前，需要调用本接口刷新NDDMA DataCache，确保缓存中的数据为最新状态。

本接口仅在AIV上生效。

## 函数原型

```cpp
__aicore__ inline void asc_ndim_copy_dci()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- 本接口在非AIV上调用直接返回。
- 本接口仅在使用[asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md)前调用，用于刷新NDDMA DataCache。
- 多核场景下，如果多个AI Core可能读取同一块GM地址，且该GM地址中的数据可能被其他核更新，应在调用[asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md)前调用本接口，保证多核间数据一致性。

## 调用示例

2D Padding场景，使用固定值填充方式
![ ](../figures/asc_ndim_copy_gm2ub_2d_padding_constant.png)

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t INPUT_BYTES = 12;
constexpr uint32_t OUTPUT_COLUMNS = 6;
constexpr uint32_t OUTPUT_BYTES = OUTPUT_COLUMNS * 9;
constexpr uint8_t PAD_VALUE = 88;

__global__ __vector__ void asc_ndim_copy_dci_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[64];
    asc_set_ndim_loop0_stride(1, 1);
    asc_set_ndim_loop1_stride(6, 3);
    asc_set_ndim_loop2_stride(0, 0);
    asc_set_ndim_loop3_stride(0, 0);
    asc_set_ndim_loop4_stride(0, 0);
    asc_set_ndim_pad_value(PAD_VALUE);
    asc_ndim_pad_count_config config;
    config.loop1_lp_count = 2;
    config.loop1_rp_count = 3;
    asc_set_ndim_pad_count(config);
    asc_ndim_copy_gm2ub(local, input, 3, 4, 1, 1, 1, 2, 1, true,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    asc_sync_mte2(0);
    asc_copy_ub2gm_align(output, local, OUTPUT_BYTES);
    asc_sync_mte3(0);
}

void print_data(const char* name, const std::vector<uint8_t>& data)
{
    std::cout << name << ":";
    const uint32_t count = data.size() < 32 ? data.size() : 32;
    for (uint32_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, PAD_VALUE);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i);
    for (uint32_t row = 0; row < 4; ++row) {
        for (uint32_t column = 0; column < 3; ++column) {
            golden[(row + 2) * OUTPUT_COLUMNS + column + 2] = input[row * 3 + column];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_ndim_copy_dci_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_ndim_copy_dci passed." : "[Failed] asc_ndim_copy_dci failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
