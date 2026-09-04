# asc_loadalign_postupdate

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

头文件路径为：`"c_api/reg_compute/load/loadalign.h"`。

从Unified Buffer（UB）中32字节对齐的起始地址读取数据，并搬入矢量数据寄存器或掩码寄存器，搬运过程中数据格式和内容保持不变。接口采用Post Update模式，搬运完成后自动更新源地址指针。本接口提供两种功能模式：

- **立即数偏移搬入模式**：从当前源地址搬入数据，搬入完成后按照指定步长自动更新源地址。
- **非连续对齐搬入模式**：按照指定的数据块间隔，从当前源地址非连续搬入8个`DataBlock`，每个`DataBlock`的数据量为32字节。搬入完成后按照指定步长自动更新源地址。

**图1** 立即数偏移搬入模式

![](../../figures/capi_loadalign_norm.png)

本接口仅在AIV上生效，非AIV调用直接返回。

## 函数原型

### 立即数偏移搬入模式

```c
// 搬入矢量数据寄存器。
__simd_callee__ inline void asc_loadalign_postupdate(vector_<dtype>& dst,
                                                     __ubuf__ <dtype>*& src,
                                                     int32_t offset)
// 搬入掩码寄存器。
__simd_callee__ inline void asc_loadalign_postupdate(vector_bool& dst,
                                                     __ubuf__ uint32_t*& src,
                                                     int32_t offset)
```

#### dtype支持数据类型

dtype支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t`。当dtype为`int4b_t`时，dst的实际类型为`vector_int4x2_t`。

#### 函数原型典型示例

```c
// 示例：float类型。
__simd_callee__ inline void asc_loadalign_postupdate(vector_float& dst,
                                                     __ubuf__ float*& src,
                                                     int32_t offset)
```

### 非连续对齐搬入模式

```c
__simd_callee__ inline void asc_loadalign_postupdate(vector_<dtype>& dst,
                                                     __ubuf__ <dtype>*& src,
                                                     uint16_t block_stride,
                                                     uint16_t repeat_stride,
                                                     vector_bool mask)
```

#### dtype支持数据类型

dtype支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`。当dtype为`int4b_t`时，dst的实际类型为`vector_int4x2_t`。

#### 函数原型典型示例

```c
// 示例：float类型。
__simd_callee__ inline void asc_loadalign_postupdate(vector_float& dst,
                                                     __ubuf__ float*& src,
                                                     uint16_t block_stride,
                                                     uint16_t repeat_stride,
                                                     vector_bool mask)
```

## 参数说明

### 立即数偏移搬入模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的矢量数据寄存器或掩码寄存器。<br>&bull; 当`dst`为矢量数据寄存器，dtype必须与`src`一致，搬入VL长度数据。<br>&bull; 当`dst`为掩码寄存器，搬入VL/8长度数据。 |
| src | 输入/输出 | 源UB地址，实际读取地址必须按32字节对齐。 |
| offset | 输入 | Post Update步长。接口从当前`src`地址完成搬入后更新`src`。<br>&bull; 当`dst`为矢量数据寄存器时，单位为元素个数，`src`增加`offset × sizeof(dtype)`字节。<br>&bull; 当`dst`为掩码寄存器时，单位为字节，`src`指针增加`offset`字节。 |

### 非连续对齐搬入模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的矢量数据寄存器。dtype必须与`src`一致，搬入VL长度数据。 |
| src | 输入/输出 | 源UB地址，实际读取地址必须按32字节对齐。 |
| block_stride | 输入 | 源操作数相邻`DataBlock`之间起始地址的步长，单位为32字节。 |
| repeat_stride | 输入 | Post Update步长，单位为32字节。接口完成本次搬入后，将`src`地址增加`repeat_stride × 32`字节。 |
| mask | 输入 | 掩码寄存器，用于指示在计算过程中哪些元素参与计算。该接口以`DataBlock`为数据搬运单元。<br>&bull; 当`DataBlock`中的任意一个元素被`mask`筛选成有效元素时，该`DataBlock`中所有数据都会搬入至矢量数据寄存器。<br>&bull; 当`DataBlock`中所有元素都被`mask`筛选成无效元素时，该`DataBlock`中的数据不会搬入到矢量数据寄存器，对应位置的元素设置为0，即使UB越界也不会报错。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- 各功能模式下的当前实际读取地址及Post Update更新后的下一次实际读取地址必须按32字节对齐，且实际读取范围必须在UB地址空间内且不越界，否则会报错。非连续对齐搬入模式中，当某个`DataBlock`的`mask`包含的元素全部无效时，该`DataBlock`即使越界也不会报错。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈 + 2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB～128KB作Data Cache，可用容量进一步减少）。UB地址偏移后不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 非连续对齐搬入模式

- 该模式不支持目的操作数为掩码寄存器。
- `mask`需通过[掩码设置接口](../../defs/type/data_type_definition.md#掩码寄存器)预先赋值后再传入；未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- 当dtype为`int64_t`时，由于掩码创建接口不支持b64模式，需要先通过[asc_create_mask_b32](../reg_mask/asc_create_mask.md)生成b32掩码，再通过[asc_unpack_lower](../reg_permute_sel/asc_unpack.md)将其展开为适用于b64数据的掩码后传入本接口。展开后的掩码以连续8个bit为一组，仅每组最低位的bit有效，用于控制对应的一个b64元素。
    ```c
    vector_int64_t src_reg;
    vector_bool mask_b32 = asc_create_mask_b32(PAT_ALL);
    vector_bool mask_b64;
    asc_unpack_lower(mask_b64, mask_b32);
    addr_reg load_offset;
    addr_reg store_offset;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        load_offset = asc_update_addr_reg_b32(elements_vl * 2);
        // 搬出到UB后数据类型为B32，偏移不需要*2
        store_offset = asc_update_addr_reg_b32(elements_vl);
        asc_loadalign(src_reg, input, load_offset);
        asc_storealign_pack(output, src_reg, store_offset, mask_b64);
    }
    ```

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
#include "c_api/asc_simd.h"
#include "acl/acl.h"
namespace {
__aicore__ constexpr uint32_t ceil_division(uint32_t numerator, uint32_t denominator)
{
    return (numerator + denominator - 1) / denominator;
}

template <typename T>
void print_data(const char* label, const std::vector<T>& dst_regs)
{
    std::cout << label << ":";
    const size_t count = dst_regs.size() < 8 ? dst_regs.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +dst_regs[i];
    if (dst_regs.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t BUFFER_ELEMENTS = 512;
constexpr uint32_t BUFFER_BYTES = BUFFER_ELEMENTS * sizeof(uint8_t);
__simd_vf__ inline void asc_loadalign_postupdate_vf(__ubuf__ uint8_t* output,
                                                    __ubuf__ uint8_t* input,
                                                    uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_uint8_t src_reg;
        asc_loadalign_postupdate(src_reg, input, one_repeat_size);
        asc_storealign(output + i * one_repeat_size, src_reg, mask);
    }
}

__global__ __vector__ void asc_loadalign_postupdate_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_ELEMENTS], input_local[BUFFER_ELEMENTS];
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);

    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint16_t one_repeat_size = asc_get_vf_len() / sizeof(uint8_t);
    uint16_t repeat_time = ceil_division(BUFFER_ELEMENTS, one_repeat_size);
    asc_vf_call<asc_loadalign_postupdate_vf>(output_local, input_local, one_repeat_size, repeat_time);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_ELEMENTS), output(BUFFER_ELEMENTS, 0xff);
    for (uint16_t i = 0; i < BUFFER_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = input == output;
    std::cout << (passed ? "[Success] asc_loadalign_postupdate completed."
                         : "[Failed] asc_loadalign_postupdate output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
