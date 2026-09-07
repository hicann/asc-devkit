# asc_sync_mte3

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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

头文件路径为：`"c_api/sync/sync.h"`。

针对`PIPE_MTE3`执行同步操作。其作用是保证`PIPE_MTE3`中前序指令（如UB到GM的数据搬运）全部完成后，`PIPE_MTE2`与`PIPE_V`的后续指令才能开始执行。

本接口为易用性接口，用于简化手工同步的编码。

## 函数原型

```c
__aicore__ inline void asc_sync_mte3(int id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| id | 输入 | 核内多流水同步的同步ID，取值范围为[0, 7]，其含义与[asc_sync_notify](asc_sync_notify.md)中`id`参数的`event_t`枚举值相同，各枚举取值请参考[event\_t枚举定义](asc_sync_notify.md#event_t_enum)。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 同步ID取值范围为[0, 7]。
- 本接口只能在AIV中调用。

## 调用示例

本示例展示`asc_sync_mte3`作为易用性接口的价值：每次迭代中，UB->GM搬出计算结果`dst0`与直接搬入的数据`dst1`后，下一轮迭代的`PIPE_MTE2`搬入（重新占用缓冲区）与`PIPE_V`计算都需要依赖该次搬出。对比代码中`PIPE_MTE2`到`PIPE_V`的同步：若手工同步，需对每条流水分别编写`asc_sync_notify`与`asc_sync_wait`；而使用`asc_sync_mte3`一次调用即可同时完成对`PIPE_MTE2`与`PIPE_V`的同步，减少手写代码量。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(float);
constexpr uint32_t ITERATIONS = 4;

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENTS;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_sync_mte3_kernel(__gm__ float* dst0, __gm__ float* dst1, __gm__ float* src0, __gm__ float* src1, __gm__ float* src2)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS], w[ELEMENTS];
    for (uint32_t iter = 0; iter < ITERATIONS; ++iter) {
        asc_copy_gm2ub_align(x, src0 + iter * ELEMENTS, BYTES);
        asc_copy_gm2ub_align(y, src1 + iter * ELEMENTS, BYTES);
        asc_copy_gm2ub_align(w, src2 + iter * ELEMENTS, BYTES);

        // PIPE_MTE2到PIPE_V的同步，需分别分别编写notify与wait。
        asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

        add_vf(z, x, y);

        // PIPE_MTE2到PIPE_MTE3的同步，保证w搬入完成后才搬出。
        asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_copy_ub2gm_align(dst1 + iter * ELEMENTS, w, BYTES);

        // PIPE_V到PIPE_MTE3的同步，保证z计算完成后才搬出。
        asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_copy_ub2gm_align(dst0 + iter * ELEMENTS, z, BYTES);

        // 一次asc_sync_mte3同时完成对PIPE_MTE2与PIPE_V的同步，
        // 保证本轮搬出完成后才进入下一轮迭代的搬入与计算。
        // 最后一次迭代之后不再进入下一轮，无需同步。
        if (iter != ITERATIONS - 1) {
            asc_sync_mte3(0);
        }
    }
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS * ITERATIONS), src1(ELEMENTS * ITERATIONS), src2(ELEMENTS * ITERATIONS), dst0(ELEMENTS * ITERATIONS, 0.0f), dst1(ELEMENTS * ITERATIONS, 0.0f), golden0(ELEMENTS * ITERATIONS), golden1(ELEMENTS * ITERATIONS);
    for (uint32_t i = 0; i < ELEMENTS * ITERATIONS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS * ITERATIONS - i) * 0.5f;
        src2[i] = static_cast<float>(i) + 1.0f;
        golden0[i] = src0[i] + src1[i];
        golden1[i] = src2[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *src2_device = nullptr, *dst0_device = nullptr, *dst1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src2_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst1_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, ELEMENTS * ITERATIONS * sizeof(float), src0.data(), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, ELEMENTS * ITERATIONS * sizeof(float), src1.data(), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src2_device, ELEMENTS * ITERATIONS * sizeof(float), src2.data(), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_mte3_kernel<<<1, 0>>>(dst0_device, dst1_device, src0_device, src1_device, src2_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(dst0.data(), ELEMENTS * ITERATIONS * sizeof(float), dst0_device, ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(dst1.data(), ELEMENTS * ITERATIONS * sizeof(float), dst1_device, ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = dst0 == golden0 && dst1 == golden1;
    std::cout << (passed ? "[Success] asc_sync_mte3 passed." : "[Failed] asc_sync_mte3 failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(src2_device);
    aclrtFree(dst0_device);
    aclrtFree(dst1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
