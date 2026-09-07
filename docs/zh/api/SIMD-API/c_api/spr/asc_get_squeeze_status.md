# asc_get_squeeze_status

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

头文件路径为：`"c_api/spr/spr.h"`。

读取squeeze操作后保存至AR特殊寄存器的有效数据长度值，用于配合[asc_squeeze_and_storeunalign](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)接口获取已连续搬出的有效数据量。

## 函数原型

```cpp
__aicore__ inline int64_t asc_get_squeeze_status()
```

## 参数说明

无

## 返回值说明

返回int64_t类型的squeeze有效数据长度值。

## 流水类型

PIPE_S

## 约束说明

调用本接口前，需要先调用[asc_squeeze_and_storeunalign_init](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md)清空AR特殊寄存器，并调用[asc_squeeze_and_storeunalign](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)完成数据筛选。

## 调用示例

本示例模拟稀疏数据压缩场景：输入包含256个`uint16_t`数据，掩码选择每4个数据中的第1个，连续筛选并搬出64个有效数据。Kernel通过`asc_get_squeeze_status`获取已搬出的有效字节数，Host侧检查返回值是否为128字节。筛选流程完成后，需要调用[asc_squeeze_and_storeunalign_finalize](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md)搬出尾块。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行。产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t REPEAT_COUNT = 2;
constexpr uint32_t INPUT_COUNT = ELEMENTS_PER_REGISTER * REPEAT_COUNT;
constexpr uint32_t OUTPUT_COUNT = INPUT_COUNT / 4;
constexpr uint64_t EXPECTED_VALID_BYTES = OUTPUT_COUNT * sizeof(uint16_t);

/*
 * 本示例说明：
 * - PAT_M4掩码选择每4个uint16_t数据中的第1个。
 * - asc_squeeze_and_storeunalign将每轮筛选结果连续写入Unified Buffer。
 * - asc_get_squeeze_status读取AR特殊寄存器中累计的有效字节数。
 */
__simd_vf__ inline void pack_selected(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input)
{
    __ubuf__ uint16_t* dst = output;
    vector_uint16_t src;
    vector_uint16_t squeezed;
    vector_store_unalign unalign;
    vector_bool every_fourth = asc_create_mask_b16(PAT_M4);

    asc_squeeze_and_storeunalign_init();
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        asc_loadalign(src, input + repeat * ELEMENTS_PER_REGISTER);
        asc_squeeze_and_storeunalign(dst, squeezed, unalign, src, every_fourth);
    }
    asc_squeeze_and_storeunalign_finalize(dst, unalign);
}

__global__ __vector__ void asc_get_squeeze_status_kernel(
    __gm__ uint16_t* output, __gm__ uint16_t* input, __gm__ uint64_t* valid_bytes)
{
    asc_init();
    __ubuf__ uint16_t input_local[INPUT_COUNT];
    __ubuf__ uint16_t output_local[OUTPUT_COUNT];

    asc_copy_gm2ub_align(input_local, input, INPUT_COUNT * sizeof(uint16_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    pack_selected(output_local, input_local);
    valid_bytes[0] = static_cast<uint64_t>(asc_get_squeeze_status());
    asc_dcci_single(valid_bytes);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_COUNT * sizeof(uint16_t));
    asc_sync();
}

} // namespace

int main()
{
    std::vector<uint16_t> input(INPUT_COUNT), output(OUTPUT_COUNT, 0);
    for (uint32_t i = 0; i < INPUT_COUNT; ++i) {
        input[i] = static_cast<uint16_t>(i + 1);
    }
    uint64_t valid_bytes = 0;
    uint16_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    uint64_t* valid_bytes_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_COUNT * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_COUNT * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&valid_bytes_device), sizeof(uint64_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_COUNT * sizeof(uint16_t), input.data(),
        INPUT_COUNT * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_get_squeeze_status_kernel<<<1, 0>>>(output_device, input_device, valid_bytes_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(&valid_bytes, sizeof(uint64_t), valid_bytes_device, sizeof(uint64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Valid bytes after squeeze: " << valid_bytes << std::endl;
    const bool passed = valid_bytes == EXPECTED_VALID_BYTES;
    std::cout << (passed ? "[Success] asc_get_squeeze_status passed."
                          : "[Failed] asc_get_squeeze_status failed.") << std::endl;

    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtFree(valid_bytes_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
