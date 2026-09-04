# asc_mem_bar

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

头文件路径为：`"c_api/reg_compute/reg_sync.h"`。

Reg矢量计算内不同流水线之间的同步指令。该同步指令指定源流水线和目的流水线，如下图所示，目的流水线将等待源流水线上所有指令完成才进行执行。
读写场景下，当读指令使用的寄存器和写指令使用的寄存器相同时，可以触发寄存器保序，指令将会按照代码顺序执行，不需要插入同步指令，而当使用的寄存器不同时，如果要确保读写指令执行，则需要插入同步指令。写写场景同理。

![SIMD_VF函数内流水线等待示意图.png](../../figures/simd_vf_pipeline_wait_diagram.png)

本接口仅在AIV上生效。

## 函数原型

```cpp
__simd_callee__ inline void asc_mem_bar(MEM_TYPE mem_type)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| mem_type | 输入 | 同步流水线的类型，类型为`MEM_TYPE`，取值范围见表2 `mem_type`取值说明。 |

**表2** 本接口支持的mem_type取值范围说明（源流水线/目的流水线表示的含义见表3 Reg计算流水线说明）

| 值  | 源流水线 | 目的流水线 |
| ------ | -------- | -------- |
| `VV_ALL` | `VEC_ALL` | `VEC_ALL` |
| `VST_VLD` | `VEC_STORE` | `VEC_LOAD` |
| `VLD_VST` | `VEC_LOAD` | `VEC_STORE` |
| `VST_VST` | `VEC_STORE` | `VEC_STORE` |
| `VS_ALL` | `VEC_ALL` | `SCALAR_ALL` |
| `VST_LD` | `VEC_STORE` | `SCALAR_LOAD` |
| `VLD_ST` | `VEC_LOAD` | `SCALAR_STORE` |
| `VST_ST` | `VEC_STORE` | `SCALAR_STORE` |
| `SV_ALL` | `SCALAR_ALL` | `VEC_ALL` |
| `ST_VLD` | `SCALAR_STORE` | `VEC_LOAD` |
| `LD_VST` | `SCALAR_LOAD` | `VEC_STORE` |
| `ST_VST` | `SCALAR_STORE` | `VEC_STORE` |

**表3** Reg计算流水线说明

| 流水线 | 含义 |
| ------ | -------- |
| `VEC_STORE` | `SIMD_VF`函数内矢量写UB流水线。<br>对应寄存器到UB的搬运指令，例如[asc_storealign](../store/asc_storealign.md)。 |
| `VEC_LOAD` | `SIMD_VF`函数内矢量读UB流水线。<br>对应UB到寄存器的搬运指令，例如[asc_loadalign](../load/asc_loadalign.md)。 |
| `SCALAR_STORE` | `SIMD_VF`函数内标量写UB流水线。 |
| `SCALAR_LOAD` | `SIMD_VF`函数内标量读UB流水线。 |
| `VEC_ALL` | `SIMD_VF`函数内所有矢量读写UB流水线。 |
| `SCALAR_ALL` | `SIMD_VF`函数内所有标量读写UB流水线。 |

## 返回值说明

无

## 约束说明

### 通用约束

- 非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。

### 指令约束

- 读写依赖的场景下，如果读指令和写指令使用的寄存器相同，会触发寄存器保序，指令将会按照代码顺序执行，无需额外插入同步指令。
- 冗余的同步指令会导致性能下降，可以通过外提出循环或者循环切分避免多次调用同步指令。
- 当Unified Buffer（UB）数据存在依赖时，才需要插入同步，判断是否有依赖取决于指令读写的内存是否有重叠。部分搬运指令读写内存的模式如下：
    - [asc_gather](../ub_gather/asc_gather.md)/[asc_scatter](../scatter/asc_scatter.md)/[asc_gather_datablock](../ub_gather/asc_gather_datablock.md)等指令取决于`index`地址偏移。
    - [asc_loadalign_brc_elem](../load/asc_loadalign_brc_elem.md)单条指令读32B数据后将第一个元素进行广播。

## 调用示例

如下示例中，前一次循环通过`asc_storealign`将累加结果写入UB，后一次循环通过`asc_loadalign`从同一块UB地址空间读取该结果。由于两条指令使用不同的矢量数据寄存器，后一次循环读取UB前需调用`asc_mem_bar(VST_VLD)`，等待前一次循环写入UB完成。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t VECTOR_ELEMENT_COUNT = 64;
constexpr uint32_t BLOCK_COUNT = 4;
constexpr uint32_t INPUT_COUNT = VECTOR_ELEMENT_COUNT * BLOCK_COUNT;
constexpr uint32_t INPUT_BYTES = INPUT_COUNT * sizeof(float);
constexpr uint32_t OUTPUT_BYTES = VECTOR_ELEMENT_COUNT * sizeof(float);

__simd_vf__ inline void accumulate_vf(__ubuf__ float* data)
{
    vector_float src0;
    vector_float src1;
    vector_float dst;
    vector_bool mask = asc_create_mask_b32(PAT_ALL);

    for (uint32_t i = 1; i < BLOCK_COUNT; ++i) {
        asc_mem_bar(VST_VLD);
        asc_loadalign(src0, data);
        asc_loadalign(src1, data + i * VECTOR_ELEMENT_COUNT);
        asc_add(dst, src0, src1, mask);
        asc_storealign(data, dst, mask);
    }
}

__global__ __vector__ void asc_mem_bar_kernel(__gm__ float* output, __gm__ float* input)
{
    asc_init();
    __ubuf__ float data_local[INPUT_COUNT];
    asc_copy_gm2ub_align(data_local, input, INPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    accumulate_vf(data_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, data_local, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(INPUT_COUNT);
    std::vector<float> output(VECTOR_ELEMENT_COUNT, 0.0f);
    std::vector<float> expected(VECTOR_ELEMENT_COUNT, 0.0f);
    for (uint32_t block = 0; block < BLOCK_COUNT; ++block) {
        for (uint32_t i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
            input[block * VECTOR_ELEMENT_COUNT + i] = static_cast<float>(block * 10U + i);
            expected[i] += input[block * VECTOR_ELEMENT_COUNT + i];
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* input_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_mem_bar_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
        if (std::fabs(output[i] - expected[i]) > 1e-6f) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_mem_bar completed."
                         : "[Failed] asc_mem_bar output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
