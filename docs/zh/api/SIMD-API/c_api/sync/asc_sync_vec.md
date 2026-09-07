# asc_sync_vec

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

**无入参函数原型`asc_sync_vec()`接口已废弃，请使用[asc_sync](asc_sync.md)执行针对所有流水的同步操作。**

针对`PIPE_V`执行同步操作。其作用是保证`PIPE_V`中前序指令（如矢量计算）全部完成后，`PIPE_MTE2`、`PIPE_MTE3`与`PIPE_S`的后续指令才能开始执行。

本接口为易用性接口，用于简化手工同步的编码。

## 函数原型

```c
// 有入参函数原型
__aicore__ inline void asc_sync_vec(int id)

// 无入参函数原型
__aicore__ inline void asc_sync_vec()
```

## 参数说明

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

本示例展示`asc_sync_vec`作为易用性接口的价值：`PIPE_V`上的矢量计算完成后，`PIPE_MTE2`上的搬入、`PIPE_MTE3`上的数据搬出以及`PIPE_S`上的标量处理都需要依赖该次计算。若手工同步，需分别对`PIPE_MTE2`、`PIPE_MTE3`与`PIPE_S`各执行一组`asc_sync_notify`与`asc_sync_wait`；而使用`asc_sync_vec`一次调用即可同时完成对`PIPE_MTE2`、`PIPE_MTE3`与`PIPE_S`的同步，减少手写代码量。示例中`PIPE_MTE2`到`PIPE_V`的依赖则使用`asc_sync_notify`与`asc_sync_wait`进行精准同步，以便与`asc_sync_vec`的易用性写法形成对比。

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

__global__ __vector__ void asc_sync_vec_kernel(__gm__ float* dst0, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS];
    asc_copy_gm2ub_align(x, src0, BYTES);
    asc_copy_gm2ub_align(y, src1, BYTES);

    // PIPE_MTE2到PIPE_V的同步，需分别编写notify与wait。
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    add_vf(z, x, y);

    // 一次asc_sync_vec同时完成对PIPE_MTE2、PIPE_MTE3与PIPE_S的同步，
    // 保证计算完成后，后续搬入、搬出与标量处理才执行。
    asc_sync_vec(0);

    asc_copy_ub2gm_align(dst0, z, BYTES);
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS), src1(ELEMENTS), dst0(ELEMENTS, 0.0f), golden0(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS - i) * 0.5f;
        golden0[i] = src0[i] + src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *dst0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, BYTES, src0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, BYTES, src1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_vec_kernel<<<1, 0>>>(dst0_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(dst0.data(), BYTES, dst0_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = dst0 == golden0;
    std::cout << (passed ? "[Success] asc_sync_vec passed." : "[Failed] asc_sync_vec failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(dst0_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
