# asc_set_ub2gm_loop1_stride

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

本接口用于设置[asc_copy_ub2gm_align](./asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)使用循环搬运模式时，内层循环中相邻两次迭代源操作数数据块起始地址之间的偏移量，以及目的操作数数据块起始地址之间的偏移量。

使用循环搬运模式时，还需通过[asc_set_ub2gm_loop_size](./asc_set_ub2gm_loop_size.md)设置循环次数，并通过[asc_set_ub2gm_loop2_stride](./asc_set_ub2gm_loop2_stride.md)设置外层循环`loop2`的步长。

本接口仅在AIV上执行有效，在AIC上调用为空操作。

以源操作数搬运场景为例，如下图所示。

![源操作数搬运场景示例](../figures/source_operand_move_example.png)

## 函数原型

```c
__aicore__ inline void asc_set_ub2gm_loop1_stride(uint64_t loop1_src_stride,
                                                  uint64_t loop1_dst_stride)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| loop1_src_stride | 输入 | 内层循环中，相邻两次迭代源操作数数据块起始地址之间的偏移量，单位为字节。取值范围为[0, $2^{21}−1$]，且必须32字节对齐。 |
| loop1_dst_stride | 输入 | 内层循环中，相邻两次迭代目的操作数数据块起始地址之间的偏移量，单位为字节。取值范围为[0, $2^{40}−1$]。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 调用`asc_copy_ub2gm_align`前，必须通过本接口、`asc_set_ub2gm_loop_size`和`asc_set_ub2gm_loop2_stride`完成循环次数及两层循环步长的配置。
- `loop1`步长配置会持续生效，直至重新配置。若后续搬运任务使用不同的`loop1`步长，应在调用`asc_copy_ub2gm_align`前调用本接口重新配置。
- `loop1_src_stride`必须32字节对齐。
- 完成当前循环搬运任务后，需调用`asc_set_ub2gm_loop_size(1, 1)`将两层循环次数复位为1，否则可能影响下一次搬运。

## 调用示例

将以下代码保存为`example.asc`后，执行对应的编译运行命令。

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

constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 128;

__global__ __vector__ void asc_set_ub2gm_loop1_stride_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[INPUT_BYTES];
    asc_copy_gm2ub_align(local, input, INPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_set_ub2gm_loop_size(2, 2);
    asc_set_ub2gm_loop1_stride(64, 32);
    asc_set_ub2gm_loop2_stride(128, 64);
    asc_copy_ub2gm_align(output, local, 1, 32, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 32, 32);
    asc_set_ub2gm_loop_size(1, 1);
    asc_sync_notify(PIPE_MTE3, PIPE_S, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_S, EVENT_ID0);
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
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i + 1);
    for (uint32_t block = 0; block < 4; ++block) {
        const uint32_t source = (block / 2) * 128 + (block % 2) * 64;
        for (uint32_t i = 0; i < 32; ++i) golden[block * 32 + i] = input[source + i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_ub2gm_loop1_stride_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_ub2gm_loop1_stride passed." : "[Failed] asc_set_ub2gm_loop1_stride failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
