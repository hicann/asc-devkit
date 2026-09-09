# asc_set_l0c_copy_config

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

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

本接口用于设置L0C Buffer搬出场景下随路tensor量化、随路tensor激活、UnitFlag功能的相关配置，具体功能说明如下：

- 功能一：设置随路tensor量化参数的起始地址索引。当L0C Buffer搬出接口启用tensor量化模式时，硬件将从Fixpipe Buffer中按本接口设置的起始地址索引读取N维量化参数，N代表搬出矩阵的列数。
- 功能二：设置随路tensor激活参数的起始地址索引。当L0C Buffer搬出接口启用tensor激活模式时，硬件将从Fixpipe Buffer中按本接口设置的起始地址索引读取N维激活参数，N代表搬出矩阵的列数。
- 功能三：UnitFlag全局状态初始化：当需要关闭L0C Buffer搬出接口的UnitFlag功能，或矩阵乘加计算与搬出的数据量不一致时，本接口可以将UnitFlag功能的全局状态设置为关闭。

本接口为矩阵搬出相关配置接口，仅在AIC上生效。

## 函数原型

```c
__aicore__ inline void asc_set_l0c_copy_config(uint64_t relu_pre_addr,
                                               uint64_t quant_pre_addr,
                                               bool is_clean_unit_flag);
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| relu_pre_addr | 输入 | tensor激活参数在Fixpipe Buffer中的起始地址索引，地址索引的计算方式为：`tensor激活参数在Fixpipe Buffer中的起始地址 / 64`，单位为64字节，tensor激活参数在Fixpipe Buffer中的起始地址需64字节对齐。仅当后续L0C Buffer搬出接口开启tensor激活模式时，本参数生效。 |
| quant_pre_addr | 输入 | tensor量化参数在Fixpipe Buffer中的起始地址索引，地址索引的计算方式为：`tensor量化参数在Fixpipe Buffer中的起始地址 / 128`，单位为128字节，tensor量化参数在Fixpipe Buffer中的起始地址需128字节对齐。仅当后续L0C Buffer搬出接口开启tensor量化模式时，本参数生效。 |
| is_clean_unit_flag | 输入 | 是否初始化L0C Buffer所有存储块的UnitFlag标志位。<br>&nbsp;&nbsp;&bull; `false`：不初始化。<br>&nbsp;&nbsp;&bull; `true`：初始化。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口仅在AIC上生效，在AIV上调用将直接返回。
<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，用于存储tensor量化参数的Fixpipe Buffer大小为4KB，用于存储tensor激活参数的Fixpipe Buffer大小为2KB，两块存储空间相互独立，地址均从0开始。tensor量化参数和tensor激活参数的起始地址和占用空间不能超出对应Buffer的范围。
<!-- end id8 -->
<!-- npu="A3,910b" id9 -->
- 针对如下产品型号：

  <!-- npu="A3" id10 -->
  Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id10 -->
  <!-- npu="910b" id11 -->
  Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id11 -->

  用于存储tensor量化参数的Fixpipe Buffer大小为2KB，用于存储tensor激活参数的Fixpipe Buffer大小为1KB，两块存储空间相互独立，地址均从0开始。tensor激活参数和tensor激活参数的起始地址和占用空间不能超出对应Buffer的范围。
<!-- end id9 -->
- 若L0C Buffer搬出接口[asc_copy_l0c2gm](../cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2l1](../cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md)、[asc_copy_l0c2ub](../cube_datamove/asc_copy_l0c2ub.md)需要开启随路量化或随路激活功能，则本接口需在搬出接口前调用。
- 本接口的配置会持续生效，直至再次调用本接口覆盖。不同搬出任务使用的配置不同时，需在对应搬出接口执行前重新配置，每次调用均会同时覆盖三个参数。

<!-- npu="950" id12 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

以下调用示例代码仅Ascend 950PR/Ascend 950DT产品支持。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DIM = 16, ELEMENTS = DIM * DIM;
constexpr uint16_t HALF_ONE = 0x3c00, HALF_THREE = 0x4200;
constexpr uint64_t SIGNED_SCALE_TWO = 0x40000000ULL | (1ULL << 46);

__global__ __cube__ void asc_set_l0c_copy_config_kernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ uint64_t* scales, __gm__ int8_t* output)
{
    asc_init();
    __cbuf__ half a_l1[ELEMENTS], b_l1[ELEMENTS];
    __cbuf__ uint64_t scale_l1[DIM];
    __fbuf__ uint64_t scale_fb[DIM];
    __ca__ half a_l0[ELEMENTS];
    __cb__ half b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), DIM * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), DIM * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_copy_gm2l1(reinterpret_cast<__cbuf__ void*>(scale_l1), reinterpret_cast<__gm__ void*>(scales), 1, 4,
        asc_channel_pad_mode::NONE, 0, 0);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_MTE2, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_FIX, EVENT_ID0);
    asc_copy_l12fb(reinterpret_cast<__fbuf__ void*>(scale_fb), reinterpret_cast<__cbuf__ void*>(scale_l1), 1, 2, 0, 0);
    const uint64_t scale_address = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(scale_fb)) / 128;
    asc_set_l0c_copy_config(0, scale_address, false);
    asc_sync_pipe(PIPE_FIX);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, DIM, DIM, DIM, DIM,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE,
        asc_quant_mode::VQF322B8_PRE, asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync_pipe(PIPE_ALL);
}

void print_row(const char* label, const std::vector<int8_t>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << +data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(ELEMENTS), b(ELEMENTS);
    std::vector<uint64_t> scales(DIM, SIGNED_SCALE_TWO);
    std::vector<int8_t> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = HALF_ONE;
        b[i * DIM + i] = HALF_THREE;
        golden[i * DIM + i] = 6;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    uint64_t* scales_device = nullptr;
    int8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&scales_device), scales.size() * sizeof(uint64_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(scales_device, scales.size() * sizeof(uint64_t), scales.data(), scales.size() * sizeof(uint64_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_l0c_copy_config_kernel<<<1, 0>>>(a_device, b_device, scales_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size(), output_device, output.size(), ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Quantized output row 0", output);
    print_row("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_l0c_copy_config applied tensor quantization scale 2."
                         : "[Failed] asc_set_l0c_copy_config result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(scales_device); aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id12 -->
