---
okftype: reference.api
description: "压缩搬出矢量或掩码寄存器数据到UB并在写入后自动累加目的地址指针，矢量场景按mask压缩、掩码场景每间隔1bit丢弃1bit。"
tag:
  - roles: [data_movement]
  - operations: [compression, store]
  - traits: [post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "正文据此说明矢量数据寄存器/掩码寄存器"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "正文据此引用 Reg 矢量计算通用约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "与其他指令存在数据依赖时插入同步指令串行化"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "目的指针改为引用传递：CANN 9.1.0 Release Notes 记载 asc_storealign_pack_postupdate 中需要自动更新的 UB 目的指针参数修正为指针引用，确保调用后地址变化对调用方可见。"
  - since: "9.1.0"
    kind: overload_added
    scope: "section:函数原型"
    scope_basis: resolved
    note: "新增 vector_bool 输出重载：CANN 9.1.0 Release Notes 明确列出 asc_storealign_pack_postupdate 新增 vector_bool 输出重载，对应正文掩码寄存器搬出模式。"
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "UB 目的地址改为引用传递：CANN 9.2.0 Release Notes 再次记载 asc_storealign_pack_postupdate 的 UB 目的地址改为引用传递，确保更新结果可回写调用方。"
---
# asc_storealign_pack_postupdate

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/store/storealign.h"`。

将矢量数据寄存器或掩码寄存器中的数据压缩搬出到Unified Buffer（UB），写入完成后启用Post Update模式自动累加目的地址指针，便于硬件循环内连续多次调用时无需手动维护目的地址。当源操作数为矢量数据寄存器时，`mask`用于指示参与搬出的元素，`mask`对应位置为1时，将有效元素的低半部分bit数据写入压缩后对应的目的位置。`mask`对应位置为0时，压缩后对应的目的位置保持原值。当源操作数为掩码寄存器时，每间隔1 bit丢弃1 bit，将保留的bit连续压缩存储到目的地址，掩码寄存器搬出场景不支持配置掩码。目的操作数为UB地址。

本接口提供以下两种功能模式：

- **矢量数据寄存器搬出模式**：`offset`单位为元素。搬出完成后，目的地址累加`offset × sizeof(dtype)`字节。
- **掩码寄存器搬出模式**：`offset`单位为字节。搬出完成后，目的地址累加`offset`字节。

以**b32位宽压缩至b16位宽搬出**过程为例，示意图如下：

**图1** 压缩搬出数据

![](../../../../figures/reg_store_pack.png)

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

### 矢量数据寄存器搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ <dtype>*& dst,
                                                           vector_<dtype> src,
                                                           int32_t offset,
                                                           vector_bool mask)
```

#### dtype支持数据类型

`dtype`取值为：`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t`。对应矢量寄存器类型为`vector_<dtype>`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint32_t*& dst,
                                                           vector_uint32_t src,
                                                           int32_t offset,
                                                           vector_bool mask)
```

### 掩码寄存器搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ <dtype>*& dst,
                                                           vector_bool src,
                                                           int32_t offset)
```

#### dtype支持数据类型

`dtype`取值为：`uint8_t`、`uint16_t`、`uint32_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint8_t*& dst,
                                                           vector_bool src,
                                                           int32_t offset)
```

## 参数说明

### 矢量数据寄存器搬出模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输入/输出 | 目的操作数的起始地址，按指针引用传入。类型为`__ubuf__ <dtype>*&`，起始地址需32字节对齐。搬出完成后，该指针由硬件自动更新。 |
| src | 输入 | 源操作数（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| offset | 输入 | 地址偏移量，类型为`int32_t`，单位为元素。接口执行后，目的地址累加`offset × sizeof(dtype)`字节。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与搬出的元素。对应位置为1时参与搬出，为0时不参与搬出。 |

### 掩码寄存器搬出模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输入/输出 | 目的操作数的起始地址，按指针引用传入。类型为`__ubuf__ <dtype>*&`，起始地址需16字节对齐。搬出完成后，该指针由硬件自动更新。 |
| src | 输入 | 源操作数（掩码寄存器）。 |
| offset | 输入 | 地址偏移量，类型为`int32_t`，单位为字节。接口执行后，`dst`指针的底层UB地址更新`offset`字节。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 源操作数为矢量数据寄存器或掩码寄存器，目的操作数为UB地址。UB地址空间外的指针不可作为目的地址传入。
- UB总容量为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈和2KB Ascend C预留空间，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作为Data Cache，可用容量进一步减少）。目的操作数的写出范围和Post Update后的地址不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在数据依赖，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 矢量数据寄存器压缩搬出场景

- `dst`起始地址需32字节对齐。
- 未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- `mask`比特位为1的源操作数元素参与搬出，并将低半部分bit数据写入压缩后对应的目的位置；为0的元素不参与搬出，压缩后对应的目的位置保持原值。
- `offset`的单位为元素。Post Update后的`dst`新地址仍需32字节对齐，且本次写出范围和更新后的地址均需落在UB可用容量范围内。

### 掩码寄存器压缩搬出场景

- `dst`起始地址需16字节对齐。
- `offset`的单位为字节。Post Update后的`dst`新地址仍需16字节对齐，且本次写出范围和更新后的地址均需落在UB可用容量范围内。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;


constexpr uint32_t MASK_BYTES = 32;
constexpr uint32_t PACKED_BYTES = 16;
// 每个分区的有效打包字节数，可调整为其他长度；19用于覆盖非整块尾部。
constexpr uint32_t TOTAL_LENGTH = 19;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
// 掩码压缩每次固定写出16字节，向上取整分配完整写出块。
constexpr uint32_t REPEAT_TIME = AscendC::Std::ceil_div(TOTAL_LENGTH, PACKED_BYTES);
// 每次写出16字节，保留16字节哨兵区；所有起始地址均为16字节对齐。
constexpr uint32_t STEP_BYTES = 32;
// 额外预留一个步长，使最后一次Post Update后的地址仍处于本分区内。
constexpr uint32_t SECTION_BYTES = (REPEAT_TIME + 1) * STEP_BYTES;
constexpr uint32_t OUTPUT_BYTES = 3 * SECTION_BYTES;
constexpr uint8_t SENTINEL = 0xa5;

// 此掩码寄存器重载不支持尾块掩码：末轮仍写满16字节，尾部填充在预留UB空间内校验。
__simd_vf__ inline void pack_postupdate_vf(__ubuf__ uint8_t* dst,
                                           __ubuf__ uint32_t* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_bool src_reg;
    asc_loadalign(src_reg, src);

    __ubuf__ uint8_t* dst8 = dst;
    __ubuf__ uint16_t* dst16 = reinterpret_cast<__ubuf__ uint16_t*>(dst + SECTION_BYTES);
    __ubuf__ uint32_t* dst32 = reinterpret_cast<__ubuf__ uint32_t*>(dst + 2 * SECTION_BYTES);
    const int32_t step_bytes = static_cast<int32_t>(one_repeat_size + PACKED_BYTES);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_storealign_pack_postupdate(dst8, src_reg, step_bytes);
        asc_storealign_pack_postupdate(dst16, src_reg, step_bytes);
        asc_storealign_pack_postupdate(dst32, src_reg, step_bytes);
    }
}

__global__ __vector__ void asc_storealign_pack_postupdate_kernel(
    __gm__ uint8_t* dst, __gm__ uint32_t* src)
{
    asc_init();
    __ubuf__ uint8_t dst_local[OUTPUT_BYTES];
    __ubuf__ uint32_t src_local[MASK_BYTES / sizeof(uint32_t)];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub(dst_local, dst, OUTPUT_BYTES);
    asc_copy_gm2ub(src_local, src, MASK_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    // 源掩码寄存器共32字节，隔位压缩后每轮输出16字节。
    uint32_t one_repeat_size = PACKED_BYTES;
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<pack_postupdate_vf>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm(dst, dst_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> src(MASK_BYTES / sizeof(uint32_t), 0x55555555u);
    std::vector<uint8_t> output(OUTPUT_BYTES, SENTINEL);
    std::vector<uint8_t> golden(OUTPUT_BYTES, SENTINEL);
    // 每次固定写满PACKED_BYTES：同时校验有效数据与末轮填充，块间和分区末尾的哨兵保持不变。
    for (uint32_t base = 0; base < OUTPUT_BYTES; base += SECTION_BYTES) {
        for (uint32_t repeat = 0; repeat < REPEAT_TIME; ++repeat) {
            for (uint32_t i = 0; i < PACKED_BYTES; ++i) {
                golden[base + repeat * STEP_BYTES + i] = 0xff;
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* mask_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&mask_device), MASK_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(mask_device, MASK_BYTES, src.data(), MASK_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_storealign_pack_postupdate_kernel<<<1, 0>>>(output_device, mask_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_storealign_pack_postupdate passed."
                         : "[Failed] asc_storealign_pack_postupdate output mismatch.")
              << std::endl;
    aclrtFree(mask_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
