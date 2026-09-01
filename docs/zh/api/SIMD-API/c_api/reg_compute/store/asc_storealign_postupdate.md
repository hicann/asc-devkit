# asc_storealign_postupdate

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

将矢量数据寄存器或掩码寄存器中的数据连续对齐搬出到Unified Buffer（UB），启用Post Update模式在搬出后自动累加目的地址指针，便于硬件循环内连续多次调用时无需手动维护目的地址。当源操作数为矢量数据寄存器时，单次搬出量为VL（256字节），支持配置掩码用于指示参与搬出的元素，掩码为1的元素写入目的地址，掩码为0的元素保留目的地址原数据。当源操作数为掩码寄存器时，单次搬出量为掩码寄存器长度（32字节），不支持配置掩码。搬运过程中数据格式与内容保持不变。本接口在Vector Function（`__simd_vf__`标记的函数）内使用，目的操作数为UB地址。

矢量数据寄存器搬出场景提供以下两种模式：

- **立即数偏移搬出模式**：将矢量数据寄存器中的数据连续对齐搬出到当前目的地址，搬出完成后基于立即数累加目的地址指针，单位为元素。
- **非连续对齐搬出模式**：单条指令搬运8个DataBlock，一个DataBlock数据量为32字节。通过参数配置相邻DataBlock的步长。搬出完成后基于立即数累加目的地址指针，单位为32字节。

掩码寄存器搬出场景提供以下一种模式：

- **立即数偏移搬出模式**：将掩码寄存器中的数据连续对齐搬出到当前目的地址，搬出完成后基于立即数累加目的地址指针，单位为字节。

本接口仅在AIV上生效。

## 函数原型

### 矢量数据寄存器立即数偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ <dtype>*& dst_align32b,
                                                       vector_<dtype> src,
                                                       int32_t offset,
                                                       vector_bool mask)
```

#### dtype支持数据类型

dtype支持的数据类型为`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int8_t*& dst_align32b,
                                                       vector_int8_t src,
                                                       int32_t offset,
                                                       vector_bool mask)
```

### 矢量数据寄存器非连续对齐搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ <dtype>*& dst_align32b,
                                                       vector_<dtype> src,
                                                       uint16_t block_stride,
                                                       uint16_t repeat_stride,
                                                       vector_bool mask)
```

#### dtype支持数据类型

dtype支持的数据类型为`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int8_t*& dst_align32b,
                                                       vector_int8_t src,
                                                       uint16_t block_stride,
                                                       uint16_t repeat_stride,
                                                       vector_bool mask)
```

### 掩码寄存器立即数偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ <dtype>*& dst,
                                                       vector_bool src,
                                                       int32_t offset)
```

#### dtype支持数据类型

dtype支持的数据类型为`uint8_t`、`uint16_t`、`uint32_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint8_t*& dst,
                                                       vector_bool src,
                                                       int32_t offset)
```

## 参数说明

### 矢量数据寄存器立即数偏移搬出模式

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst_align32b | 输入/输出 | 目的操作数（矢量）的起始地址，按指针引用传入（`__ubuf__ <dtype>*&`）。dtype须与src一致，起始地址需32字节对齐。搬出完成后该指针由硬件自动更新。 |
| src | 输入 | 源操作数（矢量数据寄存器）。dtype须与dst_align32b一致。 |
| offset | 输入 | 目的地址更新量，类型为`int32_t`，单位为元素。接口执行后，目的地址偏移`offset × sizeof(dtype)`字节。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与搬出的元素。对应位置为1时参与搬出，为0时不参与搬出。需通过掩码设置接口预先赋值后再传入。 |

### 矢量数据寄存器非连续对齐搬出模式

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst_align32b | 输入/输出 | 目的操作数（矢量）的起始地址，按指针引用传入（`__ubuf__ <dtype>*&`）。dtype须与src一致，起始地址需32字节对齐。搬出完成后该指针由硬件自动更新。 |
| src | 输入 | 源操作数（矢量数据寄存器）。dtype须与dst_align32b一致。 |
| block_stride | 输入 | 目的操作数相邻数据块之间起始地址的步长，类型为`uint16_t`，单位为32字节。 |
| repeat_stride | 输入 | 目的地址Post Update后更新的偏移量，类型为`uint16_t`，单位为32字节。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与搬出的元素。对应位置为1时参与搬出，为0时不参与搬出。需通过掩码设置接口预先赋值后再传入。 |

### 掩码寄存器立即数偏移搬出模式

**表 3**  参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输入/输出 | 目的操作数（矢量）的起始地址，按指针引用传入（`__ubuf__ <dtype>*&`），起始地址需32字节对齐。搬出完成后该指针由硬件自动更新。 |
| src | 输入 | 源操作数（掩码寄存器）。 |
| offset | 输入 | 目的地址更新量，类型为`int32_t`，单位为字节。接口执行后，dst指针的底层UB地址偏移offset字节。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- dst_align32b/dst起始地址需32字节对齐，否则会报错。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈+2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作Data Cache，可用容量进一步减少）。目的操作数的实际访问地址和Post Update后的地址不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 矢量数据寄存器搬出场景

- mask需通过掩码设置接口预先赋值后再传入；未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- mask比特位为0的位置采用保持模式：dst_align32b对应位置保持原值不变，不写入src数据。
- 步长参数模式下，通过block_stride和repeat_stride确定的实际访问地址需落在UB地址范围内，且Post Update后的dst_align32b新地址仍需32字节对齐，否则会报错。

### 掩码寄存器搬出场景

- Post Update后的dst新地址仍需32字节对齐，否则下一次调用本接口会报错。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510; ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"
namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t REGISTER_BYTES = 256;
constexpr uint32_t BUFFER_BYTES = 256;
__simd_vf__ inline void Store(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input)
{
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    for (uint16_t repeat = 0; repeat < 1; ++repeat) {
      vector_uint8_t src;
      asc_loadalign(src, input);
      asc_storealign_postupdate(output, src, 0, mask);
    }
}

__global__ __vector__ void asc_storealign_postupdate_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES], input_local[BUFFER_BYTES];
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    Store(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_BYTES), output(BUFFER_BYTES, 0xff);
    for (uint32_t i = 0; i < BUFFER_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_storealign_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = input == output;
    std::cout << (passed ? "[Success] asc_loadalign completed." : "[Failed] asc_loadalign output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
