# asc_storealign_squeeze_status

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

将[asc_squeeze_and_storeunalign](../reg_permute_sel/asc_squeeze_and_storeunalign.md)操作后保存在AR特殊寄存器中的有效数据长度写入Unified Buffer（UB）。调用无`offset`参数的重载时，写入`dst`；调用带`offset`参数的重载时，写入`dst`加字节偏移`offset`的位置。

本接口仅在AIV上生效。

## 函数原型

```cpp
__simd_callee__ inline void asc_storealign_squeeze_status(__ubuf__ uint32_t* dst)

__simd_callee__ inline void asc_storealign_squeeze_status(__ubuf__ uint32_t* dst,
                                                          int32_t offset)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数在UB上的基地址，类型为`__ubuf__ uint32_t*`。无`offset`参数时，实际写入地址为`dst`；带`offset`参数时，实际写入地址为`dst`加`offset`字节。 |
| offset | 输入 | 仅带`offset`参数的重载使用。相对`dst`基地址的字节偏移量，类型为`int32_t`；无`offset`参数时，默认值为`0`。 |

## 返回值说明

无

## 约束说明

- 非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- 实际目的地址需4字节对齐，且不得超出实际可用UB范围；无`offset`参数时实际目的地址为`dst`，带`offset`参数时为`dst`加`offset`字节。
- 调用本接口前，需使用[asc_squeeze_and_storeunalign](../reg_permute_sel/asc_squeeze_and_storeunalign.md)完成squeeze操作；写入的值是其当前累计的有效数据字节数。
- 如果本接口与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化。

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
#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t ELEMENT_COUNT = 64;
constexpr uint32_t STATUS_WORD_COUNT = 128;
constexpr uint32_t SQUEEZE_STATUS_BYTES = ELEMENT_COUNT * sizeof(float);

__simd_vf__ inline void store_squeeze_status(
    __ubuf__ uint32_t* status, __ubuf__ float* packed, __ubuf__ float* input)
{
    vector_float src;
    vector_float squeezed;
    vector_store_unalign unalign_reg;
    vector_bool full_mask = asc_create_mask_b32(PAT_ALL);

    asc_loadalign(src, input); // 将输入数据加载到矢量数据寄存器。
    asc_squeeze_and_storeunalign_init();
    asc_squeeze_and_storeunalign(packed, squeezed, unalign_reg, src, full_mask);
    asc_squeeze_and_storeunalign_finalize(packed, unalign_reg);

    // 不传offset时，默认将AR中的有效数据长度写入status起始地址。
    asc_storealign_squeeze_status(status);
}

__global__ __vector__ void store_squeeze_status_kernel(__gm__ uint32_t* output, __gm__ float* input)
{
    asc_init();
    __ubuf__ float input_local[ELEMENT_COUNT];
    __ubuf__ float packed_local[ELEMENT_COUNT];
    __ubuf__ uint32_t status_local[STATUS_WORD_COUNT];
    asc_copy_gm2ub_align(input_local, input, sizeof(input_local));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    store_squeeze_status(status_local, packed_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, status_local, sizeof(status_local));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(ELEMENT_COUNT);
    std::vector<uint32_t> output(STATUS_WORD_COUNT, 0U);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        input[i] = static_cast<float>(i);
    }

    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS) {
        return 1;
    }
    float* input_device = nullptr;
    uint32_t* output_device = nullptr;
    bool success =
        aclrtMalloc(reinterpret_cast<void**>(&input_device), input.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST) ==
            ACL_SUCCESS &&
        aclrtMalloc(
            reinterpret_cast<void**>(&output_device), output.size() * sizeof(uint32_t), ACL_MEM_MALLOC_HUGE_FIRST) ==
            ACL_SUCCESS;
    if (success) {
        success = aclrtMemcpy(
                      input_device, input.size() * sizeof(float), input.data(), input.size() * sizeof(float),
                      ACL_MEMCPY_HOST_TO_DEVICE) == ACL_SUCCESS;
    }
    if (success) {
        store_squeeze_status_kernel<<<1, 0>>>(output_device, input_device);
        success = aclrtSynchronizeDevice() == ACL_SUCCESS;
    }
    if (success) {
        success = aclrtMemcpy(
                      output.data(), output.size() * sizeof(uint32_t), output_device, output.size() * sizeof(uint32_t),
                      ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    }
    if (input_device != nullptr) {
        aclrtFree(input_device);
    }
    if (output_device != nullptr) {
        aclrtFree(output_device);
    }
    aclrtResetDevice(0);
    aclFinalize();
    if (!success) {
        return 1;
    }
    success = success && output[0] == SQUEEZE_STATUS_BYTES;
    std::cout << (success ? "PASS" : "FAIL") << '\n';
    return success ? 0 : 1;
}
```
