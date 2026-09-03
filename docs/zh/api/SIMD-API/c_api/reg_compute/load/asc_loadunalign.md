# asc_loadunalign

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

头文件路径为：`"c_api/reg_compute/load/loadunalign.h"`。

从Unified Buffer（UB）中按dtype对齐的起始地址读取VL长度连续数据，并搬入矢量数据寄存器。本接口将[asc_loadunalign_pre](asc_loadunalign_pre.md)中的非对齐寄存器（32字节）中缓存的前置数据与从UB读取的后续数据拼接，得到VL长度数据并搬入矢量数据寄存器。连续搬入时，需要在每次调用前手动更新源地址。

设本次实际读取的起始字节地址为`src_start_addr`，结束字节地址为`src_end_addr`，其中`src_end_addr = src_start_addr + VL`；将`src_start_addr`向低地址方向对齐到32字节边界，得到`aligned_src_start_addr`。`asc_loadunalign_pre`将字节地址范围`[aligned_src_start_addr, aligned_src_start_addr + 32)`的数据缓存到非对齐寄存器，本接口将该缓存与从UB读取的后续数据拼接，得到字节地址范围`[src_start_addr, src_end_addr)`的数据。

## 函数原型

```c
__simd_callee__ inline void asc_loadunalign(vector_<dtype>& dst,
                                            vector_load_unalign& src0,
                                            __ubuf__ <dtype>* src1)
```

### dtype支持数据类型

dtype支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`。当dtype为`int4b_t`时，`dst`的实际类型为`vector_int4x2_t`。

### 函数原型典型示例

```c
// 示例：int8_t类型。
__simd_callee__ inline void asc_loadunalign(vector_int8_t& dst,
                                            vector_load_unalign& src0,
                                            __ubuf__ int8_t* src1)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的矢量数据寄存器。dtype必须与`src1`一致，搬入VL长度数据。 |
| src0 | 输入 | 非对齐寄存器，大小为32字节，保存本次搬入所需的前置数据缓存。 |
| src1 | 输入 | 本次搬入的有效起始UB地址，必须按dtype对齐。本接口不会更新该指针。 |

## 返回值说明

无

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- `src1`起始地址必须按dtype对齐，且实际访问范围必须在UB地址空间内，否则会报错。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈 + 2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB～128KB作Data Cache，可用容量进一步减少）。UB地址偏移后不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在UB地址重叠，必须插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。
- 每次搬入前必须调用[asc_loadunalign_pre](asc_loadunalign_pre.md)中的源地址预处理模式进行初始化，且初始化接口和本接口必须使用相同的`src1`。

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

constexpr uint32_t DATA_ELEMENTS = 512;
constexpr uint32_t BUFFER_ELEMENTS = DATA_ELEMENTS + 32;
constexpr uint32_t BUFFER_BYTES = BUFFER_ELEMENTS * sizeof(uint8_t);

__simd_vf__ inline void asc_loadunalign_vf(__ubuf__ uint8_t* dst, __ubuf__ uint8_t* src,
                                           uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_load_unalign load_unalign_reg;
    vector_store_unalign store_unalign_reg;
    vector_uint8_t dst_reg;
    asc_loadunalign_pre(load_unalign_reg, src);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadunalign(dst_reg, load_unalign_reg, src + i * one_repeat_size);
        asc_storeunalign(dst + i * one_repeat_size, store_unalign_reg, dst_reg, one_repeat_size);
    }
    asc_storeunalign_post(dst, store_unalign_reg, repeat_time * one_repeat_size);
}

__global__ __vector__ void asc_loadunalign_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[BUFFER_ELEMENTS];
    __ubuf__ uint8_t output_local[BUFFER_ELEMENTS];
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    uint16_t one_repeat_size = asc_get_vf_len() / sizeof(uint8_t);
    uint16_t repeat_time = ceil_division(DATA_ELEMENTS, one_repeat_size);
    asc_vf_call<asc_loadunalign_vf>(output_local + 1, input_local + 1, one_repeat_size, repeat_time);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_ELEMENTS);
    std::vector<uint8_t> output(BUFFER_ELEMENTS, 0xff);
    for (uint16_t i = 0; i < BUFFER_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadunalign_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint16_t i = 0; i < DATA_ELEMENTS; ++i) passed = passed && output[i + 1] == input[i + 1];
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadunalign example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
