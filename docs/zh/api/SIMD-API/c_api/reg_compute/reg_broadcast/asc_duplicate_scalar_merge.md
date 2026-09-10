# asc_duplicate_scalar_merge

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

头文件路径为：`"c_api/reg_compute/compute/reg_broadcast.h"`。

将标量`value`广播到目的操作数`dst`中被`mask`筛选的位置，`dst`中未被`mask`筛选的元素保留原值。

本接口与[asc_duplicate_scalar](asc_duplicate_scalar.md)的广播数据来源相同，区别是本接口采用合并模式保留`dst`中未参与计算的元素。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```cpp
// 占位符形式
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_<dtype>& dst,
                                                       <dtype> value,
                                                       vector_bool mask)
```

**占位符说明如下：**

- `<dtype>`取值为：`int8_t`、`uint8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```cpp
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_float& dst,
                                                       float value,
                                                       vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 目的操作数（矢量数据寄存器）。被`mask`筛选的元素写入`value`，未被筛选的元素保留调用接口前的值。元素数据类型需要与`value`保持一致。 |
| value | 输入 | 源操作数（标量），作为待广播的数据。数据类型需要与`dst`的元素数据类型保持一致。 |
| mask | 输入 | 目的操作数元素操作的有效指示（掩码寄存器）。对应比特位为1时，`dst`中的元素写入`value`；为0时，`dst`中的元素保留原值。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 本接口只能在使用`__simd_vf__`标记的Vector Function内调用，不支持在`__aicore__`函数中直接调用，仅在AIV上生效，在AIC上调用将直接返回。
- 使用`mask`前，需要通过掩码设置或搬入接口完成初始化；未初始化的掩码寄存器内容不确定。
- 调用接口前需要初始化`dst`，否则未被`mask`筛选的元素值不确定。
- 同一寄存器的数据依赖由硬件保序，无需额外插入同步指令。本接口与前后Reg数据搬运接口之间，如果不同寄存器访问同一UB地址且存在写后读或写后写依赖，需要调用[asc_mem_bar](../reg_sync/asc_mem_bar.md)进行同步。

## 调用示例

将以下代码保存为`example.asc`后，可通过`bisheng`命令编译运行。其中，`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

以下示例将标量`1.0f`广播到`dst`的前4个元素，未被`mask`筛选的其余元素保留原值。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENT_COUNT = 64;
constexpr uint32_t ACTIVE_COUNT = 4;
constexpr float INITIAL_VALUE = -1.0f;
constexpr float FILL_VALUE = 1.0f;

__simd_vf__ inline void duplicate_scalar_merge(__ubuf__ float* dst)
{
    vector_float dst_reg;
    uint32_t active_count = ACTIVE_COUNT;
    uint32_t full_count = ELEMENT_COUNT;
    vector_bool active_mask = asc_update_mask_b32(active_count);
    vector_bool full_mask = asc_update_mask_b32(full_count);

    asc_loadalign(dst_reg, dst);
    asc_duplicate_scalar_merge(dst_reg, FILL_VALUE, active_mask);
    asc_storealign(dst, dst_reg, full_mask);
}

__global__ __vector__ void asc_duplicate_scalar_merge_kernel(__gm__ float* dst)
{
    asc_init();
    __ubuf__ float dst_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(dst_local, dst, sizeof(dst_local));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    duplicate_scalar_merge(dst_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, sizeof(dst_local));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> output(ELEMENT_COUNT, INITIAL_VALUE);
    std::vector<float> golden(ELEMENT_COUNT, INITIAL_VALUE);
    for (uint32_t i = 0; i < ACTIVE_COUNT; ++i) golden[i] = FILL_VALUE;

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(dst_device, output.size() * sizeof(float), output.data(), output.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_duplicate_scalar_merge_kernel<<<1, 0>>>(dst_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_duplicate_scalar_merge passed."
                        : "[Failed] asc_duplicate_scalar_merge failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
