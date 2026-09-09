# asc_set_gm2l1_loop_size

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

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

将数据从Global Memory（GM）搬运到L1 Buffer时，设置[asc_copy_gm2l1_align](asc_copy_gm2l1_align.md)循环填充模式的内层和外层循环次数。

需与[asc_set_gm2l1_loop1_stride](asc_set_gm2l1_loop1_stride.md)和[asc_set_gm2l1_loop2_stride](asc_set_gm2l1_loop2_stride.md)配合使用。

以源操作数搬运场景为例，如下图所示。

![源操作数搬运场景示例](../figures/source_operand_move_example.png)

本接口仅在AIC上生效。

## 函数原型

```cpp
__aicore__ inline void asc_set_gm2l1_loop_size(uint64_t loop1_size, uint64_t loop2_size)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| loop1_size | 输入 | 内层循环的循环次数，取值范围为[0, $2^{21}-1$]。 |
| loop2_size | 输入 | 外层循环的循环次数，取值范围为[0, $2^{23}-1$]。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口在非AIC上调用直接返回。
- 配置仅由[asc_copy_gm2l1_align](asc_copy_gm2l1_align.md)循环填充模式取用，循环填充模式不支持左右填充。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 128;
constexpr int64_t CROSS_CORE_FLAG_ID = 0x8;
constexpr uint64_t AIV_TO_AIC_SYNC_ID = 0x1;
constexpr uint64_t AIC_TO_AIV_SYNC_ID = 0x2;

__global__ __mix__(1, 2) void LoopKernel(  // __mix__：AIC搬运到L1，AIV将回读结果写回GM。
    __gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __cbuf__ uint8_t* l1 = (__cbuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* staging_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* result_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(OUTPUT_BYTES);
    if ASC_IS_AIV {
        const bool is_output_core = asc_get_sub_block_id() == 0;
        if (is_output_core) {
            asc_copy_gm2ub(staging_ub, input, INPUT_BYTES);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_intra_arrive(PIPE_MTE3, AIV_TO_AIC_SYNC_ID);
        }
        asc_sync_block_arrive(PIPE_MTE3, CROSS_CORE_FLAG_ID);
        if (is_output_core) {
            asc_sync_intra_wait(PIPE_MTE3, AIC_TO_AIV_SYNC_ID);
            asc_copy_ub2gm(output, result_ub, OUTPUT_BYTES);
        }
    }
    if ASC_IS_AIC {
        asc_sync_intra_wait(PIPE_MTE1, AIV_TO_AIC_SYNC_ID);
        asc_sync_block_wait(PIPE_MTE1, CROSS_CORE_FLAG_ID);
        // 2×2循环：源偏移为0、64、128、192B，目的L1偏移为0、32、64、96B。
        asc_set_gm2l1_loop_size(2, 2);
        asc_set_gm2l1_loop1_stride(64, 32);
        asc_set_gm2l1_loop2_stride(128, 64);
        asc_copy_gm2l1_align(l1, input, 1, 32, 0, 0, false, 4, 0, 32);
        // 避免循环配置影响下一次GM→L1搬运。
        asc_set_gm2l1_loop_size(1, 1);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12ub(result_ub, l1, 0, 1, OUTPUT_BYTES / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_intra_arrive(PIPE_MTE1, AIC_TO_AIV_SYNC_ID);
    }
    asc_sync_pipe(PIPE_ALL);
}

bool IsGolden(const std::vector<uint8_t>& output)
{
    for (uint32_t block = 0; block < 4; ++block) {
        const uint32_t source = (block / 2) * 128 + (block % 2) * 64;
        for (uint32_t i = 0; i < 32; ++i) {
            if (output[block * 32 + i] != static_cast<uint8_t>(source + i)) return false;
        }
    }
    return true;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE) != ACL_SUCCESS) return 1;
    LoopKernel<<<1, 0>>>(output_device, input_device);
    const bool passed = aclrtSynchronizeDevice() == ACL_SUCCESS &&
        aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS && IsGolden(output);
    std::cout << "RESULT loop_2x2=" << (passed ? "PASS" : "FAIL") << std::endl;
    aclrtFree(output_device); aclrtFree(input_device); aclrtResetDevice(0); aclFinalize();
    return passed ? 0 : 1;
}
```
