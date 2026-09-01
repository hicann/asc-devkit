# asc_storealign_pack_postupdate

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

将矢量数据寄存器或掩码寄存器中的数据压缩搬出到Unified Buffer（UB），写入完成后启用Post Update模式自动累加目的地址指针，便于硬件循环内连续多次调用时无需手动维护目的地址。当源操作数为矢量数据寄存器时，`mask`用于指示参与搬出的元素，`mask`对应位置为1时，将有效元素的低半部分bit数据写入压缩后对应的目的位置。`mask`对应位置为0时，压缩后对应的目的位置保持原值。当源操作数为掩码寄存器时，每间隔1 bit丢弃1 bit，将保留的bit连续压缩存储到目的地址，掩码寄存器搬出场景不支持配置掩码。本接口在Vector Function（`__simd_vf__`标记的函数）内使用，目的操作数为UB地址。

本接口提供以下两种功能模式：

- **矢量数据寄存器搬出模式**：`offset`单位为元素。搬出完成后，目的地址累加`offset × sizeof(dtype)`字节。
- **掩码寄存器搬出模式**：`offset`单位为字节。搬出完成后，目的地址累加`offset`字节。

本接口仅在AIV上执行有效。

## 函数原型

### 矢量数据寄存器搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ <dtype>*& dst_align32b,
                                                           vector_<dtype> src,
                                                           int32_t offset,
                                                           vector_bool mask)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t`。对应矢量寄存器类型为`vector_<dtype>`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint32_t*& dst_align32b,
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

`dtype`支持的数据类型为`uint8_t`、`uint16_t`、`uint32_t`。

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
| dst_align32b | 输入/输出 | 目的操作数的起始地址，按指针引用传入。类型为`__ubuf__ <dtype>*&`，起始地址需32字节对齐。搬出完成后，该指针由硬件自动更新。 |
| src | 输入 | 源操作数（矢量数据寄存器）。`dtype`须与`dst_align32b`一致。 |
| offset | 输入 | 地址偏移量，类型为`int32_t`，单位为元素。接口执行后，目的地址累加`offset × sizeof(dtype)`字节。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与搬出的元素。对应位置为1时参与搬出，为0时不参与搬出。需通过掩码设置接口预先赋值后再传入。 |

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

- 本接口非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，源操作数为矢量数据寄存器或掩码寄存器，目的操作数为UB地址。UB地址空间外的指针不可作为目的地址传入。
- UB总容量为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈和2KB Ascend C预留空间，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作为Data Cache，可用容量进一步减少）。目的操作数的写出范围和Post Update后的地址不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 矢量数据寄存器压缩搬出场景

- `dst_align32b`起始地址需32字节对齐。
- `mask`需通过掩码设置接口预先赋值后再传入。未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- `mask`比特位为1的源操作数元素参与搬出，并将低半部分bit数据写入压缩后对应的目的位置；为0的元素不参与搬出，压缩后对应的目的位置保持原值。
- `offset`的单位为元素。Post Update后的`dst_align32b`新地址仍需32字节对齐，且本次写出范围和更新后的地址均需落在UB可用容量范围内。

### 掩码寄存器压缩搬出场景

- `dst`起始地址需16字节对齐。
- `offset`的单位为字节。Post Update后的`dst`新地址仍需16字节对齐，且本次写出范围和更新后的地址均需落在UB可用容量范围内。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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
constexpr uint32_t MASK_BYTES = 32;
constexpr uint32_t PACKED_BYTES = 16;
constexpr uint32_t REPEAT_TIMES = 2;
constexpr uint32_t STEP_BYTES = 32;
constexpr uint32_t SECTION_BYTES = REPEAT_TIMES * STEP_BYTES;
constexpr uint32_t OUTPUT_BYTES = 3 * SECTION_BYTES;
constexpr uint8_t SENTINEL = 0xa5;

__simd_vf__ inline void pack_postupdate_vf(__ubuf__ uint8_t* output,
                                           __ubuf__ uint32_t* mask_input)
{
    vector_bool src;
    asc_loadalign(src, mask_input);

    __ubuf__ uint8_t* dst8 = output;
    __ubuf__ uint16_t* dst16 = reinterpret_cast<__ubuf__ uint16_t*>(output + SECTION_BYTES);
    __ubuf__ uint32_t* dst32 = reinterpret_cast<__ubuf__ uint32_t*>(output + 2 * SECTION_BYTES);
    for (uint32_t repeat = 0; repeat < REPEAT_TIMES; ++repeat) {
        asc_storealign_pack_postupdate(dst8, src, STEP_BYTES);
        asc_storealign_pack_postupdate(dst16, src, STEP_BYTES);
        asc_storealign_pack_postupdate(dst32, src, STEP_BYTES);
    }
}

__global__ __vector__ void asc_storealign_pack_postupdate_kernel(
    __gm__ uint8_t* output, __gm__ uint32_t* mask_input)
{
    asc_init();
    __ubuf__ uint8_t output_local[OUTPUT_BYTES];
    __ubuf__ uint32_t mask_local[MASK_BYTES / sizeof(uint32_t)];
    asc_copy_gm2ub(output_local, output, OUTPUT_BYTES);
    asc_copy_gm2ub(mask_local, mask_input, MASK_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    pack_postupdate_vf(output_local, mask_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm(output, output_local, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> mask_input(MASK_BYTES / sizeof(uint32_t), 0x55555555u);
    std::vector<uint8_t> output(OUTPUT_BYTES, SENTINEL);
    std::vector<uint8_t> expected(OUTPUT_BYTES, SENTINEL);
    for (uint32_t base = 0; base < OUTPUT_BYTES; base += SECTION_BYTES) {
        for (uint32_t repeat = 0; repeat < REPEAT_TIMES; ++repeat) {
            for (uint32_t i = 0; i < PACKED_BYTES; ++i) {
                expected[base + repeat * STEP_BYTES + i] = 0xff;
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* mask_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&mask_device), MASK_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(mask_device, MASK_BYTES, mask_input.data(), MASK_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_storealign_pack_postupdate_kernel<<<1, 0>>>(output_device, mask_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == expected;
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
