# asc_set_hf32_round_mode

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

头文件路径为：`"c_api/cube_compute/cube_compute.h"`。

本接口用于设置HF32模式舍入方式，使用该接口前需要先使用[asc_enable_hf32](asc_enable_hf32.md)开启HF32模式。

## 函数原型

```c
__aicore__ inline void asc_set_hf32_round_mode(asc_hf32_round_mode hf32_round_mode)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
|:-------|:----------|:------|
| hf32_round_mode | 输入 | HF32舍入模式控制入参，[asc_hf32_round_mode](../defs/enum/asc_hf32_round_mode.md)枚举类型，支持如下2种枚举值：<br>&nbsp;&nbsp;&bull; NEAREST_AWAY：FP32将以向最接近的值舍入，平局时远离零的方式舍入为HF32。<br>&nbsp;&nbsp;&bull; NEAREST_EVEN：FP32将以向最接近的值舍入，平局时向偶数舍入的方式舍入为HF32。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 开启HF32模式后，若不调用本接口，则默认使用`NEAREST_EVEN`代表的舍入模式。
- 本接口需在矩阵乘加指令（[asc_mmad](asc_mmad.md)）执行前调用，以此来确保模式配置在矩阵乘加计算过程中生效。
- 本接口配置的舍入模式仅在HF32模式开启期间生效，需先开启HF32模式再调用本接口，否则舍入模式配置不产生实际作用。
- 舍入模式配置后会持续生效，HF32模式关闭后再次开启仍将沿用上次的舍入模式配置，如需切换舍入模式，请重新调用本接口。

<!-- npu="950" id8 -->
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

__global__ __cube__ void asc_set_hf32_round_mode_kernel(
    __gm__ float* a, __gm__ float* b, __gm__ float* output, uint32_t nearest_away)
{
    asc_init();
    __cbuf__ float a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ float a_l0[ELEMENTS];
    __cb__ float b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, DIM * sizeof(float), 0, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, DIM * sizeof(float), 0, DIM, DIM, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 2, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 2, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_enable_hf32();
    asc_set_hf32_round_mode(nearest_away ? asc_hf32_round_mode::NEAREST_AWAY
                                         : asc_hf32_round_mode::NEAREST_EVEN);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, DIM, DIM, DIM, DIM, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_ALL);
}

void print_row(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    constexpr float tie = 1.0f + 0.5f / 1024.0f;
    std::vector<float> a(ELEMENTS), b(ELEMENTS), nearest_away(ELEMENTS), nearest_even(ELEMENTS);
    std::vector<float> away_golden(ELEMENTS), even_golden(ELEMENTS);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = tie;
        b[i * DIM + i] = 1.0f;
        away_golden[i * DIM + i] = 1.0f + 1.0f / 1024.0f;
        even_golden[i * DIM + i] = 1.0f;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float *a_device = nullptr, *b_device = nullptr, *away_device = nullptr, *even_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&away_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&even_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, ELEMENTS * sizeof(float), a.data(), ELEMENTS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, ELEMENTS * sizeof(float), b.data(), ELEMENTS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_hf32_round_mode_kernel<<<1, 0>>>(a_device, b_device, away_device, 1);
    aclrtSynchronizeDevice();
    asc_set_hf32_round_mode_kernel<<<1, 0>>>(a_device, b_device, even_device, 0);
    aclrtSynchronizeDevice();
    aclrtMemcpy(nearest_away.data(), ELEMENTS * sizeof(float), away_device, ELEMENTS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(nearest_even.data(), ELEMENTS * sizeof(float), even_device, ELEMENTS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Input tie row 0", a);
    print_row("NEAREST_AWAY row 0", nearest_away);
    print_row("NEAREST_EVEN row 0", nearest_even);
    const bool passed = nearest_away == away_golden && nearest_even == even_golden;
    std::cout << (passed ? "[Success] asc_set_hf32_round_mode selects tie rounding."
                         : "[Failed] asc_set_hf32_round_mode result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(away_device); aclrtFree(even_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
